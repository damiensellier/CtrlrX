#include "stdafx.h"
#include "stdafx_luabind.h"
static const int zero=0;
#ifdef __APPLE__
#include <memory>
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrMac.h"
#include "CtrlrMacros.h"
#include "CtrlrLog.h"

#include <random>
#include <fstream>


CtrlrMac::CtrlrMac(CtrlrManager &_owner) : owner(_owner)
{
}

CtrlrMac::~CtrlrMac()
{
}

const Result CtrlrMac::exportWithDefaultPanel(CtrlrPanel* panelToWrite, const bool isRestricted, const bool signPanel) {
    if (panelToWrite == nullptr) {
        return (Result::fail("MAC native, panel pointer is invalid"));
    }
    
    File me = File::getSpecialLocation(File::currentApplicationFile);
    File newMe;
    MemoryBlock panelExportData, panelResourcesData;
    String error;
    PluginLogger logger(me); // Create logger instance
    
    logger.log("Starting exportWithDefaultPanel");
    
    // Defines FileChooser and source file name to clone and mod as output file
    auto typeOS = juce::SystemStats::getOperatingSystemType();
    std::cout << "MAC native, launch fileChooser to select export destination path. typeOS : " << typeOS << std::endl;
    logger.log("MAC native, launch fileChooser to select export destination path. typeOS : " + typeOS);

    auto nameOS = juce::SystemStats::getOperatingSystemName();
    std::cout << "MAC native, launch fileChooser to select export destination path. nameOS : " << nameOS << std::endl;
    logger.log("MAC native, launch fileChooser to select export destination path. nameOS : " + nameOS);
    
    bool nativeFileChooser = !( typeOS == juce::SystemStats::OperatingSystemType::MacOSX_10_15 // For OSX Catalina
                               || typeOS == juce::SystemStats::OperatingSystemType::MacOS_11 //  For macOS BigSur
                               || typeOS == juce::SystemStats::OperatingSystemType::MacOS_12 //  For macOS Monterey
                               );

    File defaultDirectory = me.getParentDirectory().getChildFile(File::createLegalFileName(panelToWrite->getProperty(Ids::name))).withFileExtension(me.getFileExtension());
    
    fc = std::make_unique<FileChooser> (CTRLR_NEW_INSTANCE_DIALOG_TITLE,
                                            me.getParentDirectory().getChildFile(File::createLegalFileName(panelToWrite->getProperty(Ids::name))).withFileExtension(me.getFileExtension()),
                                            me.getFileExtension(),
                                            nativeFileChooser); // panelToWrite->getOwner().getProperty(Ids::ctrlrNativeFileDialogs)); // if vst3, won't work since there's no ctrlr.settings
    
    // Launch FileChooser to export file and define the new output file name and extension
    // browseForFileToSave(true) to show "cancel | Save" instead of "Cancel | Open" buttons won't work. It will show a filename box (we don't want that) and will force to save a the file with doubled extension such as filename.vst3..vst3
    if (fc->browseForDirectory()) {
        newMe = fc->getResult().getChildFile(File::createLegalFileName(panelToWrite->getProperty(Ids::name).toString() + me.getFileExtension()));
        if (!me.copyDirectoryTo(newMe)) {
            return (Result::fail("MAC native, copyDirectoryTo from \"" + me.getFullPathName() + "\" to \"" + newMe.getFullPathName() + "\" failed"));
            logger.log("MAC native, copyDirectoryTo from \"" + me.getFullPathName() + "\" to \"" + newMe.getFullPathName() + "\" failed");
        }
    } else {
        return (Result::fail("MAC native, browse for directory dialog failed"));
        logger.log("MAC native, browse for directory dialog failed");
    }
    
    
    Result res = setBundleInfo(panelToWrite, newMe);
    if (!res.wasOk()) {
        return (res);
    }
    
    res = setBundleInfoCarbon(panelToWrite, newMe);
    if (!res.wasOk()) {
        return (res);
    }
    
    if ((error = CtrlrPanel::exportPanel(panelToWrite, File(), newMe, &panelExportData, &panelResourcesData, isRestricted)) == "") {
        File panelFile = newMe.getChildFile("Contents/Resources/" + String(CTRLR_MAC_PANEL_FILE));
        File resourcesFile = newMe.getChildFile("Contents/Resources/" + String(CTRLR_MAC_RESOURCES_FILE));
        File fileEncrypted = newMe.getChildFile("Contents/Resources/"+String(CTRLR_MAC_PANEL_FILE)+String("BF")); // Added v5.6.31
        File executableFile = me.getChildFile("Contents/MacOS/CtrlrX");
        String fileExtension = me.getFileExtension();
        std::cout << "CtrlrX source fileExtension is : " << fileExtension << std::endl;
        logger.log("CtrlrX source fileExtension is :" + fileExtension);
        
        if (panelFile.create() && panelFile.hasWriteAccess()){
            if (!panelFile.replaceWithData(panelExportData.getData(), panelExportData.getSize()))
            {
                return (Result::fail("MAC native, failed to write panel file at: " + panelFile.getFullPathName()));
                logger.log("MAC native, failed to write panel file at: " + panelFile.getFullPathName());
            }
            else {
                logger.log("MAC native, succeeded to write panel file at: " + panelFile.getFullPathName());
            }
        }
        
        if (resourcesFile.create() && resourcesFile.hasWriteAccess())
        {
            if (!resourcesFile.replaceWithData(panelResourcesData.getData(), panelResourcesData.getSize()))
            {
                return (Result::fail("MAC native, failed to write resources file at: " + resourcesFile.getFullPathName()));
                logger.log("MAC native, failed to write resources file at: " + resourcesFile.getFullPathName());
            }
            else {
                logger.log("MAC native, succeeded to write resources file at: " + resourcesFile.getFullPathName());
            }
        }
        
        // Introduce a delay before ecncrypting
        logger.log("Thread sleep to delay encryption task.");
        juce::Thread::sleep(500); // milliseconds (250ms should be ok, adjust as needed)
        logger.log("Thread restarted for encryption task.");
        
        
        // Encrypt the Gzipped panel file as a new Blowfish encrypyted derived file. Added v5.6.31
        if (panelFile.existsAsFile()) {
            // Read file contents into a MemoryBlock
            MemoryBlock dataToEncrypt;
            if (!panelFile.loadFileAsData(dataToEncrypt)) {
                // Handle error: Failed to read source file
                return Result::fail("Error: Failed to read source file");
                logger.log("Error: Failed to read source file");
            }
            
            // Define the BlowFish encryption key as string
            String keyString = "Fw2rXbRh74Vxe53BfapNvGjz8kTDLYmU"; // Replace with your actual key (security!) Updated v5.6.32
            
            // Key is provided, proceed with encryption
            BlowFish blowfish(keyString.toUTF8(), keyString.getNumBytesAsUTF8());
            
            // Encrypt the data in-place (modifies the original file)
            blowfish.encrypt(dataToEncrypt);
            
            // Create the encrypted file
            fileEncrypted.create();
            if (!fileEncrypted.existsAsFile()) {
                // Handle error: Failed to create encrypted file
                return Result::fail("Error: Failed to create encrypted file");
                logger.log("Error: Failed to create encrypted file");
            }
            
            // Open encrypted file for writing
            FileOutputStream fos(fileEncrypted);
            if (!fos.openedOk()) {
                // Handle error: Failed to open encrypted file for writing
                return Result::fail("Error: Failed to open encrypted file");
                logger.log("Error: Failed to open encrypted file");
            }
            
            // Write encrypted data to the file
            fos.write(dataToEncrypt.getData(), dataToEncrypt.getSize());
            fos.flush();
            
            // Encryption successful, delete the source file
            if (!panelFile.deleteFile()) {
                return Result::fail("Failed to delete source file " + panelFile.getFullPathName());
                logger.log("Failed to delete source file " + panelFile.getFullPathName());
            }
        }
        
        if (executableFile.existsAsFile()) {
            
            if (fileExtension == (".vst3")) {
                std::cout << "fileExtension is : " << fileExtension << std::endl;
                logger.log("fileExtension is : " + fileExtension );
                
                // Replace the stock VST3 plugin identifiers with the panel to export ones.
                const bool replaceVst3PluginIds = panelToWrite->getProperty(Ids::panelReplaceVst3PluginIds);
                
                if (replaceVst3PluginIds) {
                    std::cout << "Replace the VST3 plugin identifiers with the panel ones : " << replaceVst3PluginIds << std::endl;
                    logger.log("Replace the VST3 plugin identifiers with the panel ones : " + replaceVst3PluginIds);
                    
                    MemoryBlock executableData;
                    if (executableFile.loadFileAsData(executableData))
                    {
                        std::cout << "Executable loaded into memory successfully." << std::endl;
                        logger.log("Executable loaded into memory successfully.");
                        
                        // Convert plugin name to hex for substitution
                        String pluginName = panelToWrite->getProperty(Ids::name).toString();
                        int pluginNameMaxLength = 32;
                        MemoryBlock pluginNameHex;
                        hexStringToBytes(pluginName, pluginNameMaxLength, pluginNameHex);
                        std::cout << "pluginName: " << pluginName << std::endl;
                        std::cout << "pluginNameHex representation: " << bytesToHexString(pluginNameHex) << std::endl;
                        logger.log("pluginName: " + pluginName);
                        logger.log("pluginNameHex representation: " + bytesToHexString(pluginNameHex));
                        
                        // Convert plugin code to hex for substitution
                        String pluginCode = panelToWrite->getProperty(Ids::panelInstanceUID).toString();
                        int pluginCodeMaxLength = 4;
                        MemoryBlock pluginCodeHex;
                        hexStringToBytes(pluginCode, pluginCodeMaxLength, pluginCodeHex);
                        std::cout << "pluginCode: " << pluginCode << std::endl;
                        std::cout << "pluginCodeHex representation: " << bytesToHexString(pluginCodeHex) << std::endl;
                        logger.log("pluginCode: " + pluginCode);
                        logger.log("pluginCodeHex representation: " + bytesToHexString(pluginCodeHex));
                        
                        // Convert manufacturer name to hex for substitution
                        String manufacturerName = panelToWrite->getProperty(Ids::panelAuthorName).toString();
                        int manufacturerNameMaxLength = 16;
                        MemoryBlock manufacturerNameHex;
                        hexStringToBytes(manufacturerName, manufacturerNameMaxLength, manufacturerNameHex);
                        std::cout << "manufacturerName: " << manufacturerName << std::endl;
                        std::cout << "manufacturerNameHex representation: " << bytesToHexString(manufacturerNameHex) << std::endl;
                        logger.log("manufacturerName: " + manufacturerName);
                        logger.log("manufacturerNameHex representation: " + bytesToHexString(manufacturerNameHex));
                        
                        // Convert plugin code to hex for substitution
                        String manufacturerCode = panelToWrite->getProperty(Ids::panelInstanceManufacturerID).toString();
                        int manufacturerCodeMaxLength = 4;
                        MemoryBlock manufacturerCodeHex;
                        hexStringToBytes(manufacturerCode, manufacturerCodeMaxLength, manufacturerCodeHex);
                        std::cout << "manufacturerCode: " << manufacturerCode << std::endl;
                        std::cout << "manufacturerCodeHex representation: " << bytesToHexString(manufacturerCodeHex) << std::endl;
                        logger.log("manufacturerCode: " + manufacturerCode);
                        logger.log("manufacturerCodeHex representation: " + bytesToHexString(manufacturerCodeHex));
                        
                        // Convert version Major to hex for substitution
                        String versionMajor = panelToWrite->getProperty(Ids::panelVersionMajor).toString();
                        int versionMajorMaxLength = 2;
                        MemoryBlock versionMajorHex;
                        hexStringToBytes(versionMajor, versionMajorMaxLength, versionMajorHex);
                        std::cout << "versionMajor: " << versionMajor << std::endl;
                        std::cout << "versionMajorHex representation: " << bytesToHexString(versionMajorHex) << std::endl;
                        logger.log("versionMajor: " + versionMajor);
                        logger.log("versionMajorHex representation: " + bytesToHexString(versionMajorHex));
                        
                        // Convert version minor to hex for substitution
                        String versionMinor = panelToWrite->getProperty(Ids::panelVersionMinor).toString();
                        int versionMinorMaxLength = 2;
                        MemoryBlock versionMinorHex;
                        hexStringToBytes(versionMinor, versionMinorMaxLength, versionMinorHex);
                        std::cout << "versionMinor: " << versionMinor << std::endl;
                        std::cout << "versionMinorHex representation: " << bytesToHexString(versionMinorHex) << std::endl;
                        logger.log("versionMinor: " + versionMinor);
                        logger.log("versionMinorHex representation: " + bytesToHexString(versionMinorHex));
                        
                        // Convert plugType to hex for substitution
                        String plugType = panelToWrite->getProperty(Ids::panelPlugType).toString();
                        int plugTypeMaxLength = 16;
                        MemoryBlock plugTypeHex;
                        hexStringToBytes(plugType, plugTypeMaxLength, plugTypeHex);
                        std::cout << "plugType: " << plugType << std::endl;
                        std::cout << "plugTypeHex representation: " << bytesToHexString(plugTypeHex) << std::endl;
                        logger.log("plugType: " + plugType);
                        logger.log("plugTypeHex representation: " + bytesToHexString(plugTypeHex));
                        
                        // Substitution process
                        // Replace CtrlrX plugin name "CtrlrX          "
                        MemoryBlock searchPluginNameHex;
                        hexStringToBytes("43 74 72 6C 72 58 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20", searchPluginNameHex);
                        replaceOccurrences(executableData, searchPluginNameHex, pluginNameHex, -1);
                        std::cout << "Plugin name replacement complete." << std::endl;
                        logger.log("Plugin name replacement complete.");
                        
                        // Replace CtrlrX plugin manufacturer code "cTrX"
                        MemoryBlock searchPluginCodeHex;
                        hexStringToBytes("63 54 72 58", searchPluginCodeHex);
                        replaceOccurrences(executableData, searchPluginCodeHex, pluginCodeHex, -1);
                        std::cout << "Plugin code replacement complete." << std::endl;
                        logger.log("Plugin code replacement complete.");
                        
                        // Replace "CtrlrX Project  "
                        MemoryBlock searchManufacturerNameHex;
                        hexStringToBytes("43 74 72 6C 72 58 20 50 72 6F 6A 65 63 74 20 20", searchManufacturerNameHex);
                        replaceOccurrences(executableData, searchManufacturerNameHex, manufacturerNameHex, -1);
                        std::cout << "Manufacturer name replacement complete." << std::endl;
                        logger.log("Manufacturer name replacement complete.");
                        
                        // Replace CtrlrX plugin code "cTrl"
                        MemoryBlock searchManufacturerCodeHex;
                        hexStringToBytes("63 54 72 6C", searchManufacturerCodeHex);
                        replaceOccurrences(executableData, searchManufacturerCodeHex, manufacturerCodeHex, -1);
                        std::cout << "Manufacturer code replacement complete." << std::endl;
                        logger.log("Manufacturer code replacement complete.");
                        
                        
                        // Replace plugType
                        
                        // If searchData is in one block, not split
                        MemoryBlock searchPlugTypeHex, searchPlugTypeHexSynth, searchPlugTypeHexTools, searchPlugTypeBytesToolsInserted, searchPlugTypeBytesSynthInserted, plugTypeBytesInsertData;
                        
                        String plugTypeHexInstrumentSynth = "49 6E 73 74 72 75 6D 65 6E 74 7C 53 79 6E 74 68";
                        String plugTypeHexInstrumentTools = "49 6E 73 74 72 75 6D 65 6E 74 7C 53 79 6E 74 68";
                        
                        hexStringToBytes(plugTypeHexInstrumentSynth, searchPlugTypeHexTools); // plugType "Instrument|Tools"
                        // replaceOccurrences(executableData, searchPlugTypeHexTools, plugTypeHex, -1); // If no insertion is required
                        // std::cout << "VST3 plugin type replacement complete. (Instrument|Tools, replaced by " << CtrlrMac::hexStringToText(plugTypeHex) << ")." << std::endl;
                        // logger.log("VST3 plugin type replacement complete. (Instrument|Tools, replaced by " + CtrlrMac::hexStringToText(plugTypeHex) + ")." );
                        
                        hexStringToBytes(plugTypeHexInstrumentTools, searchPlugTypeHexSynth); // plugType "Instrument|Synth"
                        // replaceOccurrences(executableData, searchPlugTypeHexSynth, plugTypeHex, -1); // If no insertion is required
                        // std::cout << "VST3 plugin type replacement complete. (Instrument|Synth, replaced by " << CtrlrMac::hexStringToText(plugTypeHex) << ")." << std::endl;
                        // logger.log("VST3 plugin type replacement complete. (Instrument|Synth, replaced by " + CtrlrMac::hexStringToText(plugTypeHex) + ")." );
                        
                        
                        // If searchData is split in two parts with an assembly markup inserted
                        hexStringToBytes("48 89 43 78 48 B8", plugTypeBytesInsertData); // Convert insert "HCxH¸"
                        
                        // For "Instrument|Synth" with insert "InstrumeHCxH¸nt|Synth"
                        // String plugTypeHexInstrumentSynthInserted = "49 6E 73 74 72 75 6D 65 48 89 43 78 48 B8 6E 74 7C 53 79 6E 74 68"; // plugType "Instrument|Synth" with insert "InstrumeHCxH¸nt|Synth"
                        // hexStringToBytes(plugTypeHexInstrumentSynthInserted, searchPlugTypeBytesSynthInserted);
                        // replaceOccurrencesIfSplitted(executableData, searchPlugTypeBytesSynthInserted, plugTypeBytesInsertData, plugTypeHex, 8, 1);
                        // std::cout << "VST3 plugin type replacement process complete. (Instrument|Synth, replaced by " << CtrlrMac::hexStringToText(plugTypeHex) << ")." << std::endl;
                        // logger.log("VST3 plugin type replacement process complete. (Instrument|Synth, replaced by " + CtrlrMac::hexStringToText(plugTypeHex) + ")." );
                        
                        // For "Instrument|Tools" with insert "InstrumeHCxH¸nt|Tools"
                        String plugTypeHexInstrumentToolsInserted = "49 6E 73 74 72 75 6D 65 48 89 43 78 48 B8 6E 74 7C 53 79 6E 74 68";
                        hexStringToBytes("49 6E 73 74 72 75 6D 65 48 89 43 78 48 B8 6E 74 7C 54 6F 6F 6C 73", searchPlugTypeBytesToolsInserted); // plugType "Instrument|Synth" with insert "InstrumeHCxH¸nt|Tools"
                        replaceOccurrencesIfSplitted(executableData, searchPlugTypeBytesToolsInserted, plugTypeBytesInsertData, plugTypeHex, 8, 1);
                        std::cout << "VST3 plugin type replacement complete. (Instrument|Tools, replaced by " << CtrlrMac::hexStringToText(plugTypeHex) << ")." << std::endl;
                        logger.log("VST3 plugin type replacement complete. (Instrument|Tools, replaced by " + CtrlrMac::hexStringToText(plugTypeHex) + ")." );
                        
                        
                        // Save the modified executable
                        File newExecutableFile = newMe.getChildFile("Contents/MacOS/CtrlrX");
                        if (!newExecutableFile.replaceWithData(executableData.getData(), executableData.getSize()))
                        {
                            std::cout << "Failed to write modified executable data." << std::endl;
                            logger.log("MAC Native: Failed to write modified executable data");
                            return (Result::fail("MAC Native: Failed to write modified executable data"));
                        }
                        std::cout << "Modified executable saved successfully." << std::endl;
                        logger.log("Modified executable saved successfully.");
                    } else {
                        logger.log("Failed to load executable into memory.");
                        return Result::fail("Failed to load executable into memory.");
                    }
                } // End if replaceVst3PluginIds
                else {
                    std::cout << "replaceVst3PluginIds set to false, Vst3 IDs replacement skipped." << std::endl;
                    logger.log("replaceVst3PluginIds set to false, Vst3 IDs replacement skipped.");
                }
                
                // Introduce a delay before codesigning
                logger.log("Thread sleep to delay codesigning task.");
                juce::Thread::sleep(500); // milliseconds (250ms should be ok, adjust as needed)
                logger.log("Thread restarted for codesigning task.");
                
                // Now, codesign the newMe file and return result
                juce::String newMePathName = newMe.getFullPathName();
                std::cout << "File FullPathname: " << newMePathName << std::endl;
                logger.log("File FullPathname: " + newMePathName);
                
                juce::String panelCertificateMacIdentity = panelToWrite->getProperty(Ids::panelCertificateMacId).toString();
                std::cout << "MAC Certificate Identity: " << panelCertificateMacIdentity << std::endl;
                logger.log("MAC Certificate Identity: " + panelCertificateMacIdentity);
                
                const Result codesignResult = codesignFileMac(newMePathName, panelCertificateMacIdentity);
                if (!codesignResult.wasOk()) {
                    logger.logResult(codesignResult);
                    return (codesignResult);
                }
                logger.log("Codesigning successful.");
                logger.logResult(codesignResult); // Added Logger for successful codesign.
            } // end if is .vst3
            
            else { // if is not .vst3
                // Now, codesign the newMe file and return result
                const juce::String newMePathName = newMe.getFullPathName();
                std::cout << "File FullPathname: " << newMePathName << std::endl;
                
                const juce::String panelCertificateMacIdentity = panelToWrite->getProperty(Ids::panelCertificateMacId).toString();
                std::cout << "MAC Certificate Identity: " << panelCertificateMacIdentity << std::endl;
                
                const Result codesignResult = codesignFileMac(newMePathName, panelCertificateMacIdentity);
                if (!codesignResult.wasOk()) {
                    logger.logResult(codesignResult);
                    return (codesignResult);
                }
                logger.log("Codesigning successful.");
                logger.logResult(codesignResult); // Added Logger for successful codesign.
            } // end if is not .vst3
            
        } // end if exist as file
        
    } // end if error
    
} // end result() overall function


