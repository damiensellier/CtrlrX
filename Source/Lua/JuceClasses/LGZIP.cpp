#include "JuceHeader.h" 
#include "LGZIP.h"
#include <luabind/luabind.hpp>

using namespace juce;

GZIPDecompressorInputStream* LGZIPDecompressorInputStream::create(InputStream* stream, bool deleteStream)
{
    return new GZIPDecompressorInputStream(stream, deleteStream);
}

void LGZIPDecompressorInputStream::wrapForLua(lua_State* L)
{
    using namespace luabind;
    module(L)
        [
            class_<GZIPDecompressorInputStream, InputStream>("GZIPDecompressorInputStream")
                .scope
                [
                    def("create", &LGZIPDecompressorInputStream::create)
                ]
        ];
}