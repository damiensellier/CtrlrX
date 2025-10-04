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
    // Keep libr included for getSignature, but will remove from resource retrieval
    #include "libr.h"
}


static MemoryBlock hexToBytes(const String& hexString) {
    MemoryBlock result;
    String cleaned = hexString.removeCharacters(" \t\r\n");
    
    for (int i = 0; i < cleaned.length(); i += 2) {
        if (i + 1 < cleaned.length()) {
            String byteStr = cleaned.substring(i, i + 2);
            uint8 byte = (uint8)byteStr.getHexValue32();
            result.append(&byte, 1);
        }
    }
    return result;
}

// Helper: Convert string to fixed-size byte array (pad or truncate)
static MemoryBlock stringToFixedBytes(const String& str, int fixedSize) {
    MemoryBlock result;
    result.setSize(fixedSize, true);
    
    const char* chars = str.toUTF8();
    int copySize = jmin(fixedSize, (int)strlen(chars));
    memcpy(result.getData(), chars, copySize);
    
    return result;
}

// Helper: Replace all occurrences and return count
static int replaceAllOccurrences(MemoryBlock& target, const MemoryBlock& search, const MemoryBlock& replace) {
    if (search.getSize() != replace.getSize() || search.getSize() == 0) {
        return 0;
    }
    
    int count = 0;
    const uint8* data = static_cast<const uint8*>(target.getData());
    size_t dataSize = target.getSize();
    size_t searchSize = search.getSize();
    
    for (size_t i = 0; i <= dataSize - searchSize; ++i) {
        if (memcmp(data + i, search.getData(), searchSize) == 0) {
            target.copyFrom(replace.getData(), (int)i, replace.getSize());
            data = static_cast<const uint8*>(target.getData());
            count++;
        }
    }
    
    _DBG("BinaryPatcher: Made " + String(count) + " replacement(s)");
    return count;
}

