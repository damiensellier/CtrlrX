#include "stdafx.h"
#ifdef JUCE_WINDOWS
#define NOMINMAX // Added v5.6.32
#include "CtrlrLog.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrWindows.h"
#include <psapi.h>

#include <fstream> // Added v5.6.32
#include <algorithm> // Added v5.6.32
#include <vector> // Added v5.6.32
#include <random> // Added v5.6.32

CtrlrWindows::CtrlrWindows(CtrlrManager &_owner) : owner(_owner)
{
}

CtrlrWindows::~CtrlrWindows()
{
}

const Result CtrlrWindows::writeResource (void *handle, const LPSTR resourceId, const LPSTR resourceType, const MemoryBlock &resourceData)
{
	HANDLE	hResource = (HANDLE)handle;

	if (hResource)
	{
		return (UpdateResource (hResource, resourceType, resourceId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPVOID) resourceData.getData(), (DWORD)resourceData.getSize()) ? Result::ok() : Result::fail("WIN32 UpdateResource failed"));
	}
	else
	{
		return (Result::fail("Windows Native: UpdateResource, resource HANDLE cast failed"));
	}
}

const Result CtrlrWindows::readResource (void *handle, const LPSTR resourceId, const LPSTR resourceType, MemoryBlock &resourceData)
{
	HRSRC	panelResource;
	HGLOBAL panelLoadedResource;
	String  data;
	char	*dataPointer;
	DWORD	dataSize;
	HMODULE myModuleHandle;

	if (handle != nullptr)
	{
		myModuleHandle = (HMODULE)handle;
	}
	else
	{
		myModuleHandle = GetModuleHandle(File::getSpecialLocation(File::currentExecutableFile).getFullPathName().toUTF8());
	}

	if (myModuleHandle)
	{
		panelResource = FindResource(myModuleHandle, resourceId, resourceType);

		if (panelResource)
		{
			panelLoadedResource = LoadResource(myModuleHandle, panelResource);

			if (panelLoadedResource)
			{
				dataSize	= SizeofResource (myModuleHandle, panelResource);
				dataPointer = (char *) LockResource (panelLoadedResource);

				resourceData = MemoryBlock(dataPointer,dataSize);

				return (Result::ok());
			}
			else
			{
				return (Result::fail("Windows Native: LoadResource() failed"));
			}
		}
		else
		{
			return (Result::fail("Windows Native: FindResource() failed"));
		}
	}
	else
	{
		return (Result::fail("Windows Native: GetModuleHandle() for: \""+File::getSpecialLocation(File::currentExecutableFile).getFullPathName()+"\" failed"));
	}
}

