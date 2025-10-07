/*
  ==============================================================================
    LGZIP.cpp
    Created: 7 Oct 2025 10:22:53am
    Author:  zan64
  ==============================================================================
*/
#include "JuceHeader.h" 
#include "LGZIP.h"
#include <luabind/luabind.hpp>

using namespace juce;

void LGZIPDecompressorInputStream::wrapForLua(lua_State* L)
{
    using namespace luabind;
    module(L)
        [
            class_<GZIPDecompressorInputStream, InputStream>("GZIPDecompressorInputStream")
                .def(constructor<InputStream*, bool, GZIPDecompressorInputStream::Format>())
                .def(constructor<InputStream*, bool>())
        ];
}