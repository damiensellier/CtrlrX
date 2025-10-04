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
#include <algorithm> // for std::min

extern "C"
{
    #include "libr.h"
}

// ==============================================================================
// Helper functions for binary patching (Ported from CtrlrWindows/CtrlrMac)
// ==============================================================================
namespace
{
    /**
     * Converts a string (or padded string) into a MemoryBlock of ASCII/UTF8 bytes.
     * Pads with nulls (0) up to maxLength if necessary.
     */
    void hexStringToBytes(const juce::String& text, int maxLength, juce::MemoryBlock& result) {
        result.setSize(0); // Clear the MemoryBlock before use
        // Use the raw UTF8 data from the string
        const char* utf8Data = text.toUTF8();
        size_t textLength = strlen(utf8Data);

        // Determine actual length to copy (up to maxLength)
        size_t copyLength = std::min((size_t)maxLength, textLength);
        
        // Copy the actual string content
        result.append(utf8Data, copyLength);

        // Pad with zeros if necessary
        while (result.getSize() < (size_t)maxLength) {
            uint8 zero = 0;
            result.append(&zero, 1);
        }
    }

    /**
     * Finds and replaces all occurrences of searchData with replaceData in the targetData.
     * Both searchData and replaceData MUST have the same size.
     */
    void replaceAllOccurrences(juce::MemoryBlock& targetData, const juce::MemoryBlock& searchData, const juce::MemoryBlock& replaceData)
    {
        if (searchData.getSize() != replaceData.getSize() || searchData.getSize() == 0)
        {
            _DBG("Invalid search/replace data sizes in replaceAllOccurrences");
            return;
        }

        // Use static_cast to work with raw bytes
        const uint8* rawData = static_cast<const uint8*>(targetData.getData());
        size_t dataSize = targetData.getSize();
        size_t searchSize = searchData.getSize();
        
        // Ensure the replacement block size matches the search size
        const void* replaceBytes = replaceData.getData();

        // Iterate through the target data
        for (size_t i = 0; i <= dataSize - searchSize; ++i)
        {
            // Check if the current segment matches the search data
            if (memcmp(rawData + i, searchData.getData(), searchSize) == 0)
            {
                // Replace the data in the target MemoryBlock
                // Note: JUCE MemoryBlock's copyFrom is safe even if memory shifts
                targetData.copyFrom(replaceBytes, i, replaceData.getSize());
                
                // Update rawData pointer if the MemoryBlock might have been reallocated
                rawData = static_cast<const uint8*>(targetData.getData());
            }
        }
    }
}
// ==============================================================================

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
    size_t originalFileSize;  // ← THIS LINE MUST BE HERE (As per user request)
    bool hasEmbeddedData;      // ← AND THIS LINE (As per user request)
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    
    // Search backwards from the end for the section delimiter first
    // The directory is always at the very end of the file
    size_t searchSize = std::min((size_t)8192, fileSize); // Search last 8KB
    size_t searchStart = fileSize - searchSize;
    
    file.seekg(searchStart);
    std::string buffer(searchSize, '\0');
    file.read(&buffer[0], searchSize);
    
    // Find the LAST occurrence of MAGIC_HEADER (should be near the end)
    size_t headerPos = buffer.rfind(MAGIC_HEADER);
    
    if (headerPos != std::string::npos) {
        size_t absolutePos = searchStart + headerPos;
        hasEmbeddedData = true;
        
        _DBG("SimpleEmbeddedDataManager::findSections - Found magic header at file position: " + std::to_string(absolutePos));
        
        // Parse sections starting right after the magic header
        if (parseSections(file, absolutePos + MAGIC_HEADER.length())) {
            // Find the earliest section offset to determine original file size
            originalFileSize = fileSize;
            for (const auto& section : sections) {
                if (section.offset < originalFileSize) {
                    originalFileSize = section.offset;
                }
            }
            return true;
        }
    }
    
    _DBG("SimpleEmbeddedDataManager::findSections - No magic header found");
    
    // No embedded data found
    originalFileSize = fileSize;
    return false;
}
   bool parseSections(std::ifstream& file, size_t startPos) {
    file.seekg(startPos);
    std::string line;
    
    _DBG("SimpleEmbeddedDataManager::parseSections - starting at position " + std::to_string(startPos));
    
    while (std::getline(file, line)) {
        _DBG("SimpleEmbeddedDataManager::parseSections - read line: [" + line + "]");
        
        if (line == SECTION_DELIMITER) {
            _DBG("SimpleEmbeddedDataManager::parseSections - found delimiter, stopping");
            break;
        }
        
        // Parse section info: "NAME:OFFSET:SIZE:COMPRESSED"
        std::istringstream iss(line);
        std::string name, offsetStr, sizeStr, compressedStr;
        
        if (std::getline(iss, name, ':') &&
            std::getline(iss, offsetStr, ':') &&
            std::getline(iss, sizeStr, ':') &&
            std::getline(iss, compressedStr)) {
            
            // Validate strings before conversion
            if (name.empty() || offsetStr.empty() || sizeStr.empty() || compressedStr.empty()) {
                _DBG("SimpleEmbeddedDataManager::parseSections - skipping invalid entry (empty field)");
                continue;
            }
            
            try {
                DataSection section;
                section.name = name;
                section.offset = std::stoull(offsetStr);
                section.size = std::stoull(sizeStr);
                section.compressed = (compressedStr == "1");
                sections.push_back(section);
                _DBG("SimpleEmbeddedDataManager::parseSections - added section: " + name + 
                     " offset=" + std::to_string(section.offset) + 
                     " size=" + std::to_string(section.size));
            } catch (const std::exception& e) {
                _DBG("SimpleEmbeddedDataManager::parseSections - exception: " + std::string(e.what()));
                continue;
            }
        }
    }
    
    _DBG("SimpleEmbeddedDataManager::parseSections - total sections found: " + std::to_string(sections.size()));
    
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

_DBG("CtrlrLinux::exportWithDefaultPanel - After exportPanel:");
_DBG("  panelExportData size: " + STR((int32)panelExportData.getSize()));
_DBG("  panelResourcesData size: " + STR((int32)panelResourcesData.getSize()));

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
    // dataManager.writeSection(CTRLR_INTERNAL_RESOURCES_SECTION, panelResourcesData);
    if (panelResourcesData.getSize() > 0)
    {
        if (!dataManager.writeSection(CTRLR_INTERNAL_RESOURCES_SECTION, panelResourcesData))
        {
            return (Result::fail("Linux native (Simple), failed to write panel resource data to binary [" + newMe.getFullPathName() + "], size [" + STR((int32)panelResourcesData.getSize()) + "]"));
        }
        else
        {
            _DBG("WE SHOULD BE WRITING RESOURCES OUT HERE");
            _DBG("CtrlrLinux::exportWithDefaultPanel (Simple) wrote resources, size: " + STR((int)panelResourcesData.getSize()));
        }
    }


    // ===================================================================================
    // VST3 IDENTIFIER REPLACEMENT HACK (Linux)
    // Ensures the plugin shows the custom name (e.g., JD-990 Editor) instead of "CtrlrX"
    // ===================================================================================
    File executableFile = newMe; 
    String fileExtension = executableFile.getFileExtension().toLowerCase();

    // On Linux, VST3s are typically .so files, often nested in a .vst3 folder structure.
    // We check the file extension and the VST3 replacement setting from the panel.
    if (fileExtension == ".so" || fileExtension == ".vst3")
    {
        _DBG("CtrlrLinux::exportWithDefaultPanel - Detected potential plugin file (so/vst3).");
        
        const bool replaceVst3PluginIds = panelToWrite->getProperty(Ids::panelReplaceVst3PluginIds);

        if (replaceVst3PluginIds)
        {
            _DBG("CtrlrLinux::exportWithDefaultPanel - Performing VST3 ID replacement.");
            
            MemoryBlock executableData;
            if (executableFile.loadFileAsData(executableData))
            {
                String pluginName = panelToWrite->getProperty(Ids::name).toString();
                String pluginCode = panelToWrite->getProperty(Ids::panelInstanceUID).toString();
                String manufacturerName = panelToWrite->getProperty(Ids::panelAuthorName).toString();
                String manufacturerCode = panelToWrite->getProperty(Ids::panelInstanceManufacturerID).toString();
                String plugType = panelToWrite->getProperty(Ids::panelPlugType).toString();

                _DBG("  New Plugin Name: " + pluginName);
                _DBG("  New Plugin Code: " + pluginCode);

                MemoryBlock pluginNameHex, pluginCodeHex, manufacturerNameHex, manufacturerCodeHex, plugTypeHex;

                // Prepare replacement bytes (using the helpers defined above)
                hexStringToBytes(pluginName, 32, pluginNameHex);
                hexStringToBytes(pluginCode, 4, pluginCodeHex);
                hexStringToBytes(manufacturerName, 16, manufacturerNameHex);
                hexStringToBytes(manufacturerCode, 4, manufacturerCodeHex);
                hexStringToBytes(plugType, 16, plugTypeHex);

                MemoryBlock searchPluginNameHex, searchPluginCodeHex, searchManufacturerNameHex, searchManufacturerCodeHex, searchPlugTypeHex;

                // Prepare search bytes (default CtrlrX strings/IDs from JUCE template)
                // Search: "CtrlrX          " (32 bytes with padding)
                // Search: CtrlrX plugin name "CtrlrX          " (32 bytes)
                hexStringToBytes("43 74 72 6C 72 58 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20", 32, searchPluginNameHex);
                // Search: CtrlrX plugin manufacturer code "cTrX" (4 bytes)
                hexStringToBytes("63 54 72 58", 4, searchPluginCodeHex);
                // Search: "CtrlrX Project  " (16 bytes)
                hexStringToBytes("43 74 72 6C 72 58 20 50 72 6F 6A 65 63 74 20 20", 16, searchManufacturerNameHex);
                // Search: CtrlrX plugin code "cTrl" (4 bytes)
                hexStringToBytes("63 54 72 6C", 4, searchManufacturerCodeHex);
                // Search plugType "Instrument|Tools" (16 bytes)
                hexStringToBytes("49 6E 73 74 72 75 6D 65 6E 74 7C 54 6F 6F 6C 73", 16, searchPlugTypeHex);

                _DBG("  Starting string replacement process...");

                replaceAllOccurrences(executableData, searchPluginNameHex, pluginNameHex);
                replaceAllOccurrences(executableData, searchPluginCodeHex, pluginCodeHex);
                replaceAllOccurrences(executableData, searchManufacturerNameHex, manufacturerNameHex);
                replaceAllOccurrences(executableData, searchManufacturerCodeHex, manufacturerCodeHex);
                replaceAllOccurrences(executableData, searchPlugTypeHex, plugTypeHex);

                _DBG("  String replacement process completed. Writing data back.");

                if (!executableFile.replaceWithData(executableData.getData(), executableData.getSize()))
                {
                    _DBG("Error: Failed to write modified executable data");
                    return (Result::fail("Linux Native: Failed to write modified executable data for VST3 name patching."));
                }
                _DBG("Modified executable data written successfully.");

            } else {
                _DBG("Error: Failed to load executable into memory for VST3 name patching.");
                return Result::fail("Linux Native: Failed to load executable into memory for VST3 name patching.");
            }
        } else {
            _DBG("VST3 ID replacement skipped by panel setting.");
        }
    }
    // ===================================================================================


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
    _DBG("CtrlrLinux::getDefaultResources - ENTRY");
    
    // Try new format first
    SimpleEmbeddedDataManager dataManager(File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toStdString());
    
    if (dataManager.initialize()) {
        _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager initialized successfully");
        
        if (dataManager.readSection(CTRLR_INTERNAL_RESOURCES_SECTION, dataToWrite)) {
            _DBG("CtrlrLinux::getDefaultResources (Simple), read resource data size [" + STR((int32)dataToWrite.getSize()) + "]");
            return (Result::ok());
        } else {
            _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager found no resources section");
        }
    } else {
        _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager initialize failed");
    }
    
    // Fall back to libr if new format not found
    _DBG("CtrlrLinux::getDefaultResources - falling back to libr");
    
    libr_file *handle = libr_open(File::getSpecialLocation(File::hostApplicationPath).getFullPathName().toUTF8().getAddress(), LIBR_READ);

    if (handle == nullptr)
    {
        _DBG("CtrlrLinux::getDefaultResources - libr_open FAILED");
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
        _DBG("CtrlrLinux::getDefaultResources - libr_malloc returned NULL (no resources found)");
        libr_close(handle);
        return (Result::fail("Linux native, libr_malloc didn't find embedded panel resources in binary"));
    }
    else
    {
        _DBG("CtrlrLinux::getDefaultResources, libr_malloc returned data for resources size [" + STR((int32)panelResourcesDataSize) + "]");
    }

    dataToWrite.append(panelResourcesData, panelResourcesDataSize);
    libr_close(handle);
    
    _DBG("CtrlrLinux::getDefaultResources - EXIT with Result::ok(), dataToWrite size = " + STR((int32)dataToWrite.getSize()));

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
