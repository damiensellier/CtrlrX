/*
 *
 * Copyright (c) 2008-2011 Erich Hoover
 *
 * libr libbfd Backend - Add resources into ELF binaries using libbfd
 *
 * *** PLEASE READ THE README FILE FOR LICENSE DETAILS SPECIFIC TO THIS FILE ***
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc, 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * To provide feedback, report bugs, or otherwise contact me:
 * ehoover at mines dot edu
 *
 */
#ifdef LINUX
#include "libr.h"

// --- Include BFD headers first so BFD types (bfd, asection) are known. ---
#include <bfd.h>
#include <stdbool.h>

/* File access and metadata types */
#include <sys/types.h> // Needed for mode_t, uid_t, gid_t
#include <fcntl.h>
#include <errno.h>

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// === FIX 1: Resolve the libr_file missing member issue ===
// The compiler reported a redefinition error because 'struct _libr_file' is defined
// in 'libr.h' but without the BFD-specific members, causing the "no member named 'bfd_read'" errors.
// We define a new, internal-only structure that mirrors the opaque type but includes our internal BFD handles.
typedef struct _libr_file_bfd_internal
{
    // These fields must align with the start of the opaque struct _libr_file in libr.h
    char *filename;
    libr_access_t access;

    // BFD specific handles (these are the 'missing' members that must be included here)
    bfd *bfd_read;
    bfd *bfd_write;

    // File metadata specific to the read/write process
    char tempfile[LIBR_TEMPFILE_LEN];
    int fd_handle;
    mode_t filemode;
    uid_t fileowner;
    gid_t filegroup;
} libr_file_internal;

// Define a macro to safely cast the opaque 'libr_file *' to our internal structure,
// allowing access to the BFD-specific members. All file access code uses this macro.
#define BFD_FILE_HANDLE(fh) ((libr_file_internal *)(fh))

// === FIX 2: Resolve the libr_section opaque type issue ===
// The compiler reported 'libr_section' is a 'void *', but the code attempts to access
// BFD members (->name, ->next, ->size). We must cast the opaque pointer on every access.
// We define a macro to safely cast the opaque 'libr_section' (void*) back to the actual BFD type 'asection *'.
#define BFD_SECTION_HANDLE(scn) ((asection *)(scn))


/*
 * Build the libr_file handle for processing with libbfd
 */
libr_intstatus open_handles(libr_file *file_handle, char *filename, libr_access_t access)
{
    bfd *handle = NULL;
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);

    handle = bfd_openr(filename, "default");
    if(!handle)
        RETURN(LIBR_ERROR_OPENFAILED, "Failed to open input file");
    if(!bfd_check_format(handle, bfd_object))
        RETURN(LIBR_ERROR_WRONGFORMAT, "Invalid input file format: not a libbfd object");
    if(bfd_get_flavour(handle) != bfd_target_elf_flavour)
        RETURN(LIBR_ERROR_WRONGFORMAT, "Invalid input file format: not an ELF file");
    bfd_set_error(bfd_error_no_error);
    fh->filename = filename;
    fh->bfd_read = handle;
    fh->access = access;
    if(access == LIBR_READ_WRITE)
    {
        struct stat file_stat;
        int fd;

        /* Check for write permission on the file */
        fd = open(filename, O_WRONLY);
        if(fd == ERROR)
            RETURN(LIBR_ERROR_WRITEPERM, "No write permission for file");
        close(fd);
        /* Obtain the access mode of the input file */
        if(stat(filename, &file_stat) == ERROR)
            RETURN(LIBR_ERROR_NOSIZE, "Failed to obtain file size");
        fh->filemode = file_stat.st_mode;
        fh->fileowner = file_stat.st_uid;
        fh->filegroup = file_stat.st_gid;
        /* Open a temporary file with the same settings as the input file */
        strcpy(fh->tempfile, LIBR_TEMPFILE);
        fh->fd_handle = mkstemp(fh->tempfile);
        handle = bfd_openw(fh->tempfile, bfd_get_target(fh->bfd_read));
        if(!bfd_set_format(handle, bfd_get_format(fh->bfd_read)))
            RETURN(LIBR_ERROR_SETFORMAT, "Failed to set output file format to input file format");
        if(!bfd_set_arch_mach(handle, bfd_get_arch(fh->bfd_read), bfd_get_mach(fh->bfd_read)))
            RETURN(LIBR_ERROR_SETARCH, "Failed to set output file architecture to input file architecture");
        /* twice needed ? */
        if(!bfd_set_format(handle, bfd_get_format(fh->bfd_read)))
            RETURN(LIBR_ERROR_SETFORMAT, "Failed to set output file format to input file format");
        fh->bfd_write = handle;
    }
    else
    {
        fh->fd_handle = 0;
        fh->bfd_write = NULL;
    }
    RETURN_OK;
}

