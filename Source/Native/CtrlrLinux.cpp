#include "stdafx.h"
#define LINUX
#ifdef LINUX
// https://github.com/RomanKubiak/ctrlr/issues/38
#define PACKAGE "Ctrlr"

#include "CtrlrMacros.h"
#include "CtrlrLinux.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrManager/CtrlrManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "keys.h"
#include <fstream>
#include <sstream>
#include <vector>

extern "C"
{
    #include "libr.h"
}

// Simple embedded data manager using only JUCE - no external dependencies
class SimpleEmbeddedDataManager
{
public:
    struct DataSection {
        std::string name;
        size_t offset;
        size_t size;
        bool compressed;
    };
    
    static const std::string MAGIC_HEADER;
    static const std::string SECTION_DELIMITER;
    
private:
    std::vector<DataSection> sections;
    std::string filePath;
    
    bool findSections(std::ifstream& file) {
        sections.clear();
        
        // Look for our magic header
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::string buffer(4096, '\0');
        size_t searchPos = 0;
        
        while (searchPos < fileSize) {
            file.seekg(searchPos);
            file.read(&buffer[0], std::min(buffer.size(), fileSize - searchPos));
            size_t bytesRead = file.gcount();
            
            size_t headerPos = buffer.find(MAGIC_HEADER);
            if (headerPos != std::string::npos) {
                size_t absolutePos = searchPos + headerPos + MAGIC_HEADER.length();
                return parseSections(file, absolutePos);
            }
            
            searchPos += bytesRead - MAGIC_HEADER.length();
            if (searchPos >= fileSize) break;
        }
        
        return false;
    }
    
    bool parseSections(std::ifstream& file, size_t startPos) {
        file.seekg(startPos);
        std::string line;
        
        while (std::getline(file, line)) {
            if (line == SECTION_DELIMITER) break;
            
            // Parse section info: "NAME:OFFSET:SIZE:COMPRESSED"
            std::istringstream iss(line);
            std::string name, offsetStr, sizeStr, compressedStr;
            
            if (std::getline(iss, name, ':') &&
                std::getline(iss, offsetStr, ':') &&
                std::getline(iss, sizeStr, ':') &&
                std::getline(iss, compressedStr)) {
                
                // Validate strings before conversion
                if (name.empty() || offsetStr.empty() || sizeStr.empty() || compressedStr.empty()) {
                    continue; // Skip invalid entries
                }
                
                try {
                    DataSection section;
                    section.name = name;
                    section.offset = std::stoull(offsetStr);
                    section.size = std::stoull(sizeStr);
                    section.compressed = (compressedStr == "1");
                    sections.push_back(section);
                } catch (const std::exception& e) {
                    // Skip invalid numeric conversions
                    continue;
                }
            }
        }
        
        return !sections.empty();
    }
    
public:
    SimpleEmbeddedDataManager(const std::string& path) : filePath(path) {}
    
