#include "stdafx.h" // Your precompiled headers
#include "stdafx_luabind.h" // Your precompiled headers for Luabind (which includes JuceHeader.h and all luabind headers)
#include "LOnlineUnlockStatusCheck.h" // Header for this custom class
#include "CtrlrMacros.h" // For _DBG macro
#include "CtrlrLog.h" // For _DBG output

// Make sure you have these includes for XmlElement, File, Streams if not already in stdafx.h
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h> // Potentially for Alerts, etc. (good to have for UI related tasks)
#include <algorithm> // REQUIRED for std::reverse

// --- CORRECTED HELPER FUNCTION AGAIN (Third Time's the Charm!): Get the consistent application name from the executable ---
// This ensures the directory and file names are always based on the actual executable.
static juce::String getConsistentAppName()
{
    // Get the full file name (e.g., "MyPlugin.vst3", "MyApplication.exe", "MyExecutable")
    juce::String executableFileName = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getFileName();
    
    // Use upToLastOccurrenceOf with a StringRef for the dot, and explicitly specify all three arguments.
    // Argument 1: StringRef for the separator (".")
    // Argument 2: includeSubString (false, we don't want to include the dot itself)
    // Argument 3: ignoreCase (false, case doesn't matter for a dot)
    juce::String nameWithoutExtension = executableFileName.upToLastOccurrenceOf(juce::StringRef("."), false, false);

    // If the filename has no dot (e.g., "MyExecutable"), upToLastOccurrenceOf returns the original string.
    // If it's an empty string, it returns an empty string.
    // We want to make sure we don't return an empty string if the executable name was valid but had no extension.
    if (nameWithoutExtension.isEmpty() && !executableFileName.isEmpty())
    {
        // This handles cases like a filename with no extension (e.g., "MyBinary"),
        // or a hidden file like ".bashrc" where upToLastOccurrenceOf might return empty if it treats the leading dot as the "last" dot.
        // For our purpose of getting an app name, the full name is better than empty if no extension is found.
        return executableFileName.trim();
    }
    
    return nameWithoutExtension.trim();
}

// Helper to get a consistent directory path for application data.
static juce::File getApplicationDataDirectory(const juce::String& appName)
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                         .getChildFile(appName);
}

// Get the path for the registration key file
// --- MODIFIED: Get the path for the registration key file on the DESKTOP ---
static juce::File getRegistrationKeyFile(const juce::String& appName)
{
    // Point directly to the Desktop directory. No need to create it as it always exists.
    return juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
               .getChildFile(juce::File::createLegalFileName(appName + "_license.key")); // Use a distinct filename for clarity
}


// Logger init
juce::File debugLogPath = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
PluginLoggerVst3 logger(debugLogPath);

// Constructor Definition
LOnlineUnlockStatusCheck::LOnlineUnlockStatusCheck(const juce::String& productID,
                                                   const juce::String& appNameFromLua, // Renamed to clarify origin
                                                   const juce::URL& serverURL,
                                                   const juce::RSAKey& publicKey)
    : myProductID(productID),
      // --- IMPORTANT CHANGE HERE: Use getConsistentAppName() for myAppName ---
      // This overrides the appName passed from Lua for internal file paths, ensuring consistency.
      myAppName(getConsistentAppName()),
      // --- END IMPORTANT CHANGE ---
      myServerURL(serverURL),
      myPublicKey(publicKey),
      persistentUnlockState("") // Initialize our new member
{
    parsedSuccess = false;
    parsedErrorMessage = "";
    parsedInformativeMessage = "";
    parsedUrlToLaunch = "";
    
    // Ensure the application data directory exists using the consistent app name
    getApplicationDataDirectory(myAppName).createDirectory();
    
    // Call base class load, which will call our getState()
    // This will now attempt to load the key from the file on construction.
    load();
}

// Definition for getProductID() override
juce::String LOnlineUnlockStatusCheck::getProductID()
{
    return myProductID;
}

