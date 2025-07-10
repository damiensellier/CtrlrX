#ifndef L_ONLINE_UNLOCK_STATUS
#define L_ONLINE_UNLOCK_STATUS

extern "C"
{
    #include "lua.h"
}

// **Crucial:** Use JuceHeader.h to pull in all necessary JUCE modules
#include "JuceHeader.h"

// Include your custom LOnlineUnlockStatusCheck subclass
#include "LOnlineUnlockStatusCheck.h"

class LOnlineUnlockStatus
{
public:
    static void wrapForLua (lua_State *L);
};

#endif // L_ONLINE_UNLOCK_STATUS