const Result CtrlrWindows::exportWithDefaultPanel(CtrlrPanel*  panelToWrite, const bool isRestricted, const bool signPanel)
{
	if (panelToWrite == nullptr)
	{
		return (Result::fail("Windows Native: exportWithDefaultPanel got nullptr for panel"));
	}

	File	me = File::getSpecialLocation(File::currentExecutableFile);
	File	newMe;
	HANDLE	hResource;
	MemoryBlock panelExportData,panelResourcesData;
	PluginLogger logger(me); // Create logger instance
	String error;
	logger.log("Starting exportWithDefaultPanel");
	String fileExtension = me.getFileExtension();
	logger.log("CtrlrX source fileExtension is :" + fileExtension);
	//MemoryBlock iconData(BinaryData::ico_midi_png, BinaryData::ico_midi_pngSize);

	if (panelToWrite == nullptr)
	{
		logger.log("Error: panelToWrite is nullptr");
		return (Result::fail("Windows Native: exportWithDefaultPanel got nullptr for panel"));
	}

	FileChooser exportFc(CTRLR_NEW_INSTANCE_DIALOG_TITLE,
		me.getParentDirectory().getChildFile(File::createLegalFileName(panelToWrite->getProperty(Ids::name))).withFileExtension(me.getFileExtension()),
		"*" + me.getFileExtension(),
		panelToWrite->getOwner().getProperty(Ids::ctrlrNativeFileDialogs));

	if (exportFc.browseForFileToSave(true))
	{
		newMe = exportFc.getResult();
		logger.log("File selected: " + newMe.getFullPathName());

		if (!newMe.hasFileExtension(me.getFileExtension()))
		{
			newMe = newMe.withFileExtension(me.getFileExtension());
		}

		if (!me.copyFileTo(newMe))
		{
			logger.log("Error: Failed to copy executable");
			return (Result::fail("Windows Native: exportMeWithNewResource can't copy \"" + me.getFullPathName() + "\" to \"" + newMe.getFullPathName() + "\""));
		}
		logger.log("Executable copied successfully.");
	}
	else
	{
		logger.log("Error: File selection dialog failed");
		return (Result::fail("Windows Native: exportMeWithNewResource \"Save file\" dialog failed"));
	}

	// Export panel data and resources
	hResource = BeginUpdateResource(newMe.getFullPathName().toUTF8(), FALSE);
	if (hResource)
	{
		if ((error = CtrlrPanel::exportPanel(panelToWrite, File(), newMe, &panelExportData, &panelResourcesData, isRestricted)) == "")
		{
			// Encrypt panel data and resources using JUCE BlowFish directly on copies
			//String keyString = "yourkey"; // Replace with your actual key (security!).
			//juce::BlowFish blowfish(keyString.toUTF8(), keyString.getNumBytesAsUTF8());
			//
			//if (panelExportData.getSize() > 0)
			//{
			//	MemoryBlock encryptedPanelData = panelExportData;
			//	blowfish.encrypt(encryptedPanelData);
			//	panelExportData = encryptedPanelData;
			//	logger.log("Panel data encrypted.");
			//}
			//else
			//{
			//	logger.log("Error: panelExportData is empty");
			//	return Result::fail("Error: panelExportData is empty");
			//}
			//
			//if (panelResourcesData.getSize() > 0)
			//{
			//	MemoryBlock encryptedResourcesData = panelResourcesData;
			//	blowfish.encrypt(encryptedResourcesData);
			//	panelResourcesData = encryptedResourcesData;
			//	logger.log("Panel resources encrypted.");
			//}
			//else
			//{
			//	logger.log("Error: panelResourcesData is empty");
			//	return Result::fail("Error: panelResourcesData is empty");
			//}

			if (writeResource(hResource, MAKEINTRESOURCE(CTRLR_INTERNAL_PANEL_RESID), RT_RCDATA, panelExportData)
				&& writeResource(hResource, MAKEINTRESOURCE(CTRLR_INTERNAL_RESOURCES_RESID), RT_RCDATA, panelResourcesData))
			{
				EndUpdateResource(hResource, FALSE);
			}
			else
			{
				return (Result::fail("Windows Native: exportMeWithNewResource writeResource[panel] failed"));
			}
		}
		else
		{
			return (Result::fail("Windows Native: exportMeWithNewResource exportPanel error: \"" + error + "\""));
		}

		// Maybe place the closing curlies and else statement at the end of the function
		// return (Result::ok());
	}
	else
	{
		return (Result::fail("Windows Native: exportMeWithNewResource BeginUpdateResource failed"));
	} // End if (hResource)



	// Introduce a delay before modifying the executable
	logger.log("Thread sleep to delay binary modification task.");
	juce::Thread::sleep(500); // milliseconds (250ms should be ok, adjust as needed)
	logger.log("Thread restarted for binary modification task.");

	// Now, modify executable (string replacement)
	File executableFile = newMe; // Assuming the newMe file is the executable
	if (executableFile.existsAsFile()) {

		if (fileExtension == ".vst3" || fileExtension == ".dll") { // Updated v5.6.33. Added .vst to identify vst2 instances in Cubase for macOS.(fileExtension == ".vst3" || ".vst") was wrong. FIXED on 2025.04.29
			logger.log("fileExtension is : " + fileExtension);

			// Replace the stock VST3 plugin identifiers with the panel to export ones.
			const bool replaceVst3PluginIds = panelToWrite->getProperty(Ids::panelReplaceVst3PluginIds);

			if (replaceVst3PluginIds) {
				logger.log("Replace the VST3 plugin identifiers with the panel ones : " + replaceVst3PluginIds);

				MemoryBlock executableData;
				if (executableFile.loadFileAsData(executableData))
				{
					logger.log("Executable loaded into memory for modification.");

					String pluginName = panelToWrite->getProperty(Ids::name).toString();
					String pluginCode = panelToWrite->getProperty(Ids::panelInstanceUID).toString();
					String manufacturerName = panelToWrite->getProperty(Ids::panelAuthorName).toString();
					String manufacturerCode = panelToWrite->getProperty(Ids::panelInstanceManufacturerID).toString();
					String versionMajor = panelToWrite->getProperty(Ids::panelVersionMajor).toString();
					String versionMinor = panelToWrite->getProperty(Ids::panelVersionMinor).toString();
					String plugType = panelToWrite->getProperty(Ids::panelPlugType).toString();

					logger.log("Plugin name: " + pluginName);
					logger.log("Plugin code: " + pluginCode);
					logger.log("Manufacturer name: " + manufacturerName);
					logger.log("Manufacturer code: " + manufacturerCode);
					logger.log("Version major: " + versionMajor);
					logger.log("Version minor: " + versionMinor);
					logger.log("Plug type: " + plugType);

					MemoryBlock pluginNameHex, pluginCodeHex, manufacturerNameHex, manufacturerCodeHex, versionMajorHex, versionMinorHex, plugTypeHex;

					hexStringToBytes(pluginName, 32, pluginNameHex);
					hexStringToBytes(pluginCode, 4, pluginCodeHex);
					hexStringToBytes(manufacturerName, 16, manufacturerNameHex);
					hexStringToBytes(manufacturerCode, 4, manufacturerCodeHex);
					hexStringToBytes(versionMajor, 2, versionMajorHex);
					hexStringToBytes(versionMinor, 2, versionMinorHex);
					hexStringToBytes(plugType, 16, plugTypeHex);

					MemoryBlock searchPluginNameHex, searchPluginCodeHex, searchManufacturerNameHex, searchManufacturerCodeHex, searchPlugTypeHex;

					// Replace CtrlrX plugin name "CtrlrX          "
					hexStringToBytes("43 74 72 6C 72 58 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20", searchPluginNameHex);
					// Replace CtrlrX plugin manufacturer code "cTrX"
					hexStringToBytes("63 54 72 58", searchPluginCodeHex);
					// Replace "CtrlrX Project  "
					hexStringToBytes("43 74 72 6C 72 58 20 50 72 6F 6A 65 63 74 20 20", searchManufacturerNameHex);
					// Replace CtrlrX plugin code "cTrl"
					hexStringToBytes("63 54 72 6C", searchManufacturerCodeHex);
					
					// Replace plugType "Instrument|Tools"
					hexStringToBytes("49 6E 73 74 72 75 6D 65 6E 74 7C 54 6F 6F 6C 73", searchPlugTypeHex);
					
					// Replace plugType "Instrument|Synth"
					// hexStringToBytes("49 6E 73 74 72 75 6D 65 6E 74 7C 53 79 6E 74 68", searchPlugTypeHex);

					logger.log("Starting string replacement process...");

					replaceAllOccurrences(executableData, searchPluginNameHex, pluginNameHex);
					replaceAllOccurrences(executableData, searchPluginCodeHex, pluginCodeHex);
					replaceAllOccurrences(executableData, searchManufacturerNameHex, manufacturerNameHex);
					replaceAllOccurrences(executableData, searchManufacturerCodeHex, manufacturerCodeHex);
					replaceAllOccurrences(executableData, searchPlugTypeHex, plugTypeHex);

					logger.log("String replacement process completed.");

					if (!executableFile.replaceWithData(executableData.getData(), executableData.getSize()))
					{
						logger.log("Error: Failed to write modified executable data");
						return (Result::fail("Windows Native: Failed to write modified executable data"));
					}
					logger.log("Modified executable data written.");
				}
				else
				{
					logger.log("Error: Failed to load executable into memory.");
					return Result::fail("Windows Native: Failed to load executable into memory.");
				}
			} // End if replaceVst3PluginIds
			else {
				logger.log("replaceVst3PluginIds set to false, Vst3 IDs replacement skipped.");
			}
		}
		else {
			logger.log("Exported file is not vst3.");
		} // End if file extension vst3
		

		// Introduce a delay before codesigning
		logger.log("Thread sleep to delay codesigning task.");
		juce::Thread::sleep(500); // milliseconds (250ms should be ok, adjust as needed)
		logger.log("Thread restarted for codesigning task.");


		// Now, codesign the newMe file and return result. Valid for .exe, vst & vst3
		juce::String panelCertificateWinPath = panelToWrite->getProperty(Ids::panelCertificateWinPath).toString();
		juce::String panelCertificateWinPassCode = panelToWrite->getProperty(Ids::panelCertificateWinPassCode).toString();
		logger.log("Windows Certificate Path: " + panelCertificateWinPath);
		logger.log("Windows Certificate PassCode: " + panelCertificateWinPassCode);
			
		if (panelCertificateWinPath.isNotEmpty() && juce::File::isAbsolutePath(panelCertificateWinPath) && juce::File(panelCertificateWinPath).existsAsFile() && panelCertificateWinPassCode.isNotEmpty())
		{
			const Result codesignResult = codesignFileWindows(newMe, panelCertificateWinPath, panelCertificateWinPassCode); // Call codesignFileWindows function
			if (!codesignResult.wasOk()) {
				logger.logResult(codesignResult);
				return (codesignResult);
			}
			logger.log("Codesigning successful.");
			logger.logResult(codesignResult); // Added Logger for successful codesign.
			}
		else {
			logger.log("Codesigning failed because either CertificatePath or CertificatePassCode were wrong.");
			// return (Result::fail("Windows Native: Codesigning failed because either CertificatePath or CertificatePassCode were wrong."));
			return (Result::ok()); // bypass the fail notification but export won't be codesigned
		} // End if CertificatePath & PassCode OK
	}
	else
	{
		logger.log("Error: Executable file does not exist.");
		return (Result::fail("Windows Native: Executable file does not exist"));
	} // End if file does not exist
	
    return (Result::ok()); // Should be removed v5.6.32 ? all other elements already return ok() or fail() anyway.

} // end result() overall function