// Definition for doesProductIDMatch() override
bool LOnlineUnlockStatusCheck::doesProductIDMatch (const juce::String& returnedIDFromServer)
{
    return myProductID == returnedIDFromServer;
}

// Definition for getPublicKey() override
juce::RSAKey LOnlineUnlockStatusCheck::getPublicKey()
{
    return myPublicKey;
}

// --- saveState to write to file ---
void LOnlineUnlockStatusCheck::saveState (const juce::String& newState)
{
    _DBG ("LOnlineUnlockStatusCheck: saveState called. Storing: " + newState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: saveState called. Storing: ") + newState);
        
    persistentUnlockState = newState; // Keep in memory too for immediate use

    // --- Write the new state to the dedicated .key file using myAppName ---
    juce::File keyFile = getRegistrationKeyFile(myAppName);
    juce::FileOutputStream outputStream (keyFile);

    if (outputStream.openedOk())
    {
        if (outputStream.writeString(newState))
        {
            _DBG("Successfully saved registration key to: " + keyFile.getFullPathName());
            logger.log("Successfully saved registration key to: " + keyFile.getFullPathName());
        }
        else
        {
            _DBG("Error writing registration key to file: " + keyFile.getFullPathName());
            logger.log("Error writing registration key to file: " + keyFile.getFullPathName());
        }
    }
    else
    {
        _DBG("Error opening registration key file for writing: " + keyFile.getFullPathName());
        logger.log("Error opening registration key file for writing: " + keyFile.getFullPathName());
    }
}

// --- getState to read from file ---
juce::String LOnlineUnlockStatusCheck::getState()
{
    // If the key is not already loaded into memory, try to load it from the file.
    if (persistentUnlockState.isEmpty())
    {
        // --- Load from the dedicated .key file using myAppName ---
        juce::File keyFile = getRegistrationKeyFile(myAppName);
        if (keyFile.existsAsFile())
        {
            juce::FileInputStream inputStream (keyFile);
            if (inputStream.openedOk())
            {
                persistentUnlockState = inputStream.readEntireStreamAsString().trim(); // Read and trim whitespace
                _DBG("LOnlineUnlockStatusCheck: Loaded registration key from file: " + keyFile.getFullPathName());
                logger.log("LOnlineUnlockStatusCheck: Loaded registration key from file: " + keyFile.getFullPathName());
            }
            else
            {
                _DBG("Error opening registration key file for reading: " + keyFile.getFullPathName());
                logger.log("Error opening registration key file for reading: " + keyFile.getFullPathName());
            }
        }
        else
        {
            _DBG("Registration key file does not exist: " + keyFile.getFullPathName());
            logger.log("Registration key file does not exist: " + keyFile.getFullPathName());
        }
    }

    _DBG ("LOnlineUnlockStatusCheck: getState called. Returning: " + persistentUnlockState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: getState called. Returning: ") + persistentUnlockState);
        
    return persistentUnlockState;
}

// Definition for getWebsiteName() override
juce::String LOnlineUnlockStatusCheck::getWebsiteName()
{
    return "CtrlrX Web Server";
}

// Definition for getServerAuthenticationURL() override
juce::URL LOnlineUnlockStatusCheck::getServerAuthenticationURL()
{
    return myServerURL;
}

juce::StringArray LOnlineUnlockStatusCheck::getLocalMachineIDs()
{
    logger.log("LOnlineUnlockStatusCheck::getLocalMachineIDs() called. Returning default JUCE machine IDs.");
    _DBG("LOnlineUnlockStatusCheck::getLocalMachineIDs() called. Returning default JUCE machine IDs.");
    return juce::OnlineUnlockStatus::MachineIDUtilities::getLocalMachineIDs();
}

juce::String LOnlineUnlockStatusCheck::readReplyFromWebserver (const juce::String& email, const juce::String& password)
{
    // Re-initialize logger? Consider making it a member or global static if it's logging to a fixed file.
    juce::File debugLog = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    PluginLoggerVst3 logger(debugLog);

    juce::URL serverUrl = getServerAuthenticationURL();
    serverUrl = serverUrl.withParameter ("email", email);
    serverUrl = serverUrl.withParameter ("password", password);
    // Optionally add machine IDs to the URL for server-side binding
    serverUrl = serverUrl.withParameter ("machineids", juce::OnlineUnlockStatus::MachineIDUtilities::getLocalMachineIDs().joinIntoString(","));

    logger.log(juce::String("LOnlineUnlockStatusCheck: Attempting web request to: ") + serverUrl.toString(true)); // File log
    _DBG(juce::String("LOnlineUnlockStatusCheck: Attempting web request to server: " + serverUrl.toString(true))); // Console log

    juce::String reply = serverUrl.readEntireTextStream (false);

    logger.log(juce::String("LOnlineUnlockStatusCheck: Raw server reply received: ") + reply); // File log
    _DBG(juce::String("LOnlineUnlockStatusCheck: Raw server reply received (console): ") + reply); // Console log

    if (reply.isEmpty())
    {
        logger.log("LOnlineUnlockStatusCheck: Empty reply from webserver.");
        _DBG("LOnlineUnlockStatusCheck: Empty reply from webserver.");
        parsedSuccess = false;
        parsedErrorMessage = "Empty reply from webserver.";
        return "";
    }

    // --- Start of your custom parsing for display/logging purposes ---
    // The base class will do its own parsing. We do this here for our custom parsedX variables.
    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(reply);

    if (xml != nullptr && xml->getTagName() == "OnlineUnlockStatus")
    {
        parsedSuccess = xml->getBoolAttribute("success", false);
        logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false"));
        _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false"));

        if (juce::XmlElement* messageElement = xml->getChildByName("message"))
        {
            parsedInformativeMessage = messageElement->getAllSubText();
            logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage);
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage);
        }
        else
        {
            parsedInformativeMessage = "";
            logger.log("LOnlineUnlockStatusCheck: Custom parsed informative message: (empty)");
            _DBG("LOnlineUnlockStatusCheck: Custom parsed informative message: (empty)");
        }
        
        if (juce::XmlElement* urlElement = xml->getChildByName("url"))
        {
            parsedUrlToLaunch = urlElement->getAllSubText();
            logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch);
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch);
        }
        else
        {
            parsedUrlToLaunch = "";
            logger.log("LOnlineUnlockStatusCheck: Custom parsed URL to launch: (empty)");
            _DBG("LOnlineUnlockStatusCheck: Custom parsed URL to launch: (empty)");
        }

        // Log the key for debugging, but DO NOT return it directly here.
        // The base class's processReplyFromServer will extract it if success=true
        // and pass it to saveState().
        if (juce::XmlElement* keyElement = xml->getChildByName("key"))
        {
            logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed key (for debug): ") + keyElement->getAllSubText());
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed key (for debug): ") + keyElement->getAllSubText());
        } else {
            logger.log("LOnlineUnlockStatusCheck: Custom parsing: No <key> element found!");
            _DBG("LOnlineUnlockStatusCheck: Custom parsing: No <key> element found!");
        }

        parsedErrorMessage = "";
        logger.log("LOnlineUnlockStatusCheck: Custom parsed error message: (empty)");
        _DBG("LOnlineUnlockStatusCheck: Custom parsed error message: (empty)");

    } else {
        parsedSuccess = false;
        parsedErrorMessage = "Failed to parse XML or invalid root tag. Raw reply: " + reply;
        parsedInformativeMessage = "";
        parsedUrlToLaunch = "";
        logger.log("LOnlineUnlockStatusCheck: Custom parsing failed or root tag mismatch: " + reply);
        _DBG("LOnlineUnlockStatusCheck: Custom parsing failed or root tag mismatch: " + reply);
    }

    logger.log(juce::String("LOnlineUnlockStatusCheck: Returning full raw reply for base class processing: ") + reply);
    _DBG(juce::String("LOnlineUnlockStatusCheck: Returning this string to base class: ") + reply);
    return reply; // IMPORTANT: Return the entire 'reply' string.
}