    bool initialize() {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) return false;
        return findSections(file);
    }
    
    bool readSection(const std::string& sectionName, MemoryBlock& output) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) return false;
        
        for (const auto& section : sections) {
            if (section.name == sectionName) {
                file.seekg(section.offset);
                
                MemoryBlock rawData;
                rawData.setSize(section.size, false);
                file.read(static_cast<char*>(rawData.getData()), section.size);
                
                if (section.compressed) {
                    // Data is already compressed, just copy it
                    output = rawData;
                } else {
                    output = rawData;
                }
                return true;
            }
        }
        return false;
    }
    
    bool writeSection(const std::string& sectionName, const MemoryBlock& data) {
        // Read original file
        std::ifstream originalFile(filePath, std::ios::binary);
        if (!originalFile.is_open()) return false;
        
        originalFile.seekg(0, std::ios::end);
        size_t originalSize = originalFile.tellg();
        originalFile.seekg(0, std::ios::beg);
        
        MemoryBlock originalData;
        originalData.setSize(originalSize, false);
        originalFile.read(static_cast<char*>(originalData.getData()), originalSize);
        originalFile.close();
        
        // Find or create section
        size_t dataOffset = originalSize;
        bool sectionExists = false;
        
        for (auto& section : sections) {
            if (section.name == sectionName) {
                section.offset = dataOffset;
                section.size = data.getSize();
                section.compressed = false; // We're storing pre-compressed data
                sectionExists = true;
                break;
            }
        }
        
        if (!sectionExists) {
            DataSection newSection;
            newSection.name = sectionName;
            newSection.offset = dataOffset;
            newSection.size = data.getSize();
            newSection.compressed = false;
            sections.push_back(newSection);
        }
        
        // Write new file
        std::string tempPath = filePath + ".tmp";
        std::ofstream newFile(tempPath, std::ios::binary);
        if (!newFile.is_open()) return false;
        
        // Write original data
        newFile.write(static_cast<const char*>(originalData.getData()), originalData.getSize());
        
        // Write new section data
        newFile.write(static_cast<const char*>(data.getData()), data.getSize());
        
        // Write section directory
        newFile << MAGIC_HEADER;
        for (const auto& section : sections) {
            newFile << section.name << ":" << section.offset << ":" 
                   << section.size << ":" << (section.compressed ? "1" : "0") << "\n";
        }
        newFile << SECTION_DELIMITER << "\n";
        
        newFile.close();
        
        // Replace original file
        return (rename(tempPath.c_str(), filePath.c_str()) == 0);
    }
};

const std::string SimpleEmbeddedDataManager::MAGIC_HEADER = "\n\n__CTRLR_EMBEDDED_DATA_V2__\n";
const std::string SimpleEmbeddedDataManager::SECTION_DELIMITER = "__END_SECTIONS__";

CtrlrLinux::CtrlrLinux(CtrlrManager &_owner) : owner(_owner)
{
}

CtrlrLinux::~CtrlrLinux()
{
}

