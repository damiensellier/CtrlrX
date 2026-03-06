#ifndef L_ONLINE_UNLOCK_STATUS_CHECK
#define L_ONLINE_UNLOCK_STATUS_CHECK

extern "C"
{
    #include "lua.h"
}

// Includes all necessary JUCE classes like OnlineUnlockStatus, RSAKey, URL, String, File, ValueTree, Timer
#include "JuceHeader.h"

#include <fstream> // Required for PluginLoggerVst3

// Forward declaration for LRSAKey (if LRSAKey is used as a type elsewhere and defined later)
class LRSAKey;


// PluginLoggerVst3 Declaration
class PluginLoggerVst3
{
public:
    // Original constructor (optional, can keep for backward compatibility)
    PluginLoggerVst3(const juce::File& pluginExecutableFile);

    // Constructor with an initial enable/disable flag
    PluginLoggerVst3(const juce::File& pluginExecutableFile, bool initiallyEnabled);

    // Optional: Method to change status at runtime (if you want Lua to control it)
    void setEnabled(bool enable);

    void log(const juce::String& message);
    void logResult(const juce::Result& result);

private:
    juce::File logFile;
    bool isEnabled; // Member to control logging
};

// This line remains unchanged to declare the global logger instance
extern PluginLoggerVst3 logger;

// LOnlineUnlockStatusCheck inherits from juce::OnlineUnlockStatus and juce::Timer
class LOnlineUnlockStatusCheck : public juce::OnlineUnlockStatus,
                                 public juce::Timer
{
public:
    // Constructor
    LOnlineUnlockStatusCheck(const juce::String& productID,
                             const juce::String& appNameFromLua, // <--- This parameter is new/changed
                             const juce::URL& serverURL,
                             const juce::RSAKey& publicKey);

    // Destructor (defaulted as there are no raw pointers to manage, properly overrides virtual base)
    ~LOnlineUnlockStatusCheck() override;

    //==============================================================================
    // Overrides from juce::OnlineUnlockStatus (these are typically pure virtual)

    juce::String getProductID() override;
    bool doesProductIDMatch (const juce::String& returnedIDFromServer) override;
    juce::RSAKey getPublicKey() override;
    
    // **IMPORTANT:** saveState and getState overrides will now be correctly called by base load()/save()
    void saveState (const juce::String& newState) override;
    juce::String getState() override;
    
    juce::String getWebsiteName() override;
    juce::URL getServerAuthenticationURL() override;
    
    // readReplyFromWebserver returns the full XML to base class for its processing
    juce::String readReplyFromWebserver (const juce::String& email, const juce::String& password) override;

    juce::StringArray getLocalMachineIDs() override; // Essential for concrete class
    
    // setUserEmail() in JUCE OnlineUnlockStatus is NOT virtual, so we cannot use 'override'.
    void setUserEmail (const juce::String& newEmailAddress);

    //==============================================================================
    // Override from juce::Timer (unchanged logic, but will now trigger callback)
    void timerCallback() override;

    //==============================================================================
    // Custom methods and members for LOnlineUnlockStatusCheck

    // isUnlocked() directly calls the base class's non-virtual isUnlocked().
    // It will return true ONLY if the key saved via saveState is cryptographically valid.
    bool isUnlocked() const;
    bool isPluginUnlocked() const; // Convenience method

    // Initiates the web server unlock process on a background thread
    void triggerWebserverUnlock (const juce::String& email, const juce::String& password);

    // Public getters for the parsed data from the webserver reply (for custom UI/Lua display)
    bool getParsedSuccess() const                         { return parsedSuccess; }
    juce::String getParsedErrorMessage() const            { return parsedErrorMessage; }
    juce::String getParsedInformativeMessage() const      { return parsedInformativeMessage; }
    juce::String getParsedUrlToLaunch() const             { return parsedUrlToLaunch; }

    // Method to set the Lua callback
    void setUnlockCompletionCallback (luabind::object const& luaFunction);

    // This function wraps the class for Lua binding
    static void wrapForLua (lua_State *L);

private:
    // Member variables to hold initialisation parameters
    juce::String myProductID;
    juce::String myAppName;
    juce::URL myServerURL;
    juce::RSAKey myPublicKey;

    // Stores the persistent unlock state as managed by the base JUCE class
    juce::String persistentUnlockState;

    // Member variables to store parsed data from webserver reply (for custom use/display)
    bool parsedSuccess;
    juce::String parsedErrorMessage;
    juce::String parsedInformativeMessage;
    juce::String parsedUrlToLaunch;

    // Nested class for background web unlock operation
    class WebUnlockThread;
    std::unique_ptr<WebUnlockThread> unlockThread; // Manages the background thread
    
    // Member to hold the Lua callback function
    // Using std::function to wrap the luabind::object for easier C++ usage.
    // The arguments match what you'll pass back to Lua.
    std::function<void(bool, juce::String, juce::String, juce::String)> unlockCompletionCallback;
    luabind::object luaCallbackObject; // Keep the original luabind::object to prevent GC if needed


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LOnlineUnlockStatusCheck)
};

#endif // L_ONLINE_UNLOCK_STATUS_CHECK