const Result CtrlrMac::codesignFileMac(const juce::String& newMePathName, const juce::String& panelCertificateMacIdentity) {
    juce::StringArray commandParts;
    commandParts.add("codesign");
    commandParts.add("-f");
    commandParts.add("-s");
    
    if (panelCertificateMacIdentity.isNotEmpty()) //Check if there is a certificate identity
    {
        commandParts.add(panelCertificateMacIdentity); // Use the provided certificate identity
    }
    else
    {
        commandParts.add("-"); // Use the default identity
    }
    
    commandParts.add(newMePathName);
    
    juce::ChildProcess childProcess;
    if (childProcess.start(commandParts)) {
        childProcess.waitForProcessToFinish(-1);
        
        if (!childProcess.isRunning()) { // Check if process has finished
            if (childProcess.getExitCode() == 0) {
                return juce::Result::ok(); // Codesign successful
                std::cout << "Codesign successful. " << newMePathName << std::endl;
                
            } else {
                return juce::Result::fail("Codesign failed with exit code: " + juce::String(childProcess.getExitCode())); // Codesign failed
            }
        } else {
            return juce::Result::fail("Codesign process did not finish properly."); // Process still running
        }
        
    } else {
        return juce::Result::fail("Failed to start codesign process."); // Failed to start process
    }
}

