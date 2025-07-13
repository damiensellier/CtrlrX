#include "stdafx.h"
#include "stdafx_luabind.h"
#include "LOnlineUnlockStatusCheck.h"
#include "CtrlrMacros.h"
#include "CtrlrLog.h"

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <iostream>
#include <fstream>

// PluginLoggerVst3 Implementation (unchanged, just showing context)
PluginLoggerVst3::PluginLoggerVst3(const juce::File& pluginExecutableFile)
{
    logFile = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                 .getChildFile("CtrlrX_debug_log.txt");
}

void PluginLoggerVst3::log(const juce::String& message)
{
    std::ofstream outfile(logFile.getFullPathName().toStdString(), std::ios_base::app);
    if (outfile.is_open()) {
        outfile << juce::Time::getCurrentTime().toString(true, true, true, true).toStdString() << ": " << message.toStdString() << std::endl;
        outfile.close();
    } else {
        std::cerr << "Error: Could not open log file for writing: " << logFile.getFullPathName().toStdString() << std::endl;
    }
}

void PluginLoggerVst3::logResult(const juce::Result& result)
{
    if (result.wasOk()) {
        log("Result: OK");
    } else {
        log("Result: FAIL - " + result.getErrorMessage());
    }
}
juce::File desktopLogPath = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                             .getChildFile("CtrlrX_debug_log.txt");

PluginLoggerVst3 logger(desktopLogPath);


// Helper functions (unchanged)
static juce::String getConsistentAppName()
{
    juce::String executableFileName = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getFileName();
    juce::String nameWithoutExtension = executableFileName.upToLastOccurrenceOf(juce::StringRef("."), false, false);
    if (nameWithoutExtension.isEmpty() && !executableFileName.isEmpty()) return executableFileName.trim();
    return nameWithoutExtension.trim();
}

static juce::File getApplicationDataDirectory(const juce::String& appName)
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile(appName);
}

static juce::File getRegistrationKeyFile(const juce::String& appName)
{
    // return juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile(juce::File::createLegalFileName(appName + "_license.key"));

    // This will create the file in a temporary directory
    // return juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile(juce::File::createLegalFileName(appName + "_license.key"));
    
    // OR, this will create it in your app's user data directory (recommended for actual production)
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile(appName).getChildFile(juce::File::createLegalFileName(appName + "_license.key"));
}


// LOnlineUnlockStatusCheck::WebUnlockThread Implementation (unchanged)
class LOnlineUnlockStatusCheck::WebUnlockThread : public juce::Thread
{
public:
    WebUnlockThread (LOnlineUnlockStatusCheck& ownerToUse, const juce::String& emailToUse, const juce::String& passwordToUse)
        : juce::Thread ("WebUnlockThread"),
          owner (ownerToUse),
          email (emailToUse),
          password (passwordToUse),
          result (juce::OnlineUnlockStatus::UnlockResult())
    {}

    void run() override
    {
        logger.log("WebUnlockThread: Starting attemptWebserverUnlock on background thread.");
        _DBG("WebUnlockThread: Starting attemptWebserverUnlock on background thread.");
        result = owner.attemptWebserverUnlock (email, password);
        logger.log(juce::String("WebUnlockThread: attemptWebserverUnlock completed. Success: ") + (result.succeeded ? "true" : "false"));
        _DBG(juce::String("WebUnlockThread: attemptWebserverUnlock completed. Success: ") + (result.succeeded ? "true" : "false"));
    }

    juce::OnlineUnlockStatus::UnlockResult getResult() const { return result; }

private:
    LOnlineUnlockStatusCheck& owner;
    juce::String email;
    juce::String password;
    juce::OnlineUnlockStatus::UnlockResult result;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WebUnlockThread)
};