// isUnlocked() now directly calls the base class's non-virtual isUnlocked()
// This will return true ONLY if the key received from the server and saved via saveState
// is cryptographically validated by myPublicKey.
bool LOnlineUnlockStatusCheck::isUnlocked() const
{
    _DBG("LOnlineUnlockStatusCheck: LOnlineUnlockStatusCheck::isUnlocked() called. Calling base class for real validation.");
    logger.log("LOnlineUnlockStatusCheck: LOnlineUnlockStatusCheck::isUnlocked() called. Calling base class for real validation.");

    // This is the core JUCE validation. It calls getState() and getPublicKey().
    bool unlocked = static_cast<juce::OnlineUnlockStatus*>(const_cast<LOnlineUnlockStatusCheck*>(this))->isUnlocked();
    
    _DBG(juce::String("LOnlineUnlockStatusCheck: isUnlocked() base OnlineUnlockStatus::isUnlocked() returning: ") + (unlocked ? "true" : "false"));
    logger.log(juce::String("LOnlineUnlockStatusCheck: isUnlocked() base OnlineUnlockStatus::isUnlocked() returning: ") + (unlocked ? "true" : "false"));

    return unlocked;
}

// Definition for isPluginUnlocked() method
bool LOnlineUnlockStatusCheck::isPluginUnlocked() const
{
    // This will now correctly reflect the state set by the base class's internal logic
    // after a successful attemptWebserverUnlock, because getState() and saveState() are implemented.
    return isUnlocked();
}