/*
 * Check to see if a symbol should be kept
 */
int keep_symbol(libr_section *sections, libr_section *chkscn)
{
    libr_section *scn;

    /* Check that the section is publicly exposed */
    for(scn = sections; scn != NULL; scn = BFD_SECTION_HANDLE(scn)->next)
    {
        if(scn == chkscn)
        {
            /* if it is, and has size zero, then it was marked for deletion */
            if(
                #ifdef HAVE_BFD_2_34
                bfd_section_size(BFD_SECTION_HANDLE(chkscn)) == 0
                #else
                bfd_get_section_size(BFD_SECTION_HANDLE(chkscn)) == 0
                #endif
            )
            {
                return false;
            }
            return true;
        }
    }
    return true;
}

/*
 * Remove the symbol corresponding to a deleted section
 */
void remove_sections(libr_section *sections, void *symtab_buffer, long *symtab_count)
{
    asymbol **symtab = (asymbol **) symtab_buffer;
    long i, cnt = *symtab_count;

    for(i=0;i<cnt;i++)
    {
        libr_section *chkscn = NULL;
        asymbol *symbol = symtab[i];

        if(symbol != NULL)
        {
            #ifdef HAVE_BFD_2_34
            chkscn = (libr_section *)bfd_asymbol_section(symbol);
            #else
            chkscn = (libr_section *)bfd_get_section(symbol);
            #endif
        }
        if(chkscn != NULL && !keep_symbol(sections, chkscn))
        {
            /* remove the symbol from the table */
            asymbol **tmp = (asymbol **) malloc(sizeof(asymbol *) * (cnt-(i+1)));
            memcpy(&tmp[0], &symtab[i+1], sizeof(asymbol *) * (cnt-(i+1)));
            memcpy(&symtab[i], &tmp[0], sizeof(asymbol *) * (cnt-(i+1)));
            free(tmp);
            cnt--;
        }
    }
    *symtab_count = cnt;
}

