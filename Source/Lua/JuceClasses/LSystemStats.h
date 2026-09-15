#ifndef L_SYSTEM_STATS
#define L_SYSTEM_STATS

extern "C"
{
    #include "lua.h"
}

class LSystemStats
{
public:
    static void wrapForLua (lua_State *L);
};

#endif