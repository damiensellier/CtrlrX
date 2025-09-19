# Try to find libbfd
# Once done, this will define
#
# LIBBFD_FOUND               - system has libbfd
# LIBBFD_INCLUDE_DIRS        - libbfd include directories
# LIBBFD_LIBRARIES           - libbfd library
#
# LIBBFD_ROOT_DIR may be defined as a hint for where to look
#
# Adapted from https://code.ihep.ac.cn/chenj/eos/-/blob/4.5.1/cmake/Findlibbfd.cmake

include(FindPackageHandleStandardArgs)

if(LIBBFD_INCLUDE_DIRS AND LIBBFD_LIBRARIES)
  set(LIBBFD_FIND_QUIETLY TRUE)
else()
  find_path(
    LIBBFD_INCLUDE_DIR
    NAMES bfd.h
    HINTS ${LIBBFD_ROOT_DIR}
    PATH_SUFFIXES include
  )

  if (LIBBFD_USE_STATIC_LIBS)
    set( _libbfd_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    # prefer static library if found:
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  endif()

  find_library(
    LIBBFD_LIBRARY
    NAMES bfd
    HINTS ${LIBBFD_ROOT_DIR}
    PATH_SUFFIXES lib lib64
  )

  find_library(
    LIBIBERTY_LIBRARY
    NAMES iberty
    HINTS ${LIBBFD_ROOT_DIR}
    PATH_SUFFIXES lib lib64
  )

  # Restore the original find library ordering
  if(LIBBFD_USE_STATIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_libbfd_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
  endif()


  set(LIBBFD_LIBRARIES ${LIBBFD_LIBRARY} ${LIBIBERTY_LIBRARY})
  set(LIBBFD_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIR})

  find_package_handle_standard_args(
    libbfd
    DEFAULT_MSG
    LIBBFD_LIBRARY
    LIBBFD_INCLUDE_DIR)

  if(LIBBFD_FOUND)
    add_library(libbfd STATIC IMPORTED)
    set_property(TARGET libbfd PROPERTY IMPORTED_LOCATION ${LIBBFD_LIBRARY})
  endif()
endif()