int setup_sections(bfd *ihandle, bfd *ohandle)
{
    libr_section *iscn, *oscn;
    bfd_vma vma;

    for(iscn = (libr_section *)ihandle->sections; iscn != NULL; iscn = BFD_SECTION_HANDLE(iscn)->next)
    {
        if(
            #ifdef HAVE_BFD_2_34
            bfd_section_size(BFD_SECTION_HANDLE(iscn)) == 0
            #else
            bfd_get_section_size(BFD_SECTION_HANDLE(iscn)) == 0
            #endif
        )
        {
            continue; /* Section has been marked for deletion */
        }
        /* Use SEC_LINKER_CREATED to ask the libbfd backend to take care of configuring the section */

        // Keep the ARM_ATTRIBUTES section type intact on armhf systems
        // If this is not done, readelf -A will not print any architecture information for the modified library,
        // and ldd will report that the library cannot be found
        if (strcmp(BFD_SECTION_HANDLE(iscn)->name, ".ARM.attributes") == 0)
        {
            oscn = (libr_section *)bfd_make_section_anyway_with_flags(ohandle, BFD_SECTION_HANDLE(iscn)->name, BFD_SECTION_HANDLE(iscn)->flags);
        }
        else
        {
            oscn = (libr_section *)bfd_make_section_anyway_with_flags(ohandle, BFD_SECTION_HANDLE(iscn)->name, BFD_SECTION_HANDLE(iscn)->flags | SEC_LINKER_CREATED);
        }
        if(oscn == NULL)
        {
            printf("failed to create out section: %s\n", bfd_errmsg(bfd_get_error()));
            return false;
        }
        if(
            #ifdef HAVE_BFD_2_34
            !bfd_set_section_size(BFD_SECTION_HANDLE(oscn), BFD_SECTION_HANDLE(iscn)->size)
            #else
            !bfd_set_section_size(ohandle, BFD_SECTION_HANDLE(oscn), BFD_SECTION_HANDLE(iscn)->size)
            #endif
        )
        {
            printf("failed to set data size: %s\n", bfd_errmsg(bfd_get_error()));
            return false;
        }
        #ifdef HAVE_BFD_2_34
        vma = bfd_section_vma(BFD_SECTION_HANDLE(iscn));
        #else
        vma = bfd_section_vma(ihandle, BFD_SECTION_HANDLE(iscn));
        #endif
        if(
            #ifdef HAVE_BFD_2_34
            !bfd_set_section_vma(BFD_SECTION_HANDLE(oscn), vma)
            #else
            !bfd_set_section_vma(ohandle, BFD_SECTION_HANDLE(oscn), vma)
            #endif
        )
        {
            printf("failed to set virtual memory address: %s\n", bfd_errmsg(bfd_get_error()));
            return false;
        }
        BFD_SECTION_HANDLE(oscn)->lma = BFD_SECTION_HANDLE(iscn)->lma;
        if(
            #ifdef HAVE_BFD_2_34
            !bfd_set_section_alignment(BFD_SECTION_HANDLE(oscn), bfd_section_alignment(BFD_SECTION_HANDLE(iscn)))
            #else
            !bfd_set_section_alignment(ohandle, BFD_SECTION_HANDLE(oscn), bfd_section_alignment(ihandle, BFD_SECTION_HANDLE(iscn)))
            #endif
        )
        {
            printf("failed to compute section alignment: %s\n", bfd_errmsg(bfd_get_error()));
            return false;
        }
        BFD_SECTION_HANDLE(oscn)->entsize = BFD_SECTION_HANDLE(iscn)->entsize;
        BFD_SECTION_HANDLE(iscn)->output_section = BFD_SECTION_HANDLE(oscn);
        BFD_SECTION_HANDLE(iscn)->output_offset = vma;
        if(!bfd_copy_private_section_data(ihandle, BFD_SECTION_HANDLE(iscn), ohandle, BFD_SECTION_HANDLE(oscn)))
        {
            printf("failed to compute section alignment: %s\n", bfd_errmsg(bfd_get_error()));
            return false;
        }
    }
    return true;
}

/*
 * Go through the rather complicated process of using libbfd to build the output file
 */
