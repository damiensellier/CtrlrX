#ifndef L_PLUGIN_HOST_TYPE
#define L_PLUGIN_HOST_TYPE

extern "C"
{
    #include "lua.h"
}

class LPluginHostType
{
public:
    static void wrapForLua (lua_State *L);
};

#endif