// Codesign the exported binary
const Result CtrlrWindows::codesignFileWindows(const File& fileToSign, const String& certificatePath, const String& certificatePassword)
{
	StringArray commandParts;
	commandParts.add("signtool");
	commandParts.add("sign");
	commandParts.add("/f");
	commandParts.add(certificatePath);
	commandParts.add("/p");
	commandParts.add(certificatePassword);
	commandParts.add("/t");
	commandParts.add("http://timestamp.digicert.com"); // Timestamp server
	commandParts.add(fileToSign.getFullPathName());
	
	juce::ChildProcess childProcess;
	if (childProcess.start(commandParts)) {
		childProcess.waitForProcessToFinish(-1);

		if (!childProcess.isRunning()) { // Check if process has finished
			if (childProcess.getExitCode() == 0) {
				return juce::Result::ok(); // Codesign successful

			}
			else {
				return juce::Result::fail("Codesign failed with exit code: " + juce::String(childProcess.getExitCode())); // Codesign failed
			}
		}
		else {
			return juce::Result::fail("Codesign process did not finish properly."); // Process still running
		}

	}
	else {
		return juce::Result::fail("Failed to start codesign process."); // Failed to start process
	}
}


// Convert hex string to binary data
void CtrlrWindows::hexStringToBytes(const String& hexString, MemoryBlock& result) {
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
void CtrlrWindows::hexStringToBytes(const juce::String& hexString, int maxLength, juce::MemoryBlock& result) {
	result.setSize(0); // Clear the MemoryBlock before use
	juce::String sanitizedHexString = hexString.removeCharacters("\t\r\n"); //use juce::String
	std::vector<uint8> bytes;

	for (int i = 0; i < std::min((int)sanitizedHexString.length(), maxLength); ++i) { // Use std::min
		bytes.push_back(static_cast<uint8>(sanitizedHexString[i]));
	}

	while (bytes.size() < maxLength) {
		bytes.push_back(0); // Pad with zeros
	}

	result.append(bytes.data(), bytes.size());
}


// Convert binary data to hex string
String CtrlrWindows::bytesToHexString(const juce::MemoryBlock& memoryBlock) {
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (size_t i = 0; i < memoryBlock.getSize(); ++i) {
		ss << std::setw(2) << static_cast<int>(static_cast<const uint8*>(memoryBlock.getData())[i]);
	}
	return juce::String(ss.str());
}


// Replace all occurrences of searchData with replaceData in the targetData
void CtrlrWindows::replaceAllOccurrences(MemoryBlock& targetData, const MemoryBlock& searchData, const MemoryBlock& replaceData)
{
	if (searchData.getSize() != replaceData.getSize() || searchData.getSize() == 0)
	{
		DBG("Invalid search/replace data sizes");
		return;
	}

	const uint8* rawData = static_cast<const uint8*>(targetData.getData());
	size_t dataSize = targetData.getSize();
	size_t searchSize = searchData.getSize();

	for (size_t i = 0; i <= dataSize - searchSize; ++i)
	{
		if (memcmp(rawData + i, searchData.getData(), searchSize) == 0)
		{
			// Replace the data
			targetData.copyFrom(replaceData.getData(), i, replaceData.getSize());
			// Update rawData pointer as the memory might have been reallocated
			rawData = static_cast<const uint8*>(targetData.getData());
		}
	}
}


// Replace only the first N occurrences of searchData with replaceData in the targetData
void CtrlrWindows::replaceFirstNOccurrences(MemoryBlock& targetData, const MemoryBlock& searchData, const MemoryBlock& replaceData, int maxOccurrences)
{
	if (searchData.getSize() != replaceData.getSize() || searchData.getSize() == 0)
	{
		DBG("Invalid search/replace data sizes");
		return;
	}

	const uint8* rawData = static_cast<const uint8*>(targetData.getData());
	size_t dataSize = targetData.getSize();
	size_t searchSize = searchData.getSize();
	int occurrencesFound = 0;

	for (size_t i = 0; i <= dataSize - searchSize && occurrencesFound < maxOccurrences; ++i)
	{
		if (memcmp(rawData + i, searchData.getData(), searchSize) == 0)
		{
			// Replace the data
			targetData.copyFrom(replaceData.getData(), i, replaceData.getSize());
			// Update rawData pointer as the memory might have been reallocated
			rawData = static_cast<const uint8*>(targetData.getData());
			occurrencesFound++;
		}
	}
}


const Result CtrlrWindows::getDefaultPanel(MemoryBlock &dataToWrite)
{
#ifdef DEBUG_INSTANCE
	File temp("c:\\devel\\debug_small.bpanelz");
	temp.loadFileAsData (dataToWrite);
	return (Result::ok());
#endif

	return (readResource (nullptr, MAKEINTRESOURCE(CTRLR_INTERNAL_PANEL_RESID), RT_RCDATA, dataToWrite));
}

const Result CtrlrWindows::getDefaultResources(MemoryBlock& dataToWrite)
{
#ifdef DEBUG_INSTANCE
	File temp("c:\\devel\\debug_small.bpanelz");

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
		return (Result::fail("Windows Native: getDefaultResources got data but couldn't parse it as a compressed ValueTree"));
	}
#endif

	return (readResource (nullptr, MAKEINTRESOURCE(CTRLR_INTERNAL_RESOURCES_RESID), RT_RCDATA, dataToWrite));
}