// Convert hex string to binary data
void CtrlrMac::hexStringToBytes(const String& hexString, MemoryBlock& result) {
    result.reset();
    String cleanedHex = hexString.removeCharacters(" \t\r\n");

    for (int i = 0; i < cleanedHex.length(); i += 2)
    {
        if (i + 1 < cleanedHex.length())
        {
            String byteStr = cleanedHex.substring(i, i + 2);
            int byteVal = byteStr.getHexValue32();
            uint8 byte = static_cast<uint8>(byteVal);
            result.append(&byte, 1);
        }
    }
}


// Convert hex string to binary data with defined maxLength
void CtrlrMac::hexStringToBytes(const juce::String& hexString, int maxLength, juce::MemoryBlock& result) {
    result.setSize(0); // Clear the MemoryBlock before use
    String sanitizedHexString = hexString.removeCharacters("\t\r\n");
    std::vector<uint8> bytes;

    for (int i = 0; i < std::min((int)sanitizedHexString.length(), maxLength); ++i) {
        bytes.push_back(static_cast<uint8>(sanitizedHexString[i]));
    }

    while (bytes.size() < maxLength) {
        bytes.push_back(0); // Pad with zeros
    }

    result.append(bytes.data(), bytes.size());
}


// Convert binary data to hex string
juce::String CtrlrMac::bytesToHexString(const juce::MemoryBlock& memoryBlock, bool addSpaces) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < memoryBlock.getSize(); ++i) {
        ss << std::setw(2) << static_cast<int>(static_cast<const uint8*>(memoryBlock.getData())[i]);
        if (addSpaces) {
            ss << " ";
        }
    }
    return juce::String(ss.str());
}


