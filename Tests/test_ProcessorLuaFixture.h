#pragma once

#include "test_ProcessorFixture.h"

#include "CtrlrPanel.h"
#include "CtrlrLuaManager.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

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

    // Destroy the processor here, not in the fixture's destructor: deviceSends is destroyed
    // before `processor` there, while JUCE's MidiOutput thread may still be delivering a send
    // into it (use-after-free). Destroying the processor joins that thread first.
    void TearDown() override
    {
        panel = nullptr;
        processor.reset();
        ProcessorInstance::TearDown();
    }

    // Host-side observer: what a VST3/AU wrapper would be told (parameter index per notification).
    struct HostRecorder : public juce::AudioProcessorListener
    {
        std::vector<int> indices;
        void audioProcessorParameterChanged (juce::AudioProcessor*, int index, float) override { indices.push_back (index); }
        void audioProcessorChanged (juce::AudioProcessor*, const ChangeDetails&) override {}
        int count (int index) const { return (int) std::count (indices.begin(), indices.end(), index); }
    };

    // Load a fixture from Tests/fixtures and let the load-time async work settle. Device sends are
    // permitted and recorded (fixtures may route modulators to the mock output device).
    CtrlrPanel* loadLuaPanel(const std::string& file)
    {
        allowAndRecordDeviceSends();
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

    // A Lua global as a string ("" if it is nil; numbers are converted by Lua).
    std::string luaString(const char* name)
    {
        lua_State* L = panel->getCtrlrLuaManager().getLuaState();
        lua_getglobal(L, name);
        const char* s = lua_tostring(L, -1);
        const std::string v = s != nullptr ? s : "";
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