const Result CtrlrWindows::getSignature(MemoryBlock &dataToWrite)
{
	return (readResource (nullptr, MAKEINTRESOURCE(CTRLR_INTERNAL_SIGNATURE_RESID), RT_RCDATA, dataToWrite));
}

const Result CtrlrWindows::registerFileHandler()
{
	if (!JUCEApplication::isStandaloneApp())
		return (Result::ok());

	const char* const exts[] = { ".panel", ".panelz", ".bpanel", ".bpanelz", nullptr };
	StringArray extensions(exts);

	for (int i=0; i<extensions.size(); i++)
	{
		if (!WindowsRegistry::registerFileAssociation (extensions[i], "ctrlr"+extensions[i], "Ctrlr panel file ("+extensions[i]+")", File::getSpecialLocation(File::currentApplicationFile), -1, true))
			return (Result::fail("Can't register ["+extensions[i]+"] file extension"));
	}

	return (Result::ok());
}

static void sendKey(const KeyPress &event)
{
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wScan = 0;
	input.ki.dwFlags = 0;

	// Modifier Down
	if (event.getModifiers().isCommandDown())
	{
		input.ki.wVk = VK_CONTROL;
		SendInput(1, &input, sizeof(INPUT));
	}
	if (event.getModifiers().isAltDown())
	{
		input.ki.wVk = VK_MENU;
		SendInput(1, &input, sizeof(INPUT));
	}
	if (event.getModifiers().isShiftDown())
	{
		input.ki.wVk = VK_SHIFT;
		SendInput(1, &input, sizeof(INPUT));
	}

	// KEY Down
	input.ki.wVk = event.getKeyCode();
	SendInput(1, &input, sizeof(INPUT));

	// KEY Up
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));

	// MODIFIER Up
	if (event.getModifiers().isCommandDown())
	{
		input.ki.wVk = VK_CONTROL;
		SendInput(1, &input, sizeof(INPUT));
	}
	if (event.getModifiers().isAltDown())
	{
		input.ki.wVk = VK_MENU;
		SendInput(1, &input, sizeof(INPUT));
	}
	if (event.getModifiers().isShiftDown())
	{
		input.ki.wVk = VK_SHIFT;
		SendInput(1, &input, sizeof(INPUT));
	}
}

const Result CtrlrWindows::sendKeyPressEvent (const KeyPress &event)
{
	return (sendKeyPressEvent(event, ""));
}

const Result CtrlrWindows::sendKeyPressEvent (const KeyPress &event, const String &targetWindowName)
{
	HWND firstwindow = FindWindowEx(NULL, NULL, NULL, NULL);
    HWND window = firstwindow;
    TCHAR windowtext[MAX_PATH];
	// INPUT input;

	if (targetWindowName != "")
	{
		while(1)
		{

			GetWindowText(window, windowtext, MAX_PATH);
			if (strstr(windowtext, targetWindowName.getCharPointer()) != NULL)
				break;

			window = FindWindowEx(NULL, window, NULL, NULL);
			if (window == NULL || window == firstwindow)
				return (Result::fail("Can't find target window: " + targetWindowName));
		}
	}

	SetForegroundWindow(window);
	sendKey(event);
	return (Result::ok());
}
#endif