// Function to get the actual VST3 plugin path (This is correct)
static File getVST3PluginPath()
{
    std::ifstream maps("/proc/self/maps");
    std::string line;
    
    while (std::getline(maps, line)) {
        if (line.find(".vst3/Contents/") != std::string::npos && 
            line.find(".so") != std::string::npos) {
            
            size_t pathStart = line.find('/');
            if (pathStart != std::string::npos) {
                std::string path = line.substr(pathStart);
                size_t soEnd = path.find(".so");
                if (soEnd != std::string::npos) {
                    path = path.substr(0, soEnd + 3);
                    return File(String(path));
                }
            }
        }
    }
    
    return File::getSpecialLocation(File::currentApplicationFile);
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
        size_t originalFileSize;  
        bool hasEmbeddedData;      
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        
        // Search backwards from the end for the section delimiter first
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
            
            // _DBG("SimpleEmbeddedDataManager::findSections - Found magic header at file position: " + std::to_string(absolutePos));
            
            if (parseSections(file, absolutePos + MAGIC_HEADER.length())) {
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
        
        originalFileSize = fileSize;
        return false;
    }

    bool parseSections(std::ifstream& file, size_t startPos) {
        file.seekg(startPos);
        std::string line;
        
        // _DBG("SimpleEmbeddedDataManager::parseSections - starting at position " + std::to_string(startPos));
        
        while (std::getline(file, line)) {
            // _DBG("SimpleEmbeddedDataManager::parseSections - read line: [" + line + "]"); // Removed: Too verbose
            
            if (line == SECTION_DELIMITER) {
                // _DBG("SimpleEmbeddedDataManager::parseSections - found delimiter, stopping");
                break;
            }
            
            // Parse section info: "NAME:OFFSET:SIZE:COMPRESSED"
            std::istringstream iss(line);
            std::string name, offsetStr, sizeStr, compressedStr;
            
            if (std::getline(iss, name, ':') &&
                std::getline(iss, offsetStr, ':') &&
                std::getline(iss, sizeStr, ':') &&
                std::getline(iss, compressedStr)) {
                
                if (name.empty() || offsetStr.empty() || sizeStr.empty() || compressedStr.empty()) {
                    continue;
                }
                
                try {
                    DataSection section;
                    section.name = name;
                    section.offset = std::stoull(offsetStr);
                    section.size = std::stoull(sizeStr);
                    section.compressed = (compressedStr == "1");
                    sections.push_back(section);
                    // _DBG("SimpleEmbeddedDataManager::parseSections - added section: " + name + " size=" + std::to_string(section.size)); // Removed: Too verbose
                } catch (const std::exception& e) {
                    // _DBG("SimpleEmbeddedDataManager::parseSections - exception: " + std::string(e.what())); // Removed: Only log on true errors
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
                
                output = rawData;
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
/************************************************************************************************************************/const Result CtrlrLinux::exportWithDefaultPanel(CtrlrPanel *panelToWrite, const bool isRestricted, const bool signPanel)
{
    _DBG("CtrlrLinux::exportWithDefaultPanel (Simple version)");

    if (panelToWrite == nullptr)
    {
        return (Result::fail("Linux native, panel pointer is invalid"));
    }

    File me = getVST3PluginPath();
    _DBG("CtrlrLinux::exportWithDefaultPanel - Plugin path: " + me.getFullPathName());
    
    File newMe;
    MemoryBlock panelExportData;
    MemoryBlock panelResourcesData;
    
    // Determine if we're a VST3 by checking the bundle structure
    File parentDir = me.getParentDirectory(); // x86_64-linux
    File contentsDir = parentDir.getParentDirectory(); // Contents
    File bundleDir = contentsDir.getParentDirectory(); // CtrlrX-Debug.vst3

    bool isVST3 = bundleDir.getFileName().endsWith(".vst3");

    // _DBG("CtrlrLinux::exportWithDefaultPanel - Detected type: " + String(isVST3 ? "VST3" : "Standalone")); // Removed: Less spam
    // _DBG("  Current binary: " + me.getFullPathName()); // Removed: Less spam
    // _DBG("  Bundle dir: " + bundleDir.getFullPathName()); // Removed: Less spam
    
    String panelName = File::createLegalFileName(panelToWrite->getProperty(Ids::name));
    
    // Build the suggested file path based on type
    File suggestedFile;
    if (isVST3) {
        // For VST3, suggest a .vst3 bundle
        suggestedFile = bundleDir.getParentDirectory().getChildFile(panelName + ".vst3");
    } else {
        // For standalone, use the executable extension
        suggestedFile = me.getParentDirectory()
                          .getChildFile(panelName)
                          .withFileExtension(me.getFileExtension());
    }

    FileChooser fc(CTRLR_NEW_INSTANCE_DIALOG_TITLE,
                   suggestedFile,
                   isVST3 ? ".vst3" : me.getFileExtension(),
                   panelToWrite->getOwner().getProperty(Ids::ctrlrNativeFileDialogs));

    if (fc.browseForFileToSave(true))
    {
        _DBG("CtrlrLinux::exportWithDefaultPanel chosen file: " + fc.getResult().getFullPathName());
        File chosenFile = fc.getResult();
        
        if (isVST3) {
            // Ensure the file has .vst3 extension
            if (!chosenFile.getFileName().endsWith(".vst3")) {
                chosenFile = chosenFile.withFileExtension(".vst3");
            }
            
            // Create VST3 bundle structure
            File bundleDir = chosenFile;
            File contentsDir = bundleDir.getChildFile("Contents");
            File binaryDir = contentsDir.getChildFile("x86_64-linux");
            
            // FIX: Use the VST3 bundle name for the internal .so file name.
            String binaryName = bundleDir.getFileNameWithoutExtension() + ".so"; 
            File binaryFile = binaryDir.getChildFile(binaryName);
            
            _DBG("CtrlrLinux::exportWithDefaultPanel creating VST3 bundle at: " + bundleDir.getFullPathName());
            // _DBG("  Binary will be at: " + binaryFile.getFullPathName() + " (using derived name: " + binaryName + ")"); // Removed: Less spam
            
            // Create directories
            if (!binaryDir.createDirectory()) {
                return (Result::fail("Failed to create VST3 bundle directory structure"));
            }
            
            // Copy the VST3 binary to the bundle location
            if (!me.copyFileTo(binaryFile)) {
                return (Result::fail("Linux native, copyFileTo from \"" + me.getFullPathName() + 
                                   "\" to \"" + binaryFile.getFullPathName() + "\" failed"));
            }
            
            // Update newMe to point to the actual binary for embedding
            newMe = binaryFile;
        } else {
            // Standalone - direct copy
            newMe = chosenFile;
            if (!me.copyFileTo(newMe)) {
                return (Result::fail("Linux native, copyFileTo from \"" + me.getFullPathName() + 
                                   "\" to \"" + newMe.getFullPathName() + "\" failed"));
            }
        }
    }
    else
    {
        return (Result::fail("User cancelled the export operation."));
    }

    // Export panel data
    CtrlrPanel p(owner, "", 0);
    // Note: panelExportData will contain compressed/gzipped data
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
        // _DBG("CtrlrLinux::exportWithDefaultPanel - restricted value before write: " + restrictedValue.toString()); // Removed: Less spam
    }
    
    // Perform binary patching for VST3
    if (isVST3) {
        _DBG("CtrlrLinux::exportWithDefaultPanel - Performing VST3 binary patching");
        
        String pluginName = panelToWrite->getProperty(Ids::name).toString();
        String pluginCode = panelToWrite->getProperty(Ids::panelInstanceUID).toString();
        String manufacturerName = panelToWrite->getProperty(Ids::panelAuthorName).toString();
        String manufacturerCode = panelToWrite->getProperty(Ids::panelInstanceManufacturerID).toString();
        
        _DBG("Plugin Name: " + pluginName);
        _DBG("Plugin Code: " + pluginCode);
        _DBG("Manufacturer: " + manufacturerName);
        
        // Load the binary into memory
        MemoryBlock binaryData;
        if (newMe.loadFileAsData(binaryData)) {
            // Create padded/truncated versions
            MemoryBlock pluginNameBytes = stringToFixedBytes(pluginName, 32);
            MemoryBlock pluginCodeBytes = stringToFixedBytes(pluginCode, 4);
            MemoryBlock manufacturerNameBytes = stringToFixedBytes(manufacturerName, 16);
            MemoryBlock manufacturerCodeBytes = stringToFixedBytes(manufacturerCode, 4);
            
            // Search patterns (original CtrlrX identifiers)
            MemoryBlock searchPluginName = hexToBytes("43 74 72 6C 72 58 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20");
            MemoryBlock searchPluginCode = hexToBytes("63 54 72 58");
            MemoryBlock searchManufacturerName = hexToBytes("43 74 72 6C 72 58 20 50 72 6F 6A 65 63 74 20 20");
            MemoryBlock searchManufacturerCode = hexToBytes("63 54 72 6C");
            
            int totalReplacements = 0;
            totalReplacements += replaceAllOccurrences(binaryData, searchPluginName, pluginNameBytes);
            totalReplacements += replaceAllOccurrences(binaryData, searchPluginCode, pluginCodeBytes);
            totalReplacements += replaceAllOccurrences(binaryData, searchManufacturerName, manufacturerNameBytes);
            totalReplacements += replaceAllOccurrences(binaryData, searchManufacturerCode, manufacturerCodeBytes);
            
            _DBG("CtrlrLinux::exportWithDefaultPanel - Made " + String(totalReplacements) + " total binary patches");
            
            // Write the patched binary back
            if (!newMe.replaceWithData(binaryData.getData(), binaryData.getSize())) {
                return (Result::fail("Failed to write patched binary"));
            }
        } else {
            return (Result::fail("Failed to load binary for patching"));
        }
    }
    
    // Use Simple manager instead of libr
    SimpleEmbeddedDataManager dataManager(newMe.getFullPathName().toStdString());
    
    // IMPORTANT: Initialize to read existing sections (even if there are none yet)
    if (!dataManager.initialize()) {
        _DBG("CtrlrLinux::exportWithDefaultPanel - no existing sections found, will create new ones");
    }
    
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
    if (chmod(newMe.getFullPathName().toUTF8().getAddress(), S_IRUSR | S_IWUSR | S_IXUSR | S_IXOTH | S_IRGRP | S_IXGRP | S_IROTH))
    {
        return (Result::fail("Linux native, chmod failed on destination binary [" + newMe.getFullPathName() + "]"));
    }

    return (Result::ok());
}
/************************************************************************************************************/
const Result CtrlrLinux::getDefaultPanel(MemoryBlock& dataToWrite)
{
#ifdef DEBUG_INSTANCE
    File temp("/home/r.kubiak/devel/debug.bpanelz");
    temp.loadFileAsData(dataToWrite);
    return (Result::ok());
#endif

    // Get the actual plugin binary path.
    File pluginBinary = getVST3PluginPath();
    const String pluginPath = pluginBinary.getFullPathName();
    
    _DBG("CtrlrLinux::getDefaultPanel - Attempting to read panel data from path: " + pluginPath);

    if (!pluginBinary.existsAsFile()) {
        _DBG("CtrlrLinux::getDefaultPanel - ERROR: Plugin binary path does not exist!");
        return (Result::fail("Plugin binary path is invalid or does not exist: " + pluginPath));
    }

    // Try new format first (SimpleEmbeddedDataManager)
    SimpleEmbeddedDataManager dataManager(pluginPath.toStdString());
    
    if (dataManager.initialize() && dataManager.readSection(CTRLR_INTERNAL_PANEL_SECTION, dataToWrite))
    {
        _DBG("CtrlrLinux::getDefaultPanel (Simple), read panel data size [" + STR((int32)dataToWrite.getSize()) + "]");
        
        // Debug: Check the restricted value after reading
        ValueTree testTree = ValueTree::readFromGZIPData(dataToWrite.getData(), dataToWrite.getSize());
        if (testTree.isValid()) {
            var restrictedValue = testTree.getProperty(Ids::restricted);
            // _DBG("CtrlrLinux::getDefaultPanel (Simple) - restricted value after read: " + restrictedValue.toString()); // Removed: Less spam
        }
        
        return (Result::ok());
    }
    
    // CRASH FIX: Temporarily removing the old libr fallback to isolate the crash source.
    _DBG("CtrlrLinux::getDefaultPanel - SimpleEmbeddedDataManager failed. Aborting resource retrieval.");
    
    return (Result::fail("CtrlrLinux: Failed to retrieve default panel data using Simple Embedded Manager."));
}

const Result CtrlrLinux::getDefaultResources(MemoryBlock& dataToWrite)
{
    _DBG("CtrlrLinux::getDefaultResources - ENTRY");
    
    // Get the actual plugin binary path.
    File pluginBinary = getVST3PluginPath();
    const String pluginPath = pluginBinary.getFullPathName();
    
    _DBG("CtrlrLinux::getDefaultResources - Attempting to read resource data from path: " + pluginPath);
    
    if (!pluginBinary.existsAsFile()) {
        _DBG("CtrlrLinux::getDefaultResources - ERROR: Plugin binary path does not exist!");
        return (Result::fail("Plugin binary path is invalid or does not exist: " + pluginPath));
    }
    
    // Try new format first (SimpleEmbeddedDataManager)
    SimpleEmbeddedDataManager dataManager(pluginPath.toStdString());
    
    if (dataManager.initialize()) {
        // _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager initialized successfully"); // Removed: Less spam
        
        if (dataManager.readSection(CTRLR_INTERNAL_RESOURCES_SECTION, dataToWrite)) {
            _DBG("CtrlrLinux::getDefaultResources (Simple), read resource data size [" + STR((int32)dataToWrite.getSize()) + "]");
            return (Result::ok());
        } else {
            // _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager found no resources section."); // Removed: Less spam
        }
    } else {
        _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager initialize failed.");
    }
    
    // CRASH FIX: Temporarily removing the old libr fallback to isolate the crash source.
    _DBG("CtrlrLinux::getDefaultResources - SimpleEmbeddedDataManager failed. Aborting resource retrieval.");
    
    return (Result::fail("CtrlrLinux: Failed to retrieve default resources data using Simple Embedded Manager."));
}
const Result CtrlrLinux::getSignature(MemoryBlock &dataToWrite)
{
    // The signature function still uses libr, so we must use the correct path here.
    File pluginBinary = getVST3PluginPath();
    const String pluginPath = pluginBinary.getFullPathName();
    
    _DBG("CtrlrLinux::getSignature - Attempting to read signature from path: " + pluginPath);
    
    libr_file *handle = libr_open (pluginPath.toUTF8().getAddress(), LIBR_READ);

    if (handle == nullptr)
    {
        return (Result::fail ("Linux native, libr_open failed to open binary [self] at path: " + pluginPath));
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