// Convert binary data to text string
juce::String CtrlrMac::hexStringToText(const juce::MemoryBlock& memoryBlock) {
    juce::String hexString = CtrlrMac::bytesToHexString(memoryBlock);
    juce::String textString;
    
    for (int i = 0; i < hexString.length(); i += 2) {
        if (i + 1 < hexString.length()) {
            juce::String byteStr = hexString.substring(i, i + 2);
            int byteVal = byteStr.getHexValue32();
            textString += static_cast<char>(byteVal);
        }
    }
    return textString;
}


// Replace all occurrences of searchData with replaceData in the targetData
// Replace occurrences implementation when searchData is in one piece
void CtrlrMac::replaceOccurrences(juce::MemoryBlock& targetData, const juce::MemoryBlock& searchData, const juce::MemoryBlock& replaceData, int maxOccurrences) {
    File me = File::getSpecialLocation(File::currentApplicationFile);
    PluginLogger logger(me);

    if (searchData.getSize() != replaceData.getSize()) {
        std::cerr << "Invalid search/replace data sizes" << std::endl;
        logger.log("Invalid search/replace data sizes");
        return;
    }

    const uint8* rawData = static_cast<const uint8*>(targetData.getData());
    size_t dataSize = targetData.getSize();
    size_t searchSize = searchData.getSize();
    int replacements = 0;

    for (size_t i = 0; i <= dataSize - searchSize && (maxOccurrences == -1 || replacements < maxOccurrences); ++i) {
        if (memcmp(rawData + i, searchData.getData(), searchSize) == 0) {
            targetData.copyFrom(replaceData.getData(), i, replaceData.getSize());
            rawData = static_cast<const uint8*>(targetData.getData());
            replacements++;

            std::cout << "Replacement occurrence " << replacements << ". Searched text: " << CtrlrMac::hexStringToText(searchData) << ", Replacing text: " << CtrlrMac::hexStringToText(replaceData) << std::endl;
            logger.log(juce::String("Replacement occurrence ") + juce::String(replacements) + ". Searched text: " + CtrlrMac::hexStringToText(searchData) + ", Replacing text: " + CtrlrMac::hexStringToText(replaceData));
           }
    }

    if (replacements == 0) {
        std::cout << "Search data not found for replacement. Searched text: " << CtrlrMac::hexStringToText(searchData) << ", Replacing text: " << CtrlrMac::hexStringToText(replaceData) << std::endl;
        logger.log("Search data not found for replacement. Searched text: " + CtrlrMac::hexStringToText(searchData) + ", Replacing text: " + CtrlrMac::hexStringToText(replaceData));
    }
}