// LOnlineUnlockStatusCheck Class Implementation
LOnlineUnlockStatusCheck::LOnlineUnlockStatusCheck(const juce::String& productID,
                                                   const juce::String& appNameFromLua,
                                                   const juce::URL& serverURL,
                                                   const juce::RSAKey& publicKey)
    // >>> CORRECTED: Use the default base constructor as per the API you provided <<<
    : juce::OnlineUnlockStatus(), // This is the ONLY public constructor in the API you shared.
      
      juce::Timer(), // Initialize juce::Timer (UNCHANGED)
      
      // Initialize your own member variables (UNCHANGED)
      myProductID(productID),
      myAppName(getConsistentAppName()),
      myServerURL(serverURL),
      myPublicKey(publicKey),
      persistentUnlockState("") // This will be populated by your getState() override
{
    parsedSuccess = false;
    parsedErrorMessage = "";
    parsedInformativeMessage = "";
    parsedUrlToLaunch = "";
    
    // Log the path our helper *intends* to be used for the key file.
    logger.log(juce::String("LOnlineUnlockStatusCheck: Initializing with intended key file path: ") + getRegistrationKeyFile(myAppName).getFullPathName());
    _DBG(juce::String("LOnlineUnlockStatusCheck: Initializing with intended key file path: ") + getRegistrationKeyFile(myAppName).getFullPathName());
    
    // Ensure the parent directory for the key file exists (Desktop usually exists)
    getRegistrationKeyFile(myAppName).getParentDirectory().createDirectory();
    
    // Call load(). Because getState() is virtual and overridden, it will
    // call *your* getState() which reads from the Desktop file.
    load();
}

LOnlineUnlockStatusCheck::~LOnlineUnlockStatusCheck()
{
    if (unlockThread != nullptr && unlockThread->isThreadRunning())
    {
        logger.log("LOnlineUnlockStatusCheck Destructor: Stopping unlock thread.");
        _DBG("LOnlineUnlockStatusCheck Destructor: Stopping unlock thread.");
        unlockThread->stopThread(10000);
    }
}

// Overrides from juce::OnlineUnlockStatus (unchanged logic)
juce::String LOnlineUnlockStatusCheck::getProductID() { return myProductID; }
bool LOnlineUnlockStatusCheck::doesProductIDMatch (const juce::String& returnedIDFromServer) { return myProductID == returnedIDFromServer; }
juce::RSAKey LOnlineUnlockStatusCheck::getPublicKey() { return myPublicKey; }

