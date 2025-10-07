#ifndef __L_GZIP
#define __L_GZIP

extern "C"
{
#include "lua.h"
}

class LGZIPDecompressorInputStream
{
public:
    static void wrapForLua(lua_State* L);
};

#endif // __L_GZIP