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
    static GZIPDecompressorInputStream* create(InputStream* stream, bool deleteStream);
};

#endif // __L_GZIP
