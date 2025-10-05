#include "stdafx.h"
#define LINUX
#ifdef LINUX
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
#include <cstring>  // ADDED: For strlen, memcpy

extern "C"
{
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

static MemoryBlock stringToFixedBytes(const String& str, int fixedSize) {
    MemoryBlock result;
    result.setSize(fixedSize, true);
    
    const char* chars = str.toUTF8();
    int copySize = jmin(fixedSize, (int)strlen(chars));
    memcpy(result.getData(), chars, copySize);
    
    return result;
}

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
    
    if (count > 0) {
        _DBG("BinaryPatcher: Made " + String(count) + " replacement(s)");
    }
    return count;
}

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

static bool isVST2Plugin() {
    File me = getVST3PluginPath();
    // VST2: Check if we're in ~/.vst or similar VST2 directory AND not in a .vst3 bundle
    return me.hasFileExtension(".so") && 
           !me.getFullPathName().contains(".vst3/") &&
           (me.getParentDirectory().getFullPathName().contains("/.vst") || 
            me.getParentDirectory().getFullPathName().contains("/vst"));
}


// SimpleEmbeddedDataManager class
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
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        
        size_t searchSize = std::min((size_t)8192, fileSize);
        size_t searchStart = fileSize - searchSize;
        
        file.seekg(searchStart);
        std::string buffer(searchSize, '\0');
        file.read(&buffer[0], searchSize);
        
        size_t headerPos = buffer.rfind(MAGIC_HEADER);
        
        if (headerPos != std::string::npos) {
            size_t absolutePos = searchStart + headerPos;
            return parseSections(file, absolutePos + MAGIC_HEADER.length());
        }
        
        return false;
    }

    bool parseSections(std::ifstream& file, size_t startPos) {
        file.seekg(startPos);
        std::string line;
        
        while (std::getline(file, line)) {
            if (line == SECTION_DELIMITER) break;
            
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
                } catch (const std::exception&) {
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
                
                output = rawData;
                return true;
            }
        }
        return false;
    }
    
    bool writeSection(const std::string& sectionName, const MemoryBlock& data) {
        std::ifstream originalFile(filePath, std::ios::binary);
        if (!originalFile.is_open()) return false;
        
        originalFile.seekg(0, std::ios::end);
        size_t originalSize = originalFile.tellg();
        originalFile.seekg(0, std::ios::beg);
        
        MemoryBlock originalData;
        originalData.setSize(originalSize, false);
        originalFile.read(static_cast<char*>(originalData.getData()), originalSize);
        originalFile.close();
        
        size_t dataOffset = originalSize;
        bool sectionExists = false;
        
        for (auto& section : sections) {
            if (section.name == sectionName) {
                section.offset = dataOffset;
                section.size = data.getSize();
                section.compressed = false;
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
        
        std::string tempPath = filePath + ".tmp";
        std::ofstream newFile(tempPath, std::ios::binary);
        if (!newFile.is_open()) return false;
        
        newFile.write(static_cast<const char*>(originalData.getData()), originalData.getSize());
        newFile.write(static_cast<const char*>(data.getData()), data.getSize());
        
        newFile << MAGIC_HEADER;
        for (const auto& section : sections) {
            newFile << section.name << ":" << section.offset << ":" 
                   << section.size << ":" << (section.compressed ? "1" : "0") << "\n";
        }
        newFile << SECTION_DELIMITER << "\n";
        
        newFile.close();
        
        return (rename(tempPath.c_str(), filePath.c_str()) == 0);
    }
};

const std::string SimpleEmbeddedDataManager::MAGIC_HEADER = "\n\n__CTRLR_EMBEDDED_DATA_V2__\n";
const std::string SimpleEmbeddedDataManager::SECTION_DELIMITER = "__END_SECTIONS__";

// Class implementation
CtrlrLinux::CtrlrLinux(CtrlrManager &_owner) : owner(_owner) {}
CtrlrLinux::~CtrlrLinux() {}
const Result CtrlrLinux::exportWithDefaultPanel(CtrlrPanel *panelToWrite, const bool isRestricted, const bool signPanel)
{
    if (panelToWrite == nullptr) {
        return Result::fail("Linux native, panel pointer is invalid");
    }

    File me = getVST3PluginPath();
    File newMe;
    MemoryBlock panelExportData, panelResourcesData;
    
    File parentDir = me.getParentDirectory();
    File contentsDir = parentDir.getParentDirectory();
    File bundleDir = contentsDir.getParentDirectory();
    
    bool isVST3 = bundleDir.getFileName().endsWith(".vst3");
    bool isVST2 = isVST2Plugin(); // Detect VST2
    
    String panelName = File::createLegalFileName(panelToWrite->getProperty(Ids::name));
    
    // Build suggested file based on type
    File suggestedFile;
    String filePattern;
    
    if (isVST3) {
        suggestedFile = bundleDir.getParentDirectory().getChildFile(panelName + ".vst3");
        filePattern = ".vst3";
    } else if (isVST2) {
        suggestedFile = me.getParentDirectory().getChildFile(panelName + ".so");
        filePattern = "*.so";
        _DBG("VST2: suggestedFile = " + suggestedFile.getFullPathName());
    } else {
        // Standalone
        suggestedFile = me.getParentDirectory().getChildFile(panelName).withFileExtension(me.getFileExtension());
        filePattern = me.getFileExtension();
    }

    FileChooser fc(CTRLR_NEW_INSTANCE_DIALOG_TITLE, suggestedFile, filePattern,
                   panelToWrite->getOwner().getProperty(Ids::ctrlrNativeFileDialogs));

    if (fc.browseForFileToSave(true))
    {
        File chosenFile = fc.getResult();
        _DBG("FileChooser returned: " + chosenFile.getFullPathName()); // Debug what FileChooser gives us
        
        if (isVST3) {
            if (!chosenFile.getFileName().endsWith(".vst3")) {
                chosenFile = chosenFile.withFileExtension(".vst3");
            }
            
            File bundleDir = chosenFile;
            File binaryDir = bundleDir.getChildFile("Contents/x86_64-linux");
            String binaryName = bundleDir.getFileNameWithoutExtension() + ".so";
            File binaryFile = binaryDir.getChildFile(binaryName);
            
            if (!binaryDir.createDirectory()) {
                return Result::fail("Failed to create VST3 bundle directory structure");
            }
            
            if (!me.copyFileTo(binaryFile)) {
                return Result::fail("Linux native, copyFileTo failed");
            }
            
            newMe = binaryFile;
        } 
else if (isVST2) {
    _DBG("VST2 export: chosenFile from FileChooser = " + chosenFile.getFullPathName());
    
    // ALWAYS ensure .so extension for VST2
    if (!chosenFile.getFileName().endsWith(".so")) {
        chosenFile = File(chosenFile.getFullPathName() + ".so");
        _DBG("VST2 export: Added .so extension, now = " + chosenFile.getFullPathName());
    } else {
        _DBG("VST2 export: Already has .so extension");
    }
    
    newMe = chosenFile;
    
    if (!me.copyFileTo(newMe)) {
        return Result::fail("Linux native, copyFileTo failed");
    }
    
    _DBG("VST2 export: Final file created = " + newMe.getFullPathName());
}
        else {
            // Standalone
            newMe = chosenFile;
            if (!me.copyFileTo(newMe)) {
                return Result::fail("Linux native, copyFileTo failed");
            }
        }
    }
    else
    {
        return Result::fail("User cancelled the export operation.");
    }

    // Export panel data
    CtrlrPanel p(owner, "", 0);
    String error = p.exportPanel(panelToWrite, File(), newMe, &panelExportData, &panelResourcesData, isRestricted);
    if (error != "") {
        return Result::fail("CtrlrPanel::exportPanel failed: " + error);
    }

    // Perform binary patching for VST3 AND VST2
    if (isVST3 || isVST2) {
        String pluginName = panelToWrite->getProperty(Ids::name).toString();
        String pluginCode = panelToWrite->getProperty(Ids::panelInstanceUID).toString();
        String manufacturerName = panelToWrite->getProperty(Ids::panelAuthorName).toString();
        String manufacturerCode = panelToWrite->getProperty(Ids::panelInstanceManufacturerID).toString();
        
        MemoryBlock binaryData;
        if (newMe.loadFileAsData(binaryData)) {
            MemoryBlock pluginNameBytes = stringToFixedBytes(pluginName, 32);
            MemoryBlock pluginCodeBytes = stringToFixedBytes(pluginCode, 4);
            MemoryBlock manufacturerNameBytes = stringToFixedBytes(manufacturerName, 16);
            MemoryBlock manufacturerCodeBytes = stringToFixedBytes(manufacturerCode, 4);
            
            MemoryBlock searchPluginName = hexToBytes("43 74 72 6C 72 58 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20");
            MemoryBlock searchPluginCode = hexToBytes("63 54 72 58");
            MemoryBlock searchManufacturerName = hexToBytes("43 74 72 6C 72 58 20 50 72 6F 6A 65 63 74 20 20");
            MemoryBlock searchManufacturerCode = hexToBytes("63 54 72 6C");
            
            int totalReplacements = 0;
            totalReplacements += replaceAllOccurrences(binaryData, searchPluginName, pluginNameBytes);
            totalReplacements += replaceAllOccurrences(binaryData, searchPluginCode, pluginCodeBytes);
            totalReplacements += replaceAllOccurrences(binaryData, searchManufacturerName, manufacturerNameBytes);
            totalReplacements += replaceAllOccurrences(binaryData, searchManufacturerCode, manufacturerCodeBytes);
            
            _DBG("Binary patching complete: " + String(totalReplacements) + " replacements");
            
            if (!newMe.replaceWithData(binaryData.getData(), binaryData.getSize())) {
                return Result::fail("Failed to write patched binary");
            }
        }
    }
    
    // Embed data using SimpleEmbeddedDataManager
    SimpleEmbeddedDataManager dataManager(newMe.getFullPathName().toStdString());
    dataManager.initialize();
    
    if (!dataManager.writeSection(CTRLR_INTERNAL_PANEL_SECTION, panelExportData)) {
        return Result::fail("Failed to write panel data");
    }
    
    if (panelResourcesData.getSize() > 0) {
        if (!dataManager.writeSection(CTRLR_INTERNAL_RESOURCES_SECTION, panelResourcesData)) {
            return Result::fail("Failed to write resources");
        }
    }

    if (chmod(newMe.getFullPathName().toUTF8().getAddress(), 
              S_IRUSR | S_IWUSR | S_IXUSR | S_IXOTH | S_IRGRP | S_IXGRP | S_IROTH)) {
        return Result::fail("chmod failed");
    }

    return Result::ok();
}

const Result CtrlrLinux::getDefaultPanel(MemoryBlock& dataToWrite)
{
#ifdef DEBUG_INSTANCE
    File temp("/home/r.kubiak/devel/debug.bpanelz");
    temp.loadFileAsData(dataToWrite);
    return Result::ok();
#endif

    File pluginBinary = getVST3PluginPath();
    SimpleEmbeddedDataManager dataManager(pluginBinary.getFullPathName().toStdString());
    
    if (dataManager.initialize() && dataManager.readSection(CTRLR_INTERNAL_PANEL_SECTION, dataToWrite)) {
        return Result::ok();
    }
    
    return Result::fail("Failed to retrieve panel data");
}

const Result CtrlrLinux::getDefaultResources(MemoryBlock& dataToWrite)
{
    File pluginBinary = getVST3PluginPath();
    SimpleEmbeddedDataManager dataManager(pluginBinary.getFullPathName().toStdString());
    
    if (dataManager.initialize() && dataManager.readSection(CTRLR_INTERNAL_RESOURCES_SECTION, dataToWrite)) {
        return Result::ok();
    }
    
    return Result::fail("Failed to retrieve resources");
}

const Result CtrlrLinux::getSignature(MemoryBlock &dataToWrite)
{
    File pluginBinary = getVST3PluginPath();
    libr_file *handle = libr_open(pluginBinary.getFullPathName().toUTF8().getAddress(), LIBR_READ);

    if (handle == nullptr) {
        return Result::fail("libr_open failed");
    }

    size_t signatureDataSize;
    char *signatureData = (char *)libr_malloc(handle, (char *)CTRLR_INTERNAL_RESOURCES_SECTION, &signatureDataSize);

    if (signatureData == nullptr) {
        libr_close(handle);
        return Result::fail("libr_malloc didn't find signature");
    }

    dataToWrite.append(signatureData, signatureDataSize);
    libr_close(handle);

    return Result::ok();
}

const Result CtrlrLinux::sendKeyPressEvent(const KeyPress &event)
{
    return ctrlr_sendKeyPressEvent(event);
}

#endif