// Replacement function if assembly markup is already inserted in the string searchData
void CtrlrMac::replaceOccurrencesIfSplitted(juce::MemoryBlock& targetData, const juce::MemoryBlock& searchData, const juce::MemoryBlock& insertData, juce::MemoryBlock& replaceData, size_t insertAfterN, int maxOccurrences) {
    File me = File::getSpecialLocation(File::currentApplicationFile);
    PluginLogger logger(me);

    logger.log("We are into the function replaceOccurrencesIfSplitted() after PluginLogger casting.");

    // 1. Create combined searched data (searchData already has insertData)
    if (insertAfterN > searchData.getSize()) {
        logger.log("Error: insertAfterN is out of range. insertAfterN: " + juce::String(insertAfterN) + ", searchData size: " + juce::String(searchData.getSize()));
        return;
    }

    juce::MemoryBlock combinedSearchData(searchData.getData(), searchData.getSize());
    logger.log("combinedSearchData append step 1 size: " + juce::String(combinedSearchData.getSize()));
    logger.log("combinedSearchData append step 1 value: " + CtrlrMac::hexStringToText(combinedSearchData));
    logger.log("combinedSearchData append step 1 value: " + CtrlrMac::bytesToHexString(combinedSearchData));
    
    logger.log("targetData size: " + juce::String(targetData.getSize()));

    logger.log("combinedSearchData append step 2 size: " + juce::String(combinedSearchData.getSize()));
    logger.log("combinedSearchData append step 2 value: " + CtrlrMac::hexStringToText(combinedSearchData));
    logger.log("combinedSearchData append step 2 value: " + CtrlrMac::bytesToHexString(combinedSearchData));

    
    // 2. Create modified replacing data (insert insertData into replaceData)
    juce::MemoryBlock modifiedReplaceData;

    if (insertAfterN > replaceData.getSize()) {
        logger.log("Error: insertAfterN is out of range for replaceData. insertAfterN: " + juce::String(insertAfterN) + ", replaceSize: " + juce::String(replaceData.getSize()));
        return;
    }

    modifiedReplaceData.append(replaceData.getData(), insertAfterN);
    logger.log("modifiedReplaceData append step 1 size: " + juce::String(modifiedReplaceData.getSize()));
    logger.log("modifiedReplaceData append step 1 value: " + CtrlrMac::hexStringToText(modifiedReplaceData));
    logger.log("modifiedReplaceData append step 1 value: " + CtrlrMac::bytesToHexString(modifiedReplaceData));
    
    modifiedReplaceData.append(insertData.getData(), insertData.getSize());
    logger.log("modifiedReplaceData append step 2 size: " + juce::String(modifiedReplaceData.getSize()));
    logger.log("modifiedReplaceData append step 2 value: " + CtrlrMac::hexStringToText(modifiedReplaceData));
    logger.log("modifiedReplaceData append step 2 value: " + CtrlrMac::bytesToHexString(modifiedReplaceData));
    
    modifiedReplaceData.append(static_cast<const uint8*>(replaceData.getData()) + insertAfterN, replaceData.getSize() - insertAfterN);
    logger.log("modifiedReplaceData append step 3 size: " + juce::String(modifiedReplaceData.getSize()));
    logger.log("modifiedReplaceData append step 3 value: " + CtrlrMac::hexStringToText(modifiedReplaceData));
    logger.log("modifiedReplaceData append step 3 value: " + CtrlrMac::bytesToHexString(modifiedReplaceData));

    // Adjust modifiedReplaceData size to 22 bytes
    if (modifiedReplaceData.getSize() < 22) {
        modifiedReplaceData.setSize(22);
        // Add null padding if necessary.
        memset(static_cast<uint8*>(modifiedReplaceData.getData()) + 16, 0, 6);
    }

    // 3. Size check
    if (combinedSearchData.getSize() != modifiedReplaceData.getSize()) {
        logger.log("Error: combinedSearchData and modifiedReplaceData sizes do not match.");
        logger.log("combinedSearchData size: " + juce::String(combinedSearchData.getSize()));
        logger.log("modifiedReplaceData size: " + juce::String(modifiedReplaceData.getSize()));
        return;
    }

    logger.log("insertAfterN: " + juce::String(insertAfterN) + ", replaceSize: " + juce::String(replaceData.getSize()));
    logger.log("combinedSearchData size: " + juce::String(combinedSearchData.getSize()));
    logger.log("modifiedReplaceData size: " + juce::String(modifiedReplaceData.getSize()));

    // 4. Replacement loop
    const uint8* rawData = static_cast<const uint8*>(targetData.getData());
    size_t dataSize = targetData.getSize();
    size_t searchSize = combinedSearchData.getSize();
    int replacements = 0;

    for (size_t i = 0; i < dataSize && (maxOccurrences == -1 || replacements < maxOccurrences); ++i) {
        if (i + searchSize > dataSize) {
            break;
        }
        if (memcmp(rawData + i, combinedSearchData.getData(), searchSize) == 0) {
            targetData.copyFrom(modifiedReplaceData.getData(), i, searchSize);
            rawData = static_cast<const uint8*>(targetData.getData());
            replacements++;
            std::cout << "Replacement number " << replacements << " at index " << i << ". Searched text: " << CtrlrMac::hexStringToText(combinedSearchData) << ", Replacing text: " << CtrlrMac::hexStringToText(modifiedReplaceData) << std::endl;
            logger.log(juce::String("Replacement number ") + juce::String(replacements) + juce::String(" at index ") + juce::String(i) + ". Searched text: " + CtrlrMac::hexStringToText(combinedSearchData) + ", Replacing text: " + CtrlrMac::hexStringToText(modifiedReplaceData));
        }
    }

    if (std::search(rawData, rawData + dataSize, combinedSearchData.begin(), combinedSearchData.end()) == rawData + dataSize) {
        std::cout << "Search data not found for replacement. The Process could have reached the end of the file. Searched text: " << CtrlrMac::hexStringToText(combinedSearchData) << ", Replacing text: " << CtrlrMac::hexStringToText(modifiedReplaceData) << std::endl;
        logger.log(juce::String("Search data not found for replacement. The Process could have reached the end of the file. Search text: ") + CtrlrMac::hexStringToText(combinedSearchData) + ", Replacing text: " + CtrlrMac::hexStringToText(modifiedReplaceData));
    }
}


