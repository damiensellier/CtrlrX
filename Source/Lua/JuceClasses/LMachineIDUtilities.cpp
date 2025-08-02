#include "stdafx.h"
#include "stdafx_luabind.h"
#include "LJuce.h"             // Your overarching JUCE binding header
#include "LMachineIDUtilities.h" // Your own header for this binding file

// JUCE headers:
// This single include should bring in juce::StringArray, juce::String,
// and juce::OnlineUnlockStatus (which contains MachineIDUtilities)
// IF the respective modules (juce_core, juce_product_unlocking) are enabled.
#include "JuceHeader.h"

// Luabind headers
#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/scope.hpp>

void LMachineIDUtilities::wrapForLua (lua_State *L)
{
    using namespace luabind;

    // EDIT : THIS IS ALREADY BOUND SOMEWHERE ELSE. Binding juce::StringArray as "StringArray"
//    module(L)
//    [
//        class_<juce::StringArray>("StringArray")
//            .def(constructor<>())
//            .def("size", &juce::StringArray::size)
//            .def("isEmpty", &juce::StringArray::isEmpty)
//            .def("get", (const juce::String& (juce::StringArray::*)(int) const) &juce::StringArray::operator[])
//            .def("joinIntoString", &juce::StringArray::joinIntoString)
//    ];

    module(L)
    [
        // Binding the nested juce::OnlineUnlockStatus::MachineIDUtilities
        // as "MachineIDUtilities" in Lua.
        class_<juce::OnlineUnlockStatus::MachineIDUtilities>("MachineIDUtilities") // <-- CORRECTED CLASS TYPE
            .scope
            [
                // Binding the static getLocalMachineIDs method
                def("getLocalMachineIDs", &juce::OnlineUnlockStatus::MachineIDUtilities::getLocalMachineIDs)
            ]
    ];
}
