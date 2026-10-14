#pragma once

#include "test_ProcessorFixture.h"

#include "CtrlrPanel.h"
#include "CtrlrLuaManager.h"

#include <cmath>
#include <limits>

// Raw Lua 5.1 C API, for reading globals back. Lua is vendored with C linkage.
extern "C"
{
    #include "lua.h"
}

/**
 * Shared fixture for the Lua coverage suites (test_ProcessorLua{Values,Timers,State,Data}.cpp).
 *
 * Observation seam: a test runs a Lua chunk in the panel's own Lua state through
 * CtrlrLuaManager::runCode (the entry point the Lua console uses) and reads Lua globals back
 * through the C API. Unlike the result-modulator seam in test_ProcessorLua.cpp this can tell
 * what happened *inside* the chunk from what only happened after the message loop ran (value
 * change callbacks are dispatched asynchronously). No production code is touched.
 */
class LuaPanel : public ProcessorInstance
{
protected:
    CtrlrPanel* panel = nullptr;

    // Load a fixture from Tests/fixtures and let the load-time async work settle.
    CtrlrPanel* loadLuaPanel(const std::string& file)
    {
        panel = loadPanel(file);
        processor->prepareToPlay(44100, BLOCK_SIZE);
        pumpInputThreads(100);
        return panel;
    }

    // Run `code` in the panel's Lua state. On failure the assertion message carries Lua's error.
    ::testing::AssertionResult runLua(const juce::String& code)
    {
        if (panel == nullptr)
            return ::testing::AssertionFailure() << "no panel loaded";
        if (panel->getCtrlrLuaManager().runCode(code, "test"))
            return ::testing::AssertionSuccess();
        return ::testing::AssertionFailure() << panel->getCtrlrLuaManager().getLastError();
    }

    // A Lua global as a number; NaN if it is nil or not a number.
    double luaNumber(const char* name)
    {
        lua_State* L = panel->getCtrlrLuaManager().getLuaState();
        lua_getglobal(L, name);
        const double v = lua_isnumber(L, -1) ? lua_tonumber(L, -1)
                                             : std::numeric_limits<double>::quiet_NaN();
        lua_pop(L, 1);
        return v;
    }

    bool luaIsNil(const char* name)
    {
        lua_State* L = panel->getCtrlrLuaManager().getLuaState();
        lua_getglobal(L, name);
        const bool nil = lua_isnil(L, -1);
        lua_pop(L, 1);
        return nil;
    }
};