void LOnlineUnlockStatusCheck::saveState (const juce::String& newState)
{
    _DBG ("LOnlineUnlockStatusCheck: saveState called. Storing: " + newState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: saveState called. Storing: ") + newState);
        
    persistentUnlockState = newState;

    juce::File keyFileLocation = getRegistrationKeyFile(myAppName); // Determine the target file path

    logger.log(juce::String("LOnlineUnlockStatusCheck: (saveState) Attempting to save key to: ") + keyFileLocation.getFullPathName());
    _DBG(juce::String("LOnlineUnlockStatusCheck: (saveState) Attempting to save key to: ") + keyFileLocation.getFullPathName());

    // --- IMPORTANT: Ensure the parent directory exists before attempting to open the file ---
    juce::File parentDir = keyFileLocation.getParentDirectory();
    if (! parentDir.exists())
    {
        logger.log(juce::String("LOnlineUnlockStatusCheck: (saveState) Parent directory does not exist, attempting to create: ") + parentDir.getFullPathName());
        _DBG(juce::String("LOnlineUnlockStatusCheck: (saveState) Parent directory does not exist, attempting to create: ") + parentDir.getFullPathName());
        juce::Result createDirResult = parentDir.createDirectory();
        if (createDirResult.failed())
        {
            logger.log(juce::String("LOnlineUnlockStatusCheck: (saveState) FAILED to create parent directory: ") + createDirResult.getErrorMessage());
            _DBG(juce::String("LOnlineUnlockStatusCheck: (saveState) FAILED to create parent directory: ") + createDirResult.getErrorMessage());
            return; // Stop here if directory creation failed
        }
        logger.log(juce::String("LOnlineUnlockStatusCheck: (saveState) Successfully created parent directory: ") + parentDir.getFullPathName());
        _DBG(juce::String("LOnlineUnlockStatusCheck: (saveState) Successfully created parent directory: ") + parentDir.getFullPathName());
    }

    juce::FileOutputStream outputStream (keyFileLocation);

    if (outputStream.openedOk())
    {
        logger.log(juce::String("LOnlineUnlockStatusCheck: (saveState) FileOutputStream opened successfully for: ") + keyFileLocation.getFullPathName());
        _DBG(juce::String("LOnlineUnlockStatusCheck: (saveState) FileOutputStream opened successfully for: ") + keyFileLocation.getFullPathName());

        if (outputStream.writeString(newState))
        {
            _DBG("Successfully saved registration key to: " + keyFileLocation.getFullPathName());
            logger.log("Successfully saved registration key to: " + keyFileLocation.getFullPathName());
        }
        else
        {
            _DBG("Error writing registration key to file: " + keyFileLocation.getFullPathName() + " (writeString failed)");
            logger.log("Error writing registration key to file: " + keyFileLocation.getFullPathName() + " (writeString failed)");
        }
    }
    else
    {
        _DBG("Error opening registration key file for writing: " + keyFileLocation.getFullPathName() + " (openedOk failed)");
        logger.log("Error opening registration key file for writing: " + keyFileLocation.getFullPathName() + " (openedOk failed)");
    }
}


juce::String LOnlineUnlockStatusCheck::getState()
{
    _DBG ("LOnlineUnlockStatusCheck: getState called.");
    logger.log("LOnlineUnlockStatusCheck: getState called.");

    if (persistentUnlockState.isEmpty())
    {
        juce::File keyFile = getRegistrationKeyFile(myAppName);
        if (keyFile.existsAsFile())
        {
            juce::FileInputStream inputStream (keyFile);
            if (inputStream.openedOk())
            {
                persistentUnlockState = inputStream.readEntireStreamAsString().trim();
                _DBG("LOnlineUnlockStatusCheck: Loaded registration key from file: " + keyFile.getFullPathName());
                logger.log("LOnlineUnlockStatusCheck: Loaded registration key from file: " + keyFile.getFullPathName());
            }
            else
            {
                _DBG("Error opening registration key file for reading: " + keyFile.getFullPathName());
                logger.log("Error opening registration key file for reading: " + keyFile.getFullPathName());
                persistentUnlockState = "";
            }
        }
        else
        {
            _DBG("Registration key file does not exist: " + keyFile.getFullPathName());
            logger.log("Registration key file does not exist: " + keyFile.getFullPathName());
            persistentUnlockState = "";
        }
    }
    _DBG ("LOnlineUnlockStatusCheck: getState called. Returning: " + persistentUnlockState);
    logger.log(juce::String("LOnlineUnlockStatusCheck: getState called. Returning: ") + persistentUnlockState);
    return persistentUnlockState;
}

juce::String LOnlineUnlockStatusCheck::getWebsiteName() { return "CtrlrX Web Server"; }
juce::URL LOnlineUnlockStatusCheck::getServerAuthenticationURL() { return myServerURL; }

juce::StringArray LOnlineUnlockStatusCheck::getLocalMachineIDs()
{
    logger.log("LOnlineUnlockStatusCheck::getLocalMachineIDs() called. Returning default JUCE machine IDs.");
    _DBG("LOnlineUnlockStatusCheck::getLocalMachineIDs() called. Returning default JUCE machine IDs.");
    return juce::OnlineUnlockStatus::MachineIDUtilities::getLocalMachineIDs();
}

void LOnlineUnlockStatusCheck::setUserEmail (const juce::String& newEmailAddress)
{
    juce::OnlineUnlockStatus::setUserEmail(newEmailAddress);
    logger.log(juce::String("LOnlineUnlockStatusCheck: setUserEmail called with: ") + newEmailAddress);
    _DBG(juce::String("LOnlineUnlockStatusCheck: setUserEmail called with: ") + newEmailAddress);
}

juce::String LOnlineUnlockStatusCheck::readReplyFromWebserver (const juce::String& email, const juce::String& password)
{
    juce::URL serverUrl = getServerAuthenticationURL();
    serverUrl = serverUrl.withParameter ("email", email);
    serverUrl = serverUrl.withParameter ("password", password);
    serverUrl = serverUrl.withParameter ("machineids", juce::OnlineUnlockStatus::MachineIDUtilities::getLocalMachineIDs().joinIntoString(","));

    logger.log(juce::String("LOnlineUnlockStatusCheck: Attempting web request to: ") + serverUrl.toString(true));
    _DBG(juce::String("LOnlineUnlockStatusCheck: Attempting web request to server: " + serverUrl.toString(true)));

    juce::String reply = serverUrl.readEntireTextStream (false);

    logger.log(juce::String("LOnlineUnlockStatusCheck: Raw server reply received: ") + reply);
    _DBG(juce::String("LOnlineUnlockStatusCheck: Raw server reply received (console): ") + reply);

    if (reply.isEmpty())
    {
        parsedSuccess = false;
        parsedErrorMessage = "Empty reply from webserver.";
        parsedInformativeMessage = "";
        parsedUrlToLaunch = "";
        logger.log("LOnlineUnlockStatusCheck: Empty reply from webserver. Unlock attempt failed early.");
        _DBG("LOnlineUnlockStatusCheck: Empty reply from webserver. Unlock attempt failed early.");
        return "";
    }

    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(reply);

    if (xml != nullptr && xml->getTagName() == "OnlineUnlockStatus")
    {
        parsedSuccess = xml->getBoolAttribute("success", false);
        logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false"));
        _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed success: ") + (parsedSuccess ? "true" : "false"));

        if (parsedSuccess) logger.log("LOnlineUnlockStatusCheck: Server indicated success.");
        else logger.log("LOnlineUnlockStatusCheck: Server indicated FAILURE.");

        if (juce::XmlElement* messageElement = xml->getChildByName("message")) parsedInformativeMessage = messageElement->getAllSubText();
        else parsedInformativeMessage = "";
        logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage);
        _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed informative message: ") + parsedInformativeMessage);
        
        if (juce::XmlElement* urlElement = xml->getChildByName("url")) parsedUrlToLaunch = urlElement->getAllSubText();
        else parsedUrlToLaunch = "";
        logger.log(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch);
        _DBG(juce::String("LOnlineUnlockStatusCheck: Custom parsed URL to launch: ") + parsedUrlToLaunch);

        if (! parsedSuccess) parsedErrorMessage = parsedInformativeMessage;
        else parsedErrorMessage = "";

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
    return reply;
}

// Override from juce::Timer
void LOnlineUnlockStatusCheck::timerCallback()
{
    if (unlockThread != nullptr && !unlockThread->isThreadRunning())
    {
        juce::OnlineUnlockStatus::UnlockResult threadResult = unlockThread->getResult();
        
        parsedSuccess = threadResult.succeeded;
        parsedErrorMessage = threadResult.errorMessage;
        parsedInformativeMessage = threadResult.informativeMessage;
        parsedUrlToLaunch = threadResult.urlToLaunch;

        logger.log(juce::String("LOnlineUnlockStatusCheck::timerCallback: Unlock thread completed. Success: ") + (threadResult.succeeded ? "true" : "false"));
        _DBG(juce::String("LOnlineUnlockStatusCheck::timerCallback: Unlock thread completed. Success: ") + (threadResult.succeeded ? "true" : "false"));

        stopTimer();
        unlockThread = nullptr;

        // >>> NEW: Invoke the Lua callback if set <<<
        if (unlockCompletionCallback)
        {
            try {
                // Call the stored Lua function with the results
                unlockCompletionCallback(parsedSuccess, parsedErrorMessage, parsedInformativeMessage, parsedUrlToLaunch);
                logger.log("LOnlineUnlockStatusCheck::timerCallback: Successfully invoked Lua unlockCompletionCallback.");
                _DBG("LOnlineUnlockStatusCheck::timerCallback: Successfully invoked Lua unlockCompletionCallback.");
            }
            catch (const luabind::error& e) {
                logger.log(juce::String("LOnlineUnlockStatusCheck::timerCallback: Lua callback error: ") + e.what());
                _DBG(juce::String("LOnlineUnlockStatusCheck::timerCallback: Lua callback error: ") + e.what());
            }
            catch (const std::exception& e) {
                logger.log(juce::String("LOnlineUnlockStatusCheck::timerCallback: Std exception in Lua callback: ") + e.what());
                _DBG(juce::String("LOnlineUnlockStatusCheck::timerCallback: Std exception in Lua callback: ") + e.what());
            }
            catch (...) {
                logger.log("LOnlineUnlockStatusCheck::timerCallback: Unknown exception in Lua callback.");
                _DBG("LOnlineUnlockStatusCheck::timerCallback: Unknown exception in Lua callback.");
            }
        }
        else
        {
            logger.log("LOnlineUnlockStatusCheck::timerCallback: No Lua unlockCompletionCallback set.");
            _DBG("LOnlineUnlockStatusCheck::timerCallback: No Lua unlockCompletionCallback set.");
        }
    }
}


// Custom methods for LOnlineUnlockStatusCheck (unchanged logic)
bool LOnlineUnlockStatusCheck::isUnlocked() const
{
    _DBG("LOnlineUnlockStatusCheck: LOnlineUnlockStatusCheck::isUnlocked() called. Calling base class for real validation.");
    logger.log("LOnlineUnlockStatusCheck: LOnlineUnlockStatusCheck::isUnlocked() called. Calling base class for real validation.");
    
    bool unlocked = static_cast<const juce::OnlineUnlockStatus*>(this)->isUnlocked();
    
    _DBG(juce::String("LOnlineUnlockStatusCheck: isUnlocked() base OnlineUnlockStatus::isUnlocked() returning: ") + (unlocked ? "true" : "false"));
    logger.log(juce::String("LOnlineUnlockStatusCheck: isUnlocked() base OnlineUnlockStatus::isUnlocked() returning: ") + (unlocked ? "true" : "false"));
    return unlocked;
}

bool LOnlineUnlockStatusCheck::isPluginUnlocked() const { return isUnlocked(); }

void LOnlineUnlockStatusCheck::triggerWebserverUnlock (const juce::String& email, const juce::String& password)
{
    if (unlockThread != nullptr && unlockThread->isThreadRunning())
    {
        logger.log("LOnlineUnlockStatusCheck: Unlock thread already running. Ignoring new request.");
        _DBG("LOnlineUnlockStatusCheck: Unlock thread already running. Ignoring new request.");
        return;
    }
    logger.log("LOnlineUnlockStatusCheck: Creating and starting new WebUnlockThread.");
    _DBG("LOnlineUnlockStatusCheck: Creating and starting new WebUnlockThread.");
    
    unlockThread.reset (new WebUnlockThread (*this, email, password));
    unlockThread->startThread();
    
    startTimer(500);
}

// >>> NEW: Implementation for setting the Lua callback <<<
void LOnlineUnlockStatusCheck::setUnlockCompletionCallback(luabind::object const& luaFunction)
{
    luaCallbackObject = luaFunction; // Store the luabind::object to prevent premature GC
    if (luaFunction.is_valid() && luabind::type(luaFunction) == LUA_TFUNCTION)
    {
        // Wrap the luabind::object in a std::function for easier C++ calling.
        // This lambda captures 'luaFunction' and calls it via luabind::call.
        unlockCompletionCallback = [this, luaFunction](bool success, juce::String errorMsg, juce::String infoMsg, juce::String url) {
            // Ensure we are on the Message Thread before invoking Lua callback if Lua interacts with UI
            // However, luabind::call itself might handle thread context or you might need a MessageManagerLock.
            // For now, call directly, assuming luabind can handle the context or the Lua function is thread-safe.
            // If you get crashes, you might need to defer this call to the Message Thread using invokeLater.
            luabind::call_function<void>(luaFunction, success, errorMsg, infoMsg, url);
        };
        logger.log("LOnlineUnlockStatusCheck: Lua unlock completion callback set successfully.");
        _DBG("LOnlineUnlockStatusCheck: Lua unlock completion callback set successfully.");
    }
    else
    {
        unlockCompletionCallback = nullptr; // Clear the callback if invalid
        luaCallbackObject = luabind::object(); // Clear luabind object
        logger.log("LOnlineUnlockStatusCheck: Attempted to set an invalid Lua unlock completion callback.");
        _DBG("LOnlineUnlockStatusCheck: Attempted to set an invalid Lua unlock completion callback.");
    }
}


// Lua binding (NOW INCLUDING THE NEW METHOD)
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
            .def("getLocalMachineIDs", &LOnlineUnlockStatusCheck::getLocalMachineIDs)
            .def("setUserEmail", &LOnlineUnlockStatusCheck::setUserEmail)
            .def("isUnlocked", &LOnlineUnlockStatusCheck::isUnlocked)
            .def("isPluginUnlocked", &LOnlineUnlockStatusCheck::isPluginUnlocked)
            .def("triggerWebserverUnlock", &LOnlineUnlockStatusCheck::triggerWebserverUnlock)
            .def("getParsedSuccess", &LOnlineUnlockStatusCheck::getParsedSuccess)
            .def("getParsedErrorMessage", &LOnlineUnlockStatusCheck::getParsedErrorMessage)
            .def("getParsedInformativeMessage", &LOnlineUnlockStatusCheck::getParsedInformativeMessage)
            .def("getParsedUrlToLaunch", &LOnlineUnlockStatusCheck::getParsedUrlToLaunch)
            // >>> NEW: Bind the setUnlockCompletionCallback method <<<
            .def("setUnlockCompletionCallback", &LOnlineUnlockStatusCheck::setUnlockCompletionCallback)
    ];
}