void LOnlineUnlockStatusCheck::wrapForLua (lua_State *L)
{
    luabind::module(L)
    [
        luabind::class_<LOnlineUnlockStatusCheck, juce::OnlineUnlockStatus>("OnlineUnlockStatusCheck")
            .def(luabind::constructor<const juce::String&, const juce::String&, const juce::URL&, const juce::RSAKey&>())
            .def("getProductID", &LOnlineUnlockStatusCheck::getProductID)
            .def("doesProductIDMatch", &LOnlineUnlockStatusCheck::doesProductIDMatch)
            .def("getPublicKey", &LOnlineUnlockStatusCheck::getPublicKey)
            .def("saveState", &LOnlineUnlockStatusCheck::saveState)
            .def("getState", &LOnlineUnlockStatusCheck::getState)
            .def("getWebsiteName", &LOnlineUnlockStatusCheck::getWebsiteName)
            .def("getServerAuthenticationURL", &LOnlineUnlockStatusCheck::getServerAuthenticationURL)
            .def("readReplyFromWebserver", (juce::String (LOnlineUnlockStatusCheck::*)(const juce::String&, const juce::String&)) &LOnlineUnlockStatusCheck::readReplyFromWebserver)
            .def("isPluginUnlocked", &LOnlineUnlockStatusCheck::isPluginUnlocked)
            .def("isUnlocked", &LOnlineUnlockStatusCheck::isUnlocked) // This binds YOUR LOnlineUnlockStatusCheck::isUnlocked() const
            .def("setUserEmail", &LOnlineUnlockStatusCheck::setUserEmail)
            .def("attemptWebserverUnlock", &LOnlineUnlockStatusCheck::attemptWebserverUnlock)

            .def("getParsedSuccess", &LOnlineUnlockStatusCheck::getParsedSuccess)
            .def("getParsedErrorMessage", &LOnlineUnlockStatusCheck::getParsedErrorMessage)
            .def("getParsedInformativeMessage", &LOnlineUnlockStatusCheck::getParsedInformativeMessage)
            .def("getParsedUrlToLaunch", &LOnlineUnlockStatusCheck::getParsedUrlToLaunch)
    ];
}