int build_output(libr_file *file_handle)
{
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);
    void *symtab_buffer = NULL, *reloc_buffer = NULL, *buffer = NULL;
    bfd_size_type symtab_size, reloc_size, size;
    bfd *ohandle = fh->bfd_write;
    bfd *ihandle = fh->bfd_read;
    long symtab_count, reloc_count;
    libr_section *iscn, *oscn;

    if(!bfd_set_start_address(ohandle, bfd_get_start_address(ihandle)))
    {
        printf("failed to set start address: %s\n", bfd_errmsg(bfd_get_error()));
        return false;
    }
    if(!bfd_set_file_flags(ohandle, bfd_get_file_flags(ihandle)))
    {
        printf("failed to set file flags: %s\n", bfd_errmsg(bfd_get_error()));
        return false;
    }
    /* Setup the sections in the output file */
    if(!setup_sections(ihandle, ohandle))
        return false; /* error already printed */
    if(!bfd_copy_private_header_data(ihandle, ohandle))
    {
        printf("failed to copy header: %s\n", bfd_errmsg(bfd_get_error()));
        return false; /* failed to create section */
    }
    /* Get the old symbol table */
    if((bfd_get_file_flags(ihandle) & HAS_SYMS) == 0)
    {
        printf("file has no symbol table: %s\n", bfd_errmsg(bfd_get_error()));
        return false;
    }
    symtab_size = bfd_get_symtab_upper_bound(ihandle);
    if((signed)symtab_size < 0)
    {
        printf("failed to get symbol table size: %s\n", bfd_errmsg(bfd_get_error()));
        return false;
    }
    symtab_buffer = malloc(symtab_size);
    symtab_count = bfd_canonicalize_symtab(ihandle, symtab_buffer);
    if(symtab_count < 0)
    {
        printf("failed to get symbol table number of entries: %s\n", bfd_errmsg(bfd_get_error()));
        return false;
    }
    /* Tweak the symbol table to remove sections that no-longer exist */
    remove_sections((libr_section *)ihandle->sections, symtab_buffer, &symtab_count);
    bfd_set_symtab(ohandle, symtab_buffer, symtab_count);
    /* Actually copy section data */
    for(iscn = (libr_section *)ihandle->sections; iscn != NULL; iscn = BFD_SECTION_HANDLE(iscn)->next)
    {
        #ifdef HAVE_BFD_2_34
        size = bfd_section_size(BFD_SECTION_HANDLE(iscn));
        #else
        size = bfd_get_section_size(BFD_SECTION_HANDLE(iscn));
        #endif
        if(size == 0)
            continue; /* Section has been marked for deletion */
        oscn = (libr_section *)BFD_SECTION_HANDLE(iscn)->output_section;
        reloc_size = bfd_get_reloc_upper_bound(ihandle, BFD_SECTION_HANDLE(iscn));
        if(reloc_size == 0)
            bfd_set_reloc(ohandle, BFD_SECTION_HANDLE(oscn), NULL, 0);
        else
        {
            reloc_buffer = malloc(reloc_size);
            reloc_count = bfd_canonicalize_reloc(ihandle, BFD_SECTION_HANDLE(iscn), reloc_buffer, symtab_buffer);
            bfd_set_reloc(ohandle, BFD_SECTION_HANDLE(oscn), reloc_buffer, reloc_count);
        }

        if(
            #ifdef HAVE_BFD_2_34
            bfd_section_flags(BFD_SECTION_HANDLE(iscn)) & SEC_HAS_CONTENTS
            #else
            bfd_get_section_flags(ihandle, BFD_SECTION_HANDLE(iscn)) & SEC_HAS_CONTENTS
            #endif
        )
        {
            /* NOTE: if the section is just being copied then do that, otherwise grab
             * the user data for the section (stored previously by set_data)
             */
            if(BFD_SECTION_HANDLE(iscn)->userdata == NULL)
            {
                buffer = malloc(size);
                if(!bfd_get_section_contents(ihandle, BFD_SECTION_HANDLE(iscn), buffer, 0, size))
                {
                    printf("failed to get section contents: %s\n", bfd_errmsg(bfd_get_error()));
                    free(buffer);
                    return false;
                }
            }
            else
                buffer = BFD_SECTION_HANDLE(iscn)->userdata;
            if(!bfd_set_section_contents(ohandle, BFD_SECTION_HANDLE(oscn), buffer, 0, size))
            {
                printf("failed to set section contents: %s\n", bfd_errmsg(bfd_get_error()));
                free(buffer);
                return false;
            }
            free(buffer);
            if(!bfd_copy_private_section_data(ihandle, BFD_SECTION_HANDLE(iscn), ohandle, BFD_SECTION_HANDLE(oscn)))
            {
                printf("failed to copy private section data: %s\n", bfd_errmsg(bfd_get_error()));
                return false;
            }
        }
    }
    if(!bfd_copy_private_bfd_data(ihandle, ohandle))
    {
        printf("failed to copy private data: %s\n", bfd_errmsg(bfd_get_error()));
        return false;
    }
    return true;
}

/*
 * Perform a cross-device safe rename
 */
