// LRSAKey.cpp
#include "LRSAKey.h"
#include "stdafx_luabind.h" // This now pulls in JuceHeader.h and all Luabind headers

// For std::pair binding (might be pulled by stdafx_luabind.h if it includes <luabind/stl/pair.hpp>)
#include <utility>

// ==============================================================================
// LRSAKey static member function definitions
// ==============================================================================

std::pair<juce::RSAKey, juce::RSAKey> LRSAKey::createKeyPairWrapper(int numBits)
{
    juce::RSAKey publicKey;
    juce::RSAKey privateKey;
    juce::RSAKey::createKeyPair(publicKey, privateKey, numBits);
    return {publicKey, privateKey};
}

juce::String LRSAKey::getDummyString()
{
    return "Hello from LRSAKey C++!";
}

// ==============================================================================
// LRSAKey::wrapForLua static member function.
// ==============================================================================
void LRSAKey::wrapForLua (lua_State *L)
{
    using namespace luabind;

    // 1. Bind juce::RSAKey as "RSAKey"
    module(L)
    [
        // juce::RSAKey is a ReferenceCountedObject, so use adopt(result) for constructors
        class_<juce::RSAKey>("RSAKey")
            .def(constructor<>(), adopt(result)) // Default constructor, Lua adopts
            .def(constructor<const juce::String&>(), adopt(result)) // String constructor, Lua adopts
            .def("toString",            &juce::RSAKey::toString)
            .def("isValid",             &juce::RSAKey::isValid)
            .def("applyToValue",        &juce::RSAKey::applyToValue)
    ];

    // 2. Bind std::pair<juce::RSAKey, juce::RSAKey> as "RSAKeyPair"
    // Luabind should handle the adopt policy for the contained RSAKey objects
    // if RSAKey itself is bound correctly.
    module(L)
    [
        class_<std::pair<juce::RSAKey, juce::RSAKey>>("RSAKeyPair")
            .def_readonly("first",  &std::pair<juce::RSAKey, juce::RSAKey>::first)
            .def_readonly("second", &std::pair<juce::RSAKey, juce::RSAKey>::second)
    ];

    // 3. Bind LRSAKey as a separate Lua class, e.g., "RSATools" for its static methods
    module(L)
    [
        class_<LRSAKey>("RSATools")
            .scope
            [
                def("createKeyPair", static_cast<std::pair<juce::RSAKey, juce::RSAKey> (*)(int)>(&LRSAKey::createKeyPairWrapper)),
                def("getDummy",      static_cast<juce::String (*)()>(&LRSAKey::getDummyString))
            ]
    ];
}
