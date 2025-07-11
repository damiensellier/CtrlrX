#include "stdafx.h" // Your precompiled headers
#include "stdafx_luabind.h" // Your precompiled headers for Luabind (which includes JuceHeader.h and all luabind headers)
#include "LOnlineUnlockStatusCheck.h" // Header for this custom class
#include "CtrlrMacros.h" // For _DBG macro
#include "CtrlrLog.h" // For _DBG output

// Make sure you have these includes for XmlElement if not already in stdafx.h
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

// Helper to get a consistent directory path for application data.
// We'll still create this, even if the key itself is in-memory for now.
static juce::File getApplicationDataDirectory(const juce::String& appName)
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile(appName);
}

// Logger init
juce::File debugLogPath = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
PluginLoggerVst3 logger(debugLogPath);

// Constructor Definition
LOnlineUnlockStatusCheck::LOnlineUnlockStatusCheck(const juce::String& productID,
                                                   const juce::String& appName,
                                                   const juce::URL& serverURL,
                                                   const juce::RSAKey& publicKey)
    : myProductID(productID),
      myAppName(appName),
      myServerURL(serverURL),
      myPublicKey(publicKey),
      persistentUnlockState("") // Initialize our new member
{
    parsedSuccess = false;
    parsedErrorMessage = "";
    parsedInformativeMessage = "";
    parsedUrlToLaunch = "";
    
    // Ensure the application data directory exists
    getApplicationDataDirectory(appName).createDirectory();
    
    // Call base class load, which will call our getState()
    // If you plan to truly persist the key to disk later,
    // this `load()` call here is where you'd read the saved key from file.
    // For now, it will return an empty string, and base class will be 'unlocked' state.
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

// saveState now just stores to the persistentUnlockState member
void LOnlineUnlockStatusCheck::saveState (const juce::String& newState)
{
    _DBG ("LOnlineUnlockStatusCheck: saveState called. Storing in memory: " + newState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: saveState called. Storing in memory: ") + newState);
        
    persistentUnlockState = newState;
    
    // --- IMPORTANT: This is where you would add file persistence later ---
    // If you want to save the key to disk after it's successfully validated by JUCE:
    // juce::File keyFile = getApplicationDataDirectory(myAppName).getChildFile("unlock_key.txt");
    // juce::FileOutputStream os(keyFile);
    // if (os.openedOk()) os.writeString(newState);
    // --------------------------------------------------------------------
}

// !!! IMPORTANT: getState() now returns the state from our member variable
juce::String LOnlineUnlockStatusCheck::getState()
{
    // --- IMPORTANT: This is where you would add file loading later ---
    // If you want to load the key from disk when the plugin starts:
    // juce::File keyFile = getApplicationDataDirectory(myAppName).getChildFile("unlock_key.txt");
    // if (keyFile.existsAsFile() && persistentUnlockState.isEmpty())
    // {
    //     persistentUnlockState = keyFile.loadFileAsString();
    // }
    // --------------------------------------------------------------------

    _DBG ("LOnlineUnlockStatusCheck: getState called. Returning from memory: " + persistentUnlockState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: getState called. Returning from memory: ") + persistentUnlockState);
        
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
        logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false")); // Console log
        _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false")); // Console log

        if (juce::XmlElement* messageElement = xml->getChildByName("message"))
        {
            parsedInformativeMessage = messageElement->getAllSubText();
            logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage); // Console log
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage); // Console log
        }
        else
        {
            parsedInformativeMessage = "";
            logger.log("LOnlineUnlockStatusCheck: Custom parsed informative message: (empty)"); // Console log
            _DBG("LOnlineUnlockStatusCheck: Custom parsed informative message: (empty)"); // Console log
        }
        
        if (juce::XmlElement* urlElement = xml->getChildByName("url"))
        {
            parsedUrlToLaunch = urlElement->getAllSubText();
            logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch);
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch); // Console log
        }
        else
        {
            parsedUrlToLaunch = "";
            logger.log("LOnlineUnlockStatusCheck: Custom parsed URL to launch: (empty)"); // Console log
            _DBG("LOnlineUnlockStatusCheck: Custom parsed URL to launch: (empty)"); // Console log
        }

        // Log the key for debugging, but DO NOT return it directly.
        if (juce::XmlElement* keyElement = xml->getChildByName("key"))
        {
            logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed key (for debug): ") + keyElement->getAllSubText()); // Console log
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed key (for debug): ") + keyElement->getAllSubText()); // Console log
        } else {
            logger.log("LOnlineUnlockStatusCheck: Custom parsing: No <key> element found!"); // Console log
            _DBG("LOnlineUnlockStatusCheck: Custom parsing: No <key> element found!"); // Console log
        }

        parsedErrorMessage = "";
        logger.log("LOnlineUnlockStatusCheck: Custom parsed error message: (empty)"); // Console log
        _DBG("LOnlineUnlockStatusCheck: Custom parsed error message: (empty)"); // Console log

    } else {
        parsedSuccess = false;
        parsedErrorMessage = "Failed to parse XML or invalid root tag. Raw reply: " + reply;
        parsedInformativeMessage = "";
        parsedUrlToLaunch = "";
        logger.log("LOnlineUnlockStatusCheck: Custom parsing failed or root tag mismatch: " + reply);
        _DBG("LOnlineUnlockStatusCheck: Custom parsing failed or root tag mismatch: " + reply);
    }

    logger.log(juce::String("LOnlineUnlockStatusCheck: Returning full raw reply for base class processing: ") + reply); // File log
    _DBG(juce::String("LOnlineUnlockStatusCheck: Returning this string to base class: ") + reply); // Console log
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