int safe_rename(const char *old, const char *new)
{
    char buffer[1024];
    FILE *in, *out;
    int read_count;

    in = fopen(old, "r");
    if(!in)
        return -1;
    out = fopen(new, "w");
    if(!out)
    {
        fclose(in);
        return -1;
    }
    while(!feof(in) && !ferror(in))
    {
        read_count = fread(buffer, 1, sizeof(buffer), in);
        fwrite(buffer, read_count, 1, out);
    }
    fclose(in);
    fclose(out);
    if(ferror(in))
    {
        remove(new);
        return -1;
    }
    return remove(old);
}

/*
 * Write the output file using the libbfd method
 */
int write_output(libr_file *file_handle)
{
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);
    int write_ok = false;

    if(fh->bfd_write != NULL)
    {
        write_ok = true;
        if(!build_output(file_handle))
        {
            printf("BFD::write_output failed to build output file.\n");
            write_ok = false;
        }
        else
        {
            printf("BFD::write_output built output file: %s temp: %s\n", fh->filename, fh->tempfile);
        }

        if(!bfd_close(fh->bfd_write))
        {
            printf("BFD::write_output failed to close write handle.\n");
            write_ok = false;
        }
        else
        {
            printf ("BFD::write_output closed file handle: file: %s temp: %s\n", fh->filename, fh->tempfile);
        }

        if(fh->fd_handle != 0 && close(fh->fd_handle) == ERROR)
        {
            write_ok = false;
            printf("BFD::write_output failed to close write file descriptor file: %s temp: %s\n", fh->filename, fh->tempfile);
        }
        else
        {
            printf ("BFD::write_output closed file descriptor: file: %s temp: %s\n", fh->filename, fh->tempfile);
        }
    }
    /* The read handle must be closed last since it is used in the write process */
    if(!bfd_close(fh->bfd_read))
    {
        printf("BFD::write_output failed to close read handle file: %s temp: %s\n", fh->filename, fh->tempfile);
    }
    else
    {
        printf ("BFD::write_output closed read handle: file: %s temp: %s\n", fh->filename, fh->tempfile);
    }

    if(write_ok)
    {
        if(rename(fh->tempfile, fh->filename) < 0)
        {
            if(errno != EXDEV || safe_rename(fh->tempfile, fh->filename) < 0)
            {
                printf("BFD::write_output failed to rename output file: %m (temp:%s->file:%s)\n", fh->tempfile, fh->filename);
            }
            else
            {
                printf("BFD::write_output renamed (%s->%s)\n", fh->filename, fh->tempfile);
            }
        }

        if(chmod(fh->filename, fh->filemode) < 0)
        {
            printf("BFD::write_output failed to set file mode file: %s temp: %s\n", fh->filename, fh->tempfile);
        }
        else
        {
            printf("BFD::write_output chmod success file: %s temp: %s\n", fh->filename, fh->tempfile);
        }

        if(chown(fh->filename, fh->fileowner, fh->filegroup) < 0)
        {
            printf("BFD::write_output failed to set file ownership file: %s temp: %s\n", fh->filename, fh->tempfile);
        }
        else
        {
            printf("BFD::write_output chown success file: %s temp: %s\n", fh->filename, fh->tempfile);
        }
    }

    return (write_ok);
}

/*
 * Find a named section from the ELF file using libbfd
 */
libr_intstatus find_section(libr_file *file_handle, char *section_name, libr_section **retscn)
{
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);
    libr_section *scn;

    for(scn = (libr_section *)fh->bfd_read->sections; scn != NULL; scn = BFD_SECTION_HANDLE(scn)->next)
    {
        if(strcmp(BFD_SECTION_HANDLE(scn)->name, section_name) == 0)
        {
            *retscn = scn;
            RETURN_OK;
        }
    }
    RETURN(LIBR_ERROR_NOSECTION, "ELF resource section not found");
}

/*
 * Obtain the data from a section using libbfd
 */
libr_data *get_data(libr_file *file_handle, libr_section *scn)
{
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);
    size_t size = (size_t)BFD_SECTION_HANDLE(scn)->size;
    libr_data *data = malloc(size);

    if(!bfd_get_section_contents(fh->bfd_read, BFD_SECTION_HANDLE(scn), data, 0, size))
    {
        free(data);
        data = NULL;
    }
    BFD_SECTION_HANDLE(scn)->userdata = data;
    return data;
}