const Result CtrlrMac::getDefaultPanel(MemoryBlock &dataToWrite) {

    File me = File::getSpecialLocation(File::currentApplicationFile).getChildFile("Contents/Resources/"+String(CTRLR_MAC_PANEL_FILE));
    File meBF = File::getSpecialLocation(File::currentApplicationFile).getChildFile("Contents/Resources/"+String(CTRLR_MAC_PANEL_FILE)+String("BF"));
    
    _DBG("MAC native, loading panel BF from file: \""+me.getFullPathName()+"\"");
    
    // Try loading the encrypted file first
    if (meBF.existsAsFile()) {
        // Decrypt file "meBF" (panelZBF)
        MemoryBlock encryptedData;
        if (!meBF.loadFileAsData(encryptedData)) {
            return Result::fail("Error reading encrypted file \"" + meBF.getFullPathName() + "\"");
        }
        
        // Define the BlowFish encryption key as string
        String keyString = "Fw2rXbRh74Vxe53BfapNvGjz8kTDLYmU"; // Replace with your actual key (security!). Updated v5.6.32
        
        // Check if a blowfish key is provided
        if (keyString.isEmpty()) {
            return Result::fail("Blowfish key not provided for decryption");
        } else {
            // Key is provided, proceed with encryption
            BlowFish blowfish(keyString.toUTF8(), keyString.getNumBytesAsUTF8());
            blowfish.decrypt(encryptedData.getData(), encryptedData.getSize());
        }
        
        // Decrypted data is already in 'data', append to output. Added v5.6.31
        dataToWrite.append(encryptedData.getData(), encryptedData.getSize());
        
        return Result::ok();
        
    } else {
        // Fallback to panelZ if panelZBF doesn't exist
        _DBG("MAC native, \"" + meBF.getFullPathName() + "\" does not exist, falling back to \"" + me.getFullPathName() + "\"");
        
        // Read "me" (panelZ) file contents directly
        if (me.existsAsFile()){
            // File "me" (panelZ) loaded successfully, treat it as plain data
            me.loadFileAsData (dataToWrite);
            return (Result::ok());
        } else {
            // Error loading "me" (panelZ) file
            return Result::fail("MAC native, failed to load panel from \"" + me.getFullPathName() + "\"");
        }
    }
}


