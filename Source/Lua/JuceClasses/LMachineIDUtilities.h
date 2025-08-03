#ifndef L_MACHINE_ID_UTILITIES
#define L_MACHINE_ID_UTILITIES

extern "C"
{
    #include "lua.h" // Keep this for lua_State*
}

// JUCE headers
// This single include should bring in juce::StringArray and juce::OnlineUnlockStatus
// (which contains MachineIDUtilities) IF the respective modules are enabled.
#include "JuceHeader.h"

class LMachineIDUtilities
{
public:
    static void wrapForLua (lua_State *L);
};

#endif // L_MACHINE_ID_UTILITIES