/*
 * Create new data for a section using libbfd
 */
libr_data *new_data(libr_file *file_handle, libr_section *scn)
{
    /* NOTE: expanding data is handled by set_data for libbfd */
    if(BFD_SECTION_HANDLE(scn)->userdata != NULL)
        return BFD_SECTION_HANDLE(scn)->userdata;
    BFD_SECTION_HANDLE(scn)->size = 0;
    BFD_SECTION_HANDLE(scn)->userdata = malloc(0);
    return BFD_SECTION_HANDLE(scn)->userdata;
}

/*
 * Create new data for a section using libbfd (at least, do so memory-wise)
 */
libr_intstatus set_data(libr_file *file_handle, libr_section *scn, libr_data *data, off_t offset, char *buffer, size_t size)
{
    char *intbuffer = NULL;
    bfd_size_type new_size = offset + size;

    /* special case: clear buffer */
    if(buffer == NULL)
    {
        BFD_SECTION_HANDLE(scn)->size = 0;
        if(BFD_SECTION_HANDLE(scn)->userdata != NULL)
            free(BFD_SECTION_HANDLE(scn)->userdata);
        RETURN_OK;
    }
    /* normal case: add new data to the buffer */
    BFD_SECTION_HANDLE(scn)->size = new_size;
    BFD_SECTION_HANDLE(scn)->userdata = realloc(data, new_size);
    if(BFD_SECTION_HANDLE(scn)->userdata == NULL)
        RETURN(LIBR_ERROR_MEMALLOC, "Failed to allocate memory for data");
    intbuffer = BFD_SECTION_HANDLE(scn)->userdata;
    memcpy(&intbuffer[offset], buffer, size);
    RETURN_OK;
}

/*
 * Create a new section using libbfd
 */
libr_intstatus add_section(libr_file *file_handle, char *resource_name, libr_section **retscn)
{
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);
    libr_section *scn = NULL;

    scn = (libr_section *)bfd_make_section(fh->bfd_read, resource_name);
    if(scn == NULL)
        RETURN(LIBR_ERROR_NEWSECTION, "Failed to create new section");
    if(
        #ifdef HAVE_BFD_2_34
        !bfd_set_section_flags(BFD_SECTION_HANDLE(scn), SEC_HAS_CONTENTS | SEC_DATA | SEC_IN_MEMORY)
        #else
        !bfd_set_section_flags(fh->bfd_read, BFD_SECTION_HANDLE(scn), SEC_HAS_CONTENTS | SEC_DATA | SEC_IN_MEMORY)
        #endif
    )
    {
        RETURN(LIBR_ERROR_SETFLAGS, "Failed to set flags for section");
    }
    *retscn = scn;
    RETURN_OK;
}

/*
 * Remove a section and eliminate it from the ELF string table using libbfd
 */
libr_intstatus remove_section(libr_file *file_handle, libr_section *scn)
{
    BFD_SECTION_HANDLE(scn)->size = 0;
    RETURN_OK;
}

/*
 * Return the pointer to the actual data in the section
 */
void *data_pointer(libr_section *scn, libr_data *data)
{
    return data;
}

/*
 * Return the size of the data in the section
 */
size_t data_size(libr_section *scn, libr_data *data)
{
    return (size_t)BFD_SECTION_HANDLE(scn)->size;
}

/*
 * Return the next section in the ELF file
 */
libr_section *next_section(libr_file *file_handle, libr_section *scn)
{
    libr_file_internal *fh = BFD_FILE_HANDLE(file_handle);

    /* get the first section */
    if(scn == NULL)
    {
        if(fh->bfd_read == NULL)
            return NULL;
        return (libr_section *)fh->bfd_read->sections;
    }
    return (libr_section *)BFD_SECTION_HANDLE(scn)->next;
}

/*
 * Return the name of a section
 */
char *section_name(libr_file *file_handle, libr_section *scn)
{
    return (char *) BFD_SECTION_HANDLE(scn)->name;
}

/*
 * Initialize libbfd
 */
void initialize_backend(void)
{
    bfd_init();
}

#endif
