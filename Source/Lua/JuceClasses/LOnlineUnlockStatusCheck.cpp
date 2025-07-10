#include "stdafx.h" // Your precompiled headers
#include "stdafx_luabind.h" // Your precompiled headers for Luabind (which includes JuceHeader.h and all luabind headers)
#include "LOnlineUnlockStatusCheck.h" // Header for this custom class
#include "CtrlrMacros.h" // For _DBG macro
#include "CtrlrLog.h" // For PluginLoggerVst3

// Make sure you have these includes for XmlElement if not already in stdafx.h
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

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
    
    // IMPORTANT: Call load() here so the base class can initialize its internal state
    // from whatever was previously saved.
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

// !!! IMPORTANT: saveState() now actually stores the state to our member variable
void LOnlineUnlockStatusCheck::saveState (const juce::String& newState)
{
    juce::File debugLogPath = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    PluginLoggerVst3 logger(debugLogPath);
    
    _DBG ("LOnlineUnlockStatusCheck: saveState called with: " + newState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: saveState called with: ") + newState);
    
    persistentUnlockState = newState;
    // In a real application, you would save persistentUnlockState to disk here
    // using juce::PropertiesFile or similar persistent storage.
}

// !!! IMPORTANT: getState() now returns the state from our member variable
juce::String LOnlineUnlockStatusCheck::getState()
{
    juce::File debugLogPath = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    PluginLoggerVst3 logger(debugLogPath);
    
    _DBG ("LOnlineUnlockStatusCheck: getState called. Returning: " + persistentUnlockState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: getState called. Returning: ") + persistentUnlockState);
    
    // In a real application, you would load persistentUnlockState from disk here if it's not already loaded.
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

// MODIFIED: This function should return the raw XML reply for the base class to parse.
juce::String LOnlineUnlockStatusCheck::readReplyFromWebserver (const juce::String& email, const juce::String& password)
{
    juce::File debugLog = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    PluginLoggerVst3 logger(debugLog);

    juce::URL serverUrl = getServerAuthenticationURL();

    serverUrl = serverUrl.withParameter ("email", email);
    serverUrl = serverUrl.withParameter ("password", password);

    logger.log(juce::String("LOnlineUnlockStatusCheck: Attempting web request to: ") + serverUrl.toString(true)); // File log
    _DBG(juce::String("LOnlineUnlockStatusCheck: Attempting web request to server: " + serverUrl.toString(true))); // Console log

    juce::String reply = serverUrl.readEntireTextStream (false);

    logger.log(juce::String("LOnlineUnlockStatusCheck: Raw server reply received: ") + reply); // File log
    _DBG(juce::String("LOnlineUnlockStatusCheck: Raw server reply received (console): ") + reply); // Console log

    if (reply.isEmpty())
    {
        logger.log("LOnlineUnlockStatusCheck: Empty reply from webserver."); // File log
        _DBG("LOnlineUnlockStatusCheck: Empty reply from webserver."); // Console log
        _DBG("LOnlineUnlockStatusCheck: Returning EMPTY string to base class due to empty reply."); // Console log
        return "";
    }

    // --- Start of your custom parsing for display/logging purposes ---
    // The base class will do its own parsing. We do this here for our custom parsedX variables.
    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(reply);

    if (xml != nullptr && xml->getTagName() == "OnlineUnlockStatus")
    {
        parsedSuccess = xml->getBoolAttribute("success", false);
        _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false")); // Console log

        if (juce::XmlElement* messageElement = xml->getChildByName("message"))
        {
            parsedInformativeMessage = messageElement->getAllSubText();
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage); // Console log
        }
        else
        {
            parsedInformativeMessage = "";
            _DBG("LOnlineUnlockStatusCheck: Custom parsed informative message: (empty)"); // Console log
        }
        
        if (juce::XmlElement* urlElement = xml->getChildByName("url"))
        {
            parsedUrlToLaunch = urlElement->getAllSubText();
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch); // Console log
        }
        else
        {
            parsedUrlToLaunch = "";
            _DBG("LOnlineUnlockStatusCheck: Custom parsed URL to launch: (empty)"); // Console log
        }

        // Log the key for debugging, but DO NOT return it directly.
        if (juce::XmlElement* keyElement = xml->getChildByName("key"))
        {
            _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed key (for debug): ") + keyElement->getAllSubText()); // Console log
        } else {
            _DBG("LOnlineUnlockStatusCheck: Custom parsing: No <key> element found!"); // Console log
        }

        parsedErrorMessage = "";
        _DBG("LOnlineUnlockStatusCheck: Custom parsed error message: (empty)"); // Console log

    } else {
        parsedSuccess = false;
        parsedErrorMessage = "Failed to parse XML or invalid root tag. Raw reply: " + reply;
        parsedInformativeMessage = "";
        parsedUrlToLaunch = "";
        _DBG("LOnlineUnlockStatusCheck: Custom parsing failed or root tag mismatch: " + reply); // Console log
        _DBG("LOnlineUnlockStatusCheck: Returning EMPTY string to base class due to custom XML parsing failure."); // Console log
        return ""; // Return empty string to base if our custom parsing fails too
    }
    // --- End of your custom parsing ---

    // DEBUG HACK: Temporarily call saveState() to bypass base class's internal key validation
    // and see if isUnlocked() can then return true. REMOVE THIS LATER IN PRODUCTION.
    if (parsedSuccess) // Only if our custom parsing confirmed success
    {
        saveState("DEBUG_HACK_VALID_KEY_STRING"); // Force a state to be saved
        _DBG("LOnlineUnlockStatusCheck: DEBUG HACK: Manually called saveState to force unlock state."); // Console log
    }
    // END DEBUG HACK

    logger.log(juce::String("LOnlineUnlockStatusCheck: Returning full raw reply for base class processing: ") + reply); // File log
    _DBG(juce::String("LOnlineUnlockStatusCheck: Returning this string to base class: ") + reply); // Console log
    return reply; // IMPORTANT: Return the entire 'reply' string.
}

// isUnlocked() definition - NOT an override, but a new const version
bool LOnlineUnlockStatusCheck::isUnlocked() const
{
    juce::File debugLogPath = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
    PluginLoggerVst3 logger(debugLogPath);

    _DBG("LOnlineUnlockStatusCheck: LOnlineUnlockStatusCheck::isUnlocked() (your const version) called.");
    logger.log("LOnlineUnlockStatusCheck: LOnlineUnlockStatusCheck::isUnlocked() (your const version) called.");

    // IMPORTANT: Call the base class's non-const isUnlocked().
    // The base class's isUnlocked() will then call *your* overridden getState() and getPublicKey().
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