const Result CtrlrMac::getDefaultResources(MemoryBlock& dataToWrite)
{
#ifdef DEBUG_INSTANCE
	File meRes = File("/Users/atom/devel/debug.z");
#else
	File meRes = File::getSpecialLocation(File::currentApplicationFile).getChildFile("Contents/Resources/"+String(CTRLR_MAC_RESOURCES_FILE));
#endif
	_DBG("MAC native, loading resuources from: \""+meRes.getFullPathName()+"\"");

	if (meRes.existsAsFile())
	{
		meRes.loadFileAsData (dataToWrite);
		return (Result::ok());
	}

	return (Result::fail("MAC native, \""+meRes.getFullPathName()+"\" does not exist"));
}

const Result CtrlrMac::setBundleInfo(CtrlrPanel *sourceInfo, const File &bundle)
{
	File plist = bundle.getChildFile("Contents/Info.plist");

	if (plist.existsAsFile() && plist.hasWriteAccess())
	{
		std::unique_ptr <XmlElement> plistXml (XmlDocument::parse(plist));
		if (plistXml == nullptr)
		{
			return (Result::fail("MAC native, can't parse Info.plist as a XML document"));
		}

		XmlElement *dict = plistXml->getChildByName("dict");
        XmlElement *cfInsertKeyManufacturerID = dict->createNewChildElement("key");
        cfInsertKeyManufacturerID->addTextElement("ManufacturerID");
        XmlElement *cfInsertStringManufacturerID = dict->createNewChildElement("string");
        cfInsertStringManufacturerID->addTextElement("ManufacturerID");
        
		if (dict != nullptr)
		{
			forEachXmlChildElement (*dict, e1)
			{
				if (e1->hasTagName("key") && e1->getAllSubText() == "CFBundleName")
				{
					XmlElement *cfBundleElement = e1->getNextElementWithTagName("string");
					if (cfBundleElement != nullptr)
					{
						cfBundleElement->deleteAllTextElements();
						cfBundleElement->addTextElement(sourceInfo->getProperty(Ids::name).toString());
					}
				}
				if (e1->hasTagName("key") && (e1->getAllSubText() == "CFBundleShortVersionString" || e1->getAllSubText() == "CFBundleVersion"))
				{
					XmlElement *cfVersionElement = e1->getNextElementWithTagName("string");
					if (cfVersionElement != nullptr)
					{
						cfVersionElement->deleteAllTextElements();
						cfVersionElement->addTextElement(sourceInfo->getVersionString(false,false,"."));
					}
				}
                if (e1->hasTagName("key") && (e1->getAllSubText() == "CFBundleSignature"))
                {
                    XmlElement *cfVersionElement = e1->getNextElementWithTagName("string");
                    if (cfVersionElement != nullptr)
                    {
                        cfVersionElement->deleteAllTextElements();
                        cfVersionElement->addTextElement(sourceInfo->getProperty(Ids::panelInstanceUID).toString());
                    }
                }
                if (e1->hasTagName("key") && (e1->getAllSubText() == "NSHumanReadableCopyright"))
				{
					XmlElement *nsCopyright = e1->getNextElementWithTagName("string");
					if (nsCopyright != nullptr)
					{
						nsCopyright->deleteAllTextElements();
						nsCopyright->addTextElement(sourceInfo->getProperty(Ids::panelAuthorName).toString());
					}
                }
                if (e1->hasTagName("key") && (e1->getAllSubText() == "ManufacturerID"))
                {
                    XmlElement *cfManufacturerID = e1->getNextElementWithTagName("string");
                    if (cfManufacturerID != nullptr)
                    {
                        cfManufacturerID->deleteAllTextElements();
                        cfManufacturerID->addTextElement(sourceInfo->getProperty(Ids::panelInstanceManufacturerID).toString());
                    }
                }
                if (e1->hasTagName ("key") && (e1->getAllSubText() == "AudioComponents"))
                {
                    _DBG("INSTANCE: AudioComponents found");

                    /* resource section */
                    XmlElement *dict = nullptr;
                    XmlElement *array = e1->getNextElement();
                    if (array)
                    {
                        _DBG("INSTANCE: array is valid");
                        dict = array->getChildByName("dict");
                    }

                    if (dict != nullptr)
                    {
                        _DBG("INSTANCE: dict is valid");

                        forEachXmlChildElement (*dict, e2)
                        {
                            _DBG("INSTANCE: enum element: "+e2->getTagName());
                            _DBG("INSTANCE: enum subtext: "+e2->getAllSubText());
                            if (e2->hasTagName("key") && (e2->getAllSubText() == "description"))
                            {
                                _DBG("\tmodify");
                                XmlElement *description = e2->getNextElementWithTagName("string");
                                if (description != nullptr)
                                {
                                    description->deleteAllTextElements();
                                    description->addTextElement (sourceInfo->getProperty(Ids::name).toString());
                                }
                            }

                            if (e2->hasTagName("key") && (e2->getAllSubText() == "manufacturer"))
                            {
                                _DBG("\tmodify");
                                XmlElement *manufacturer = e2->getNextElementWithTagName("string");
                                if (manufacturer != nullptr)
                                {
                                    manufacturer->deleteAllTextElements();
                                    manufacturer->addTextElement(sourceInfo->getProperty(Ids::panelInstanceManufacturerID).toString().isEmpty() ? generateRandomUniquePluginId() : sourceInfo->getProperty(Ids::panelInstanceManufacturerID).toString());
                                }
                            }

                            if (e2->hasTagName("key") && (e2->getAllSubText() == "name"))
                            {
                                XmlElement *name = e2->getNextElementWithTagName("string");
                                if (name != nullptr)
                                {
                                    name->deleteAllTextElements();
                                    name->addTextElement(sourceInfo->getProperty(Ids::panelAuthorName).toString() + ": " + sourceInfo->getProperty(Ids::name).toString());
                                }
                            }

                            if (e2->hasTagName("key") && (e2->getAllSubText() == "subtype"))
                            {
                                _DBG("\tmodify");
                                XmlElement *subtype = e2->getNextElementWithTagName("string");
                                if (subtype != nullptr)
                                {
                                    subtype->deleteAllTextElements();
                                    subtype->addTextElement(sourceInfo->getProperty(Ids::panelInstanceUID).toString().isEmpty() ? generateRandomUniquePluginId() : sourceInfo->getProperty(Ids::panelInstanceUID).toString());
                                }
                            }

                            if (e2->hasTagName("key") && (e2->getAllSubText() == "version"))
                            {
                                _DBG("\tmodify");
                                XmlElement *version = e2->getNextElementWithTagName("integer");
                                if (version != nullptr)
                                {
                                    version->deleteAllTextElements();
                                    version->addTextElement (_STR(getVersionAsHexInteger (sourceInfo->getVersionString(false,false,"."))));
                                }
                            }
                        }
                    }
                }
			}
		}
		else
		{
			return (Result::fail("MAC native, Info.plist does not contain <dict /> element"));
		}
		plist.replaceWithText(plistXml->createDocument("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"));
        
		return (Result::ok());
	}
	else
	{
		 return (Result::fail("MAC native, Info.plist does not exist or is not writable: \""+plist.getFullPathName()+"\""));
    }

    return (Result::ok());
}

