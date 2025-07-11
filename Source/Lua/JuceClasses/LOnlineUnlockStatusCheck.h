#ifndef L_ONLINE_UNLOCK_STATUS_CHECK
#define L_ONLINE_UNLOCK_STATUS_CHECK

extern "C"
{
    #include "lua.h"
}

#include "JuceHeader.h" // Includes all necessary JUCE classes like OnlineUnlockStatus, RSAKey, URL, String, File, ValueTree

#include <fstream> // Required for vst3 logger (from your original file)

// Forward declaration for LRSAKey (keeping as per your original file)
class LRSAKey;

// LOnlineUnlockStatusCheck inherits from juce::OnlineUnlockStatus
class LOnlineUnlockStatusCheck  : public juce::OnlineUnlockStatus
{
public:
    // Constructor
    LOnlineUnlockStatusCheck (const juce::String& productID,
                              const juce::String& appName,
                              const juce::URL& serverURL,
                              const juce::RSAKey& publicKey);

    // Destructor (defaulted as there are no raw pointers to manage)
    ~LOnlineUnlockStatusCheck() override = default;

    //==============================================================================
    // Overrides from juce::OnlineUnlockStatus

    juce::String getProductID() override;
    bool doesProductIDMatch (const juce::String& returnedIDFromServer) override;
    juce::RSAKey getPublicKey() override;
    
    // IMPORTANT: These are now properly implemented to save/retrieve internal state
    void saveState (const juce::String& newState) override;
    
    // getState() in JUCE OnlineUnlockStatus is NOT virtual and NOT const.
    // So, we cannot use 'override', but we can make our version const for local calls.
    juce::String getState() override;
    
    juce::String getWebsiteName() override;
    juce::URL getServerAuthenticationURL() override;
    
    // readReplyFromWebserver returns the full XML to base class for its processing
    juce::String readReplyFromWebserver (const juce::String& email, const juce::String& password) override;

    juce::StringArray getLocalMachineIDs() override; // Essential for concrete class

    //==============================================================================
    // Custom methods and members for LOnlineUnlockStatusCheck
    // isUnlocked() directly calls the base class's non-virtual isUnlocked().
    // It will return true ONLY if the key saved via saveState is cryptographically valid.
    bool isUnlocked() const;
    bool isPluginUnlocked() const; // Convenience method


    // Public getters for the parsed data from the webserver reply (for custom UI/Lua display)
    bool getParsedSuccess() const                        { return parsedSuccess; }
    juce::String getParsedErrorMessage() const           { return parsedErrorMessage; }
    juce::String getParsedInformativeMessage() const     { return parsedInformativeMessage; }
    juce::String getParsedUrlToLaunch() const            { return parsedUrlToLaunch; }

    // This function wraps the class for Lua binding
    static void wrapForLua (lua_State *L);

private:
    // Member variables to hold initialisation parameters
    juce::String myProductID;
    juce::String myAppName;
    juce::URL myServerURL;
    juce::RSAKey myPublicKey;

    // NEW MEMBER: Stores the persistent unlock state as managed by the base JUCE class
    juce::String persistentUnlockState;

    // Member variables to store parsed data from webserver reply (for custom use/display)
    bool parsedSuccess;
    juce::String parsedErrorMessage;
    juce::String parsedInformativeMessage;
    juce::String parsedUrlToLaunch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LOnlineUnlockStatusCheck)
};



class PluginLoggerVst3 { // Keeping as is from your original file
public:
    PluginLoggerVst3(const juce::File& pluginExecutableFile) {
        juce::String fileExt = pluginExecutableFile.getFileExtension();
        if (fileExt == ".exe") {
            logFile = pluginExecutableFile.getParentDirectory().getChildFile("CtrlrX_debug_log.txt");
        }
        else {
            logFile = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("CtrlrX_debug_log.txt");
        }
        if (!logFile.exists()) {
            logFile.create();
        }
    }

    void log(const juce::String& message) {
        std::ofstream outfile(logFile.getFullPathName().toStdString(), std::ios_base::app);
        if (outfile.is_open()) {
            outfile << juce::Time::getCurrentTime().toString(true, true, true, true) << ": " << message.toStdString() << std::endl;
            outfile.close();
        } else {
            std::cerr << "Error: Could not open log file for writing." << std::endl;
        }
    }

    void logResult(const juce::Result& result) {
        if (result.wasOk()) {
            log("Result: OK");
        } else {
            log("Result: FAIL - " + result.getErrorMessage());
        }
    }

private:
    juce::File logFile;
};

#endif // L_ONLINE_UNLOCK_STATUS_CHECK