const Result CtrlrLinux::exportWithDefaultPanel(CtrlrPanel *panelToWrite, const bool isRestricted, const bool signPanel)
{
    _DBG("CtrlrLinux::exportWithDefaultPanel (Simple version)");

    if (panelToWrite == nullptr)
    {
        return (Result::fail("Linux native, panel pointer is invalid"));
    }

    File me = File::getSpecialLocation(File::currentApplicationFile);
    File newMe;
    MemoryBlock panelExportData;
    MemoryBlock panelResourcesData;

    FileChooser fc(CTRLR_NEW_INSTANCE_DIALOG_TITLE,
                   me.getParentDirectory().getChildFile(File::createLegalFileName(panelToWrite->getProperty(Ids::name))).withFileExtension(me.getFileExtension()),
                   me.getFileExtension(),
                   panelToWrite->getOwner().getProperty(Ids::ctrlrNativeFileDialogs));

    if (fc.browseForFileToSave(true))
    {
        _DBG("CtrlrLinux::exportWithDefaultPanel chosen file: " + fc.getResult().getFullPathName());
        newMe = fc.getResult();

        if (!me.copyFileTo(newMe))
        {
            return (Result::fail("Linux native, copyFileTo from \"" + me.getFullPathName() + "\" to \"" + newMe.getFullPathName() + "\" failed"));
        }
    }
    else
    {
        return (Result::fail("User cancelled the export operation."));
    }

    // Export panel data
    CtrlrPanel p(owner, "", 0);
    String error = p.exportPanel(panelToWrite, File(), newMe, &panelExportData, &panelResourcesData, isRestricted);

    if (error != "")
    {
        return Result::fail("CtrlrPanel::exportPanel failed: " + error);
    }

    // Debug: Check the restricted value before writing
    ValueTree testTree = ValueTree::readFromGZIPData(panelExportData.getData(), panelExportData.getSize());
    if (testTree.isValid()) {
        var restrictedValue = testTree.getProperty(Ids::restricted);
        _DBG("CtrlrLinux::exportWithDefaultPanel - restricted value before write: " + restrictedValue.toString());
    }
    
    // Use Simple manager instead of libr
    SimpleEmbeddedDataManager dataManager(newMe.getFullPathName().toStdString());
    
    // Write panel data
    if (!dataManager.writeSection(CTRLR_INTERNAL_PANEL_SECTION, panelExportData))
    {
        return (Result::fail("Linux native (Simple), failed to write panel data to binary [" + newMe.getFullPathName() + "], size [" + STR((int32)panelExportData.getSize()) + "]"));
    }
    
    _DBG("CtrlrLinux::exportWithDefaultPanel (Simple) wrote panel data to binary size [" + STR((int32)panelExportData.getSize()) + "]");

    // Write resources if any
    if (panelResourcesData.getSize() > 0)
    {
        if (!dataManager.writeSection(CTRLR_INTERNAL_RESOURCES_SECTION, panelResourcesData))
        {
            return (Result::fail("Linux native (Simple), failed to write panel resource data to binary [" + newMe.getFullPathName() + "], size [" + STR((int32)panelResourcesData.getSize()) + "]"));
        }
        else
        {
            _DBG("CtrlrLinux::exportWithDefaultPanel (Simple) wrote resources, size: " + _STR((int)panelResourcesData.getSize()));
        }
    }

    // Set executable permissions
    if (chmod(newMe.getFullPathName().toUTF8().getAddress(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH))
    {
        return (Result::fail("Linux native, chmod failed on destination binary [" + newMe.getFullPathName() + "]"));
    }

    return (Result::ok());
}

const Result CtrlrLinux::getDefaultPanel(MemoryBlock& dataToWrite)
{
#ifdef DEBUG_INSTANCE
    File temp("/home/r.kubiak/devel/debug.bpanelz");
    temp.loadFileAsData(dataToWrite);
    return (Result::ok());
#endif

    // Try new format first
    SimpleEmbeddedDataManager dataManager(File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toStdString());
    
    if (dataManager.initialize() && dataManager.readSection(CTRLR_INTERNAL_PANEL_SECTION, dataToWrite))
    {
        _DBG("CtrlrLinux::getDefaultPanel (Simple), read panel data size [" + STR((int32)dataToWrite.getSize()) + "]");
        
        // Debug: Check the restricted value after reading
        ValueTree testTree = ValueTree::readFromGZIPData(dataToWrite.getData(), dataToWrite.getSize());
        if (testTree.isValid()) {
            var restrictedValue = testTree.getProperty(Ids::restricted);
            _DBG("CtrlrLinux::getDefaultPanel (Simple) - restricted value after read: " + restrictedValue.toString());
        }
        
        return (Result::ok());
    }
    
    // Fall back to libr if new format not found
    _DBG("CtrlrLinux::getDefaultPanel - falling back to libr");
    
    libr_file *handle = libr_open(File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toUTF8().getAddress(), LIBR_READ);

    if (handle == nullptr)
    {
        return (Result::fail("Linux native, libr_open failed to open binary [self]"));
    }
    else
    {
        _DBG("CtrlrLinux::getDefaultPanel, libr_open succeeded for binary [self]");
    }

    size_t panelDataSize;
    char *panelData = (char *)libr_malloc(handle, (char *)CTRLR_INTERNAL_PANEL_SECTION, &panelDataSize);

    if (panelData == nullptr)
    {
        libr_close(handle);
        return (Result::fail("Linux native, libr_malloc didn't find embedded panel in binary"));
    }
    else
    {
        _DBG("CtrlrLinux::getDefaultPanel, libr_malloc returned data for panel size [" + STR((int32)panelDataSize) + "]");
    }

    dataToWrite.append(panelData, panelDataSize);
    libr_close(handle);

    return (Result::ok());
}

const Result CtrlrLinux::getDefaultResources(MemoryBlock& dataToWrite)
{
#ifdef DEBUG_INSTANCE
    File temp("/home/r.kubiak/devel/debug.bpanelz");
    MemoryBlock data;
    {
        ScopedPointer <FileInputStream> fis (temp.createInputStream());
        fis->readIntoMemoryBlock (data);
    }

    ValueTree t = ValueTree::readFromGZIPData(data.getData(), data.getSize());

    if (t.isValid())
    {
        ValueTree r = t.getChildWithName (Ids::resourceExportList);
        if (r.isValid())
        {
            MemoryOutputStream mos (dataToWrite, false);
            {
                GZIPCompressorOutputStream gzipOutputStream (&mos);
                r.writeToStream(gzipOutputStream);
                gzipOutputStream.flush();
            }
            return (Result::ok());
        }
    }
    else
    {
        return (Result::fail("Linux Native: getDefaultResources got data but couldn't parse it as a compressed ValueTree"));
    }
#endif

    // Try new format first
    SimpleEmbeddedDataManager dataManager(File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toStdString());
    
    if (dataManager.initialize() && dataManager.readSection(CTRLR_INTERNAL_RESOURCES_SECTION, dataToWrite))
    {
        _DBG("CtrlrLinux::getDefaultResources (Simple), read resource data size [" + STR((int32)dataToWrite.getSize()) + "]");
        return (Result::ok());
    }
    
    // Fall back to libr if new format not found
    _DBG("CtrlrLinux::getDefaultResources - falling back to libr");
    
    libr_file *handle = libr_open(File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toUTF8().getAddress(), LIBR_READ);

    if (handle == nullptr)
    {
        return (Result::fail("Linux native, libr_open failed to open binary [self]"));
    }
    else
    {
        _DBG("CtrlrLinux::getDefaultResources, libr_open succeeded for binary [self]");
    }

    size_t panelResourcesDataSize;
    char *panelResourcesData = (char *)libr_malloc(handle, (char *)CTRLR_INTERNAL_RESOURCES_SECTION, &panelResourcesDataSize);

    if (panelResourcesData == nullptr)
    {
        libr_close(handle);
        return (Result::fail("Linux native, libr_malloc didn't find embedded panel resources in binary"));
    }
    else
    {
        _DBG("CtrlrLinux::getDefaultResources, libr_malloc returned data for panel size [" + STR((int32)panelResourcesDataSize) + "]");
    }

    dataToWrite.append(panelResourcesData, panelResourcesDataSize);
    libr_close(handle);

    return (Result::ok());
}

const Result CtrlrLinux::getSignature(MemoryBlock &dataToWrite)
{
    libr_file *handle = libr_open (File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toUTF8().getAddress(), LIBR_READ);

    if (handle == nullptr)
    {
        return (Result::fail ("Linux native, libr_open failed to open binary [self]"));
    }
    else
    {
        _DBG("CtrlrLinux::getSignature, libr_open succeeded for binary [self]");
    }

    size_t signatureDataSize;
    char *signatureData = (char *)libr_malloc (handle, (char *)CTRLR_INTERNAL_RESOURCES_SECTION, &signatureDataSize);

    if (signatureData == nullptr)
    {
        libr_close (handle);
        return (Result::fail ("Linux native, libr_malloc didn't find embedded signature in binary"));
    }
    else
    {
        _DBG("CtrlrLinux::getSignature, libr_malloc returned data for signature size ["+STR((int32)signatureDataSize)+"]");
    }

    dataToWrite.append (signatureData, signatureDataSize);

    libr_close (handle);

    return (Result::ok());
}

const Result CtrlrLinux::sendKeyPressEvent(const KeyPress &event)
{
    return (ctrlr_sendKeyPressEvent (event));
}

#endif