const Result CtrlrMac::setBundleInfoCarbon (CtrlrPanel *sourceInfo, const File &bundle)
{
#ifdef JUCE_DEBUG
	File rsrcFile = bundle.getChildFile ("Contents/Resources/Ctrlr-Debug.rsrc");
#else
	File rsrcFile = bundle.getChildFile ("Contents/Resources/Ctrlr.rsrc");
#endif

    const String instanceName           = sourceInfo->getPanelInstanceName();
    const String instanceManufacturer   = sourceInfo->getPanelInstanceManufacturer();
    const String instanceID             = sourceInfo->getPanelInstanceID();
    const String instanceManufacturerID = sourceInfo->getPanelInstanceManufacturerID();

    String nameToWrite			= String (instanceManufacturer + ": " + instanceName).substring (0,127);
    const String idToWrite      = instanceID+instanceManufacturerID;
	const int nameLength		= nameToWrite.length();
	String zipFileEntryName		= "result_"+_STR(nameLength)+".rsrc";

    if (idToWrite.length() != 8)
    {
        return (Result::fail("MAC native, id to write for Carbon information is not 8 characters \""+idToWrite+"\""));
    }

	MemoryInputStream zipStream (BinaryData::RSRC_zip, BinaryData::RSRC_zipSize, false);
    ZipFile zipFile (zipStream);
	const ZipFile::ZipEntry *zipFileEntry = zipFile.getEntry(zipFileEntryName);

	_DBG("INSTANCE: trying to use zip file entry with name: "+zipFileEntryName);

	if (zipFileEntry)
	{
		_DBG("\tgot it");
		ScopedPointer<InputStream> is (zipFile.createStreamForEntry(*zipFileEntry));

		if (is)
		{
			MemoryBlock data;
			is->readIntoMemoryBlock (data, is->getTotalLength());

			if (data.getSize() > 0)
			{
				/* name data start 261 */
				data.removeSection (261, 3 + nameLength);

				/* id data startx 579 - after the name is removed*/
				data.removeSection (299, 8);
				data.insert (idToWrite.toUTF8().getAddress(), 8, 299);

				data.insert (nameToWrite.toUTF8().getAddress(), 3+nameLength, 261);

				if (rsrcFile.hasWriteAccess())
				{
					if (rsrcFile.replaceWithData (data.getData(), data.getSize()))
					{
						return (Result::ok());
					}
					else
					{
						return (Result::fail ("MAC native, can't replace rsrc contents with custom data: "+rsrcFile.getFullPathName()));
					}
				}
				else
				{
					Result::fail ("MAC native, can't write to bundle's rsrc file at: "+rsrcFile.getFullPathName());
				}
			}
		}
	}
	else
	{
		return (Result::fail("MAC native, can't find a resource file with name: \""+zipFileEntryName+"\""));
	}

	return (Result::fail("MAC reached end of setBundleInfoCarbon() without any usable result"));
}
#endif
