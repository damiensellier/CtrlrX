#include "test_ProcessorLuaFixture.h"

#include <chrono>
#include <thread>

/**
 * Lua value paths and the value-change callbacks they fire.
 *
 * Pins, per path (mod:setValue, mod:setModulatorValue with its vst/midi/ui flags, host
 * parameter, incoming MIDI), the value afterwards, whether luaModulatorValueChange ran, when
 * (inside the calling chunk or only after the message loop ran), the `source` it received and
 * the MIDI emitted. Also lookups (getModulatorByName and the typed component getters).
 *
 * Tests marked "Characterization" pin current behaviour that may surprise a panel author; the
 * PR description lists them so the maintainer can say whether any of it is a bug.
 *
 * Source values (CtrlrModulatorValue's change source): 1 host, 2 MIDI in, 5 Lua, 6 program.
 *
 * Fixture fixture_lua_values: mock device in (port 00) / out (port 01), device channel 2.
 *  - target (vst 1, CC 10), other (vst 2, CC 11), boot (vst 3, CC 12): sliders whose callbacks
 *    record _<name>Calls / _<name>Value / _<name>Source, append "<T|O|B><value>/<source>;" to
 *    _log, then call the optional Lua global _<name>Hook(mod, value, source).
 *  - nrpnA / nrpnB (vst 4 / 5): NRPN 300 / 301 (Multi, no header latching), no callback.
 *  - aLabel / aCombo / aList: a uiLabel, uiCombo and uiListBox for the typed lookups.
 *  - luaPanelLoaded records the bootstrap state and sets `boot` to 7.
 */

namespace
{
    constexpr int srcHost = 1;
    constexpr int srcMidiIn = 2;
    constexpr int srcLua = 5;

    juce::MidiMessage targetCC(int value) { return juce::MidiMessage::controllerEvent(2, 10, value); }
    juce::MidiMessage otherCC(int value) { return juce::MidiMessage::controllerEvent(2, 11, value); }
    juce::MidiMessage bootCC(int value) { return juce::MidiMessage::controllerEvent(2, 12, value); }

    // One NRPN as the device should see it: header (CC 99/98) then data (CC 6/38), channel 2.
    std::vector<juce::MidiMessage> nrpnBurst(int number, int value)
    {
        return { juce::MidiMessage::controllerEvent(2, 99, (number >> 7) & 0x7f),
                 juce::MidiMessage::controllerEvent(2, 98, number & 0x7f),
                 juce::MidiMessage::controllerEvent(2, 6, (value >> 7) & 0x7f),
                 juce::MidiMessage::controllerEvent(2, 38, value & 0x7f) };
    }

    // The device stream for `pairs` rounds of nrpnA = i, nrpnB = 100 + i (i = 1..pairs), each
    // burst whole and in order -- what a synth needs to apply every value to its own parameter.
    std::vector<juce::MidiMessage> contiguousBursts(int pairs)
    {
        std::vector<juce::MidiMessage> all;
        for (int i = 1; i <= pairs; i++)
            for (const auto& burst : { nrpnBurst(300, i), nrpnBurst(301, 100 + i) })
                all.insert(all.end(), burst.begin(), burst.end());
        return all;
    }

    // Lua that sets nrpnA / nrpnB alternately, `pairs` times, in one chunk.
    juce::String setNrpnPairs(int pairs)
    {
        return "local a, b = panel:getModulatorByName('nrpnA'), panel:getModulatorByName('nrpnB')\n"
               "for i = 1, " + juce::String(pairs) + " do a:setValue(i, true) b:setValue(100 + i, true) end";
    }

    // Enough bursts in one chunk that at least two are queued before JUCE's MidiOutput thread
    // drains the first; with a single pair the interleave only shows in ~85% of runs.
    constexpr int nrpnPairs = 8;
}

class LuaValues : public LuaPanel
{
protected:
    HostRecorder host;

    void TearDown() override
    {
        if (processor != nullptr)
            processor->removeListener(&host);
        LuaPanel::TearDown();
    }

    // Load fixture_lua_values, start observing the host, and forget what loading produced.
    void loadValues()
    {
        ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);
        processor->addListener(&host);
        clearObservations();
    }

    // Let pending async updates run, then forget recorded callbacks, device sends and host
    // notifications.
    void clearObservations()
    {
        pumpInputThreads(50);
        ASSERT_TRUE(runLua("_targetCalls, _targetValue, _targetSource = nil, nil, nil\n"
                           "_otherCalls, _otherValue, _otherSource = nil, nil, nil\n"
                           "_targetHook, _otherHook, _log, _inChunk = nil, nil, nil, nil"));
        takeSendsAfterDelivery(50);
        host.indices.clear();
    }

    // Put `target` back to 0 without a callback or MIDI (ui = true, midi = false), then clear.
    void resetTarget()
    {
        ASSERT_TRUE(runLua("panel:getModulatorByName('target'):setModulatorValue(0, false, false, true)"));
        clearObservations();
    }

    // Run `code` with locals t / o bound to target / other. The target's callback count at the
    // end of the chunk is saved in _inChunk, so a test can tell "ran inside the chunk" from "ran
    // after the message loop".
    ::testing::AssertionResult runWithModulators(const juce::String& code)
    {
        return runLua("local t = panel:getModulatorByName('target')\n"
                      "local o = panel:getModulatorByName('other')\n"
                      + code + "\n_inChunk = _targetCalls or 0");
    }

    int count(const char* global) { return luaIsNil(global) ? 0 : (int) luaNumber(global); }

    double targetValue()
    {
        runLua("_readBack = panel:getModulatorByName('target'):getModulatorValue()");
        return luaNumber("_readBack");
    }

    // Pump the message loop (async value updates, callbacks), then collect the device output.
    std::vector<juce::MidiMessage> pumpAndTakeDeviceSends()
    {
        pumpInputThreads(200);
        return takeSendsAfterDelivery(150);
    }

    void setFromHost(int vstIndex, int value)
    {
        processor->prepareToPlay(44100, BLOCK_SIZE);
        processor->setParameter(vstIndex, value / 127.0f);
        processor->processBlock(buffer, midiMessages);
        midiMessages.clear();
    }

    bool deviceObservable() { return midi_mock.hasSubsystemMock(); }
};

// ----------------------------------------------------------------------------------------------
// Lua value paths
// ----------------------------------------------------------------------------------------------

namespace
{
    struct ValuePath
    {
        const char* call;         // Lua; `t` is the target modulator, at value 0 before the call
        double value;             // getModulatorValue() afterwards (= the callback's value arg)
        int callbacks;            // luaModulatorValueChange runs, all after the chunk returned
        int midiValue;            // CC 10 value sent to the device; -1 = nothing sent
        const char* note;
    };

    const ValuePath valuePaths[] = {
        { "t:setValue(42, false)",                        42, 1, 42, "" },
        { "t:setValue(42, true)",                         42, 1, 42, "force only matters for an unchanged value" },
        { "t:setValue(42, true, true)",                   42, 1, -1, "third arg = mute: no MIDI, callback still runs" },
        { "t:setValue(2.7, true)",                         2, 1,  2, "Characterization: Lua setters take int, decimals truncate" },
        { "t:setModulatorValue(50, false, false, false)", 50, 1, -1, "" },
        { "t:setModulatorValue(50, false, true, false)",  50, 1, 50, "" },
        { "t:setModulatorValue(50, true, false, false)",  50, 1, -1, "Characterization: vst flag ignored" },
        { "t:setModulatorValue(50, true, true, false)",   50, 1, 50, "" },
        { "t:setModulatorValue(50, false, false, true)",  50, 0, -1, "Characterization: ui = true skips the callback (intentional since 5.6.31, midibox 6e5a0b2)" },
        { "t:setModulatorValue(50, false, true, true)",   50, 0, 50, "Characterization: ui = true skips the callback" },
        { "t:setModulatorValue(50, true, false, true)",   50, 0, -1, "Characterization: ui = true skips the callback" },
        { "t:setModulatorValue(50, true, true, true)",    50, 0, 50, "Characterization: ui = true skips the callback" },
        { "t:setValueMapped(3, true)",                    -1, 1,  0, "Characterization: without a value map the value becomes -1" },
    };
}

/**
 * The value-path matrix. For each Lua setter and flag combination: the value afterwards, the
 * callback count (never inside the chunk: callbacks are dispatched asynchronously), the source
 * the callback sees (always Lua = 5), the MIDI sent to the device, and that the host is told
 * exactly once. The host notification does not depend on setModulatorValue's `vst` flag.
 */
TEST_F(LuaValues, lua_value_paths_set_value_callback_source_midi_and_host)
{
    loadValues();

    for (const auto& path : valuePaths)
    {
        SCOPED_TRACE(std::string(path.call) + "  " + path.note);
        resetTarget();

        ASSERT_TRUE(runWithModulators(path.call));
        EXPECT_EQ(luaNumber("_inChunk"), 0.0) << "no callback may run inside the calling chunk";

        const auto sends = pumpAndTakeDeviceSends();
        EXPECT_EQ(targetValue(), path.value);
        EXPECT_EQ(count("_targetCalls"), path.callbacks);
        if (path.callbacks > 0)
        {
            EXPECT_EQ(luaNumber("_targetSource"), (double) srcLua);
            EXPECT_EQ(luaNumber("_targetValue"), path.value);
        }
        EXPECT_EQ(host.count(1), 1) << "the host is notified once, whatever the flags";

        if (deviceObservable())
        {
            const std::vector<juce::MidiMessage> expected = path.midiValue < 0
                ? std::vector<juce::MidiMessage>{}
                : std::vector<juce::MidiMessage>{ targetCC(path.midiValue) };
            EXPECT_EQ(sends, expected);
        }
    }
}

/** Setting the value a modulator already has, without force, does nothing at all. */
TEST_F(LuaValues, unchanged_value_without_force_does_nothing)
{
    loadValues();
    ASSERT_TRUE(runWithModulators("t:setValue(33, false)"));
    pumpAndTakeDeviceSends();
    clearObservations();

    ASSERT_TRUE(runWithModulators("t:setValue(33, false)"));
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(count("_targetCalls"), 0);
    EXPECT_EQ(host.count(1), 0);
    EXPECT_TRUE(sends.empty());
}

/**
 * Setting an unchanged value with force (setValue(v, true), or setModulatorValue, which always
 * forces) resends the MIDI and re-runs the callback, but does not notify the host.
 *
 * Characterization: current behaviour. The forced same-value branch of
 * CtrlrModulatorProcessor::setValueGeneric does not store the new change source, so the callback
 * receives the source of the *previous* change: after a host change, a Lua call reports
 * source 1 (host), not 5 (Lua); after setModulatorValue(..., ui = true) it does not run at all.
 */
TEST_F(LuaValues, forced_unchanged_value_resends_midi_and_reuses_the_previous_source)
{
    loadValues();

    // previous change from Lua -> callback again, source Lua
    ASSERT_TRUE(runWithModulators("t:setValue(33, false)"));
    pumpAndTakeDeviceSends();
    clearObservations();
    ASSERT_TRUE(runWithModulators("t:setValue(33, true)"));
    auto sends = pumpAndTakeDeviceSends();
    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetSource"), (double) srcLua);
    EXPECT_EQ(host.count(1), 0) << "a forced unchanged value does not notify the host";
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(33) }));

    // previous change from the host -> the Lua call's callback reports source host
    setFromHost(1, 30);
    pumpAndTakeDeviceSends();
    clearObservations();
    ASSERT_TRUE(runWithModulators("t:setValue(30, true)"));
    sends = pumpAndTakeDeviceSends();
    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetSource"), (double) srcHost) << "stale source: the previous (host) change's";
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(30) }));

    // previous change with ui = true (source program) -> no callback for the forced Lua call
    ASSERT_TRUE(runWithModulators("t:setModulatorValue(20, false, false, true)"));
    pumpAndTakeDeviceSends();
    clearObservations();
    ASSERT_TRUE(runWithModulators("t:setValue(20, true)"));
    sends = pumpAndTakeDeviceSends();
    EXPECT_EQ(count("_targetCalls"), 0) << "stale source: program, which the callback gate skips";
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(20) }));
}

/**
 * A host parameter change (setParameter on the vst index) sets the value, runs the callback once
 * with source host (1), sends the MIDI, and is not echoed back to the host.
 */
TEST_F(LuaValues, host_parameter_change_runs_callback_with_source_host)
{
    loadValues();
    resetTarget();

    setFromHost(1, 30);
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(targetValue(), 30.0);
    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetSource"), (double) srcHost);
    EXPECT_EQ(host.count(1), 0) << "a host change is not reported back to the host";
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(30) }));
}

/**
 * A matching CC from the device sets the value, runs the callback once with source MIDI in (2),
 * notifies the host, and sends nothing back to the device.
 */
TEST_F(LuaValues, device_midi_input_runs_callback_with_source_midi_in)
{
    if (!deviceObservable())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

    loadValues();
    resetTarget();
    processor->prepareToPlay(44100, BLOCK_SIZE);

    // as in MidiSysex: pump the message loop, then a few blocks for the panel's input thread
    MockMidi::getInstance()->injectMidiInput(targetCC(20));
    for (int i = 0; i < 24; i++)
        juce::MessageManager::getInstance()->runDispatchLoopUntil(25);
    for (int i = 0; i < 4; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(23));
        processor->processBlock(buffer, midiMessages);
        midiMessages.clear();
    }
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(targetValue(), 20.0);
    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetSource"), (double) srcMidiIn);
    EXPECT_EQ(host.count(1), 1);
    EXPECT_TRUE(sends.empty()) << "incoming MIDI is not echoed to the device";
}

// ----------------------------------------------------------------------------------------------
// Ordering, coalescing, re-entrancy
// ----------------------------------------------------------------------------------------------

/** Callbacks for several modulators set in one chunk run after it, in the order of the sets. */
TEST_F(LuaValues, callbacks_run_after_the_chunk_in_the_order_values_were_set)
{
    loadValues();

    ASSERT_TRUE(runWithModulators("o:setValue(6, false)\nt:setValue(5, false)"));
    EXPECT_EQ(luaNumber("_inChunk"), 0.0);
    EXPECT_TRUE(luaIsNil("_log")) << "no callback ran inside the chunk";
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(luaString("_log"), "O6/5;T5/5;");
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ otherCC(6), targetCC(5) }));
}

/**
 * Setting one modulator twice in a chunk sends MIDI twice and notifies the host twice, but runs
 * the callback only once, with the last value (the async updates coalesce).
 *
 * Characterization: current behaviour.
 */
TEST_F(LuaValues, two_sets_in_one_chunk_run_the_callback_once_with_the_last_value)
{
    loadValues();

    ASSERT_TRUE(runWithModulators("t:setValue(3, false)\nt:setValue(4, false)"));
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetValue"), 4.0);
    EXPECT_EQ(host.count(1), 2);
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(3), targetCC(4) }));
}

/**
 * Re-entrancy: target's callback sets other, whose callback sets target back to the value it
 * already has. That second set is a no-op (unchanged, no force), so the chain ends: each
 * callback ran once, in order, and each modulator sent once.
 */
TEST_F(LuaValues, callback_chain_between_two_modulators_terminates)
{
    loadValues();

    ASSERT_TRUE(runWithModulators(
        "_targetHook = function(m, v, s) panel:getModulatorByName('other'):setValue(v, false) end\n"
        "_otherHook = function(m, v, s) panel:getModulatorByName('target'):setValue(v, false) end\n"
        "t:setValue(9, false)"));
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(luaString("_log"), "T9/5;O9/5;");
    EXPECT_EQ(targetValue(), 9.0);
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(9), otherCC(9) }));
}

/**
 * A callback that corrects its own modulator with setModulatorValue(n, false, false, true) (the
 * "clamp" pattern) does not re-enter: the callback ran once with the unclamped value and the
 * value ends clamped. With midi = false the device only ever saw the unclamped value.
 *
 * Characterization: current behaviour.
 */
TEST_F(LuaValues, callback_clamping_its_own_modulator_with_ui_flag_runs_once)
{
    loadValues();

    ASSERT_TRUE(runWithModulators(
        "_targetHook = function(m, v, s) if v > 100 then m:setModulatorValue(100, false, false, true) end end\n"
        "t:setValue(120, false)"));
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetValue"), 120.0);
    EXPECT_EQ(targetValue(), 100.0);
    EXPECT_EQ(host.count(1), 2);
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(120) }));
}

// ----------------------------------------------------------------------------------------------
// Bootstrap
// ----------------------------------------------------------------------------------------------

/**
 * Inside luaPanelLoaded, getBootstrapState() and isLoading() are true, and a value set there
 * keeps its value and sends its MIDI but never runs its callback (not inside, not afterwards).
 * Callbacks run again once loading returned and the message loop ran (see
 * is_loading_stays_true_until_the_message_loop_ran_after_load).
 *
 * Characterization: callbacks are skipped while bootstrapping (the callback gate in
 * CtrlrModulatorProcessor, 5.6.32 "stricter conditions");
 */
TEST_F(LuaValues, value_set_in_panel_loaded_keeps_value_and_midi_but_skips_callback)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);
    const auto loadSends = takeSendsAfterDelivery(150);

    EXPECT_EQ(luaNumber("_loadedBootstrap"), 1.0) << "getBootstrapState() inside luaPanelLoaded";
    EXPECT_EQ(luaNumber("_loadedIsLoading"), 1.0) << "isLoading() inside luaPanelLoaded";
    EXPECT_EQ(luaNumber("_loadedBootCalls"), 0.0);
    if (deviceObservable())
        EXPECT_EQ(loadSends, (std::vector<juce::MidiMessage>{ bootCC(7) })) << "the MIDI goes out during load";

    ASSERT_TRUE(runLua("_bootValue = panel:getModulatorByName('boot'):getModulatorValue()\n"
                       "panel:getModulatorByName('target'):setValue(11, false)"));
    const auto sends = pumpAndTakeDeviceSends();

    EXPECT_TRUE(luaIsNil("_bootCalls")) << "boot's callback never ran";
    EXPECT_EQ(luaNumber("_bootValue"), 7.0);
    EXPECT_EQ(count("_targetCalls"), 1) << "callbacks run once loading returned";
    if (deviceObservable())
        EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ targetCC(11) }));
}

/**
 * Right after loading, getBootstrapState() is already false while isLoading() stays true until
 * the message loop has run: bootstrapPanel() holds a post-bootstrap gate open across two
 * MessageManager::callAsync hops, so work queued during load drains first. It is not time-based.
 *
 * Loads with loadPanel() rather than loadLuaPanel(), which pumps the message loop (and so opens
 * the gate) before returning.
 */
TEST_F(LuaValues, is_loading_stays_true_until_the_message_loop_ran_after_load)
{
    allowAndRecordDeviceSends();
    panel = loadPanel("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    ASSERT_TRUE(runLua("_bootstrapAfter = panel:getBootstrapState() and 1 or 0\n"
                       "_loadingAfter = panel:isLoading() and 1 or 0"));

    EXPECT_EQ(luaNumber("_bootstrapAfter"), 0.0) << "getBootstrapState() once loading returned";
    EXPECT_EQ(luaNumber("_loadingAfter"), 1.0) << "isLoading() still true before the message loop ran";

    pumpInputThreads(50);
    ASSERT_TRUE(runLua("_loadingLater = panel:isLoading() and 1 or 0"));
    EXPECT_EQ(luaNumber("_loadingLater"), 0.0) << "isLoading() false once the message loop ran";
}

// ----------------------------------------------------------------------------------------------
// Multi-message (NRPN) bursts
// ----------------------------------------------------------------------------------------------

/**
 * NRPN modulators set alternately in one chunk reach the device as whole 4-CC bursts, one after
 * the other, with panelMidiGlobalDelay = 2. Pins the workaround for the zero-delay case below.
 */
TEST_F(LuaValues, nrpn_bursts_stay_contiguous_with_global_delay)
{
    if (!deviceObservable())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    loadValues();
    ASSERT_TRUE(runLua("panel:setPropertyInt('panelMidiGlobalDelay', 2)"));
    ASSERT_TRUE(runLua(setNrpnPairs(nrpnPairs)));

    EXPECT_EQ(takeSendsAfterDelivery(400), contiguousBursts(nrpnPairs));
}

/**
 * Desired behaviour, a known failure (Tests/known_failures_*.txt): the same bursts stay
 * contiguous at the default panelMidiGlobalDelay = 0. Today every buffer sent within the same
 * millisecond gets the same start time (CtrlrMIDIDevice::sendMidiBuffer), so bursts queued
 * together interleave CC by CC (99 99 98 98 6 6 38 38) and both data values land on the last
 * selected parameter.
 */
TEST_F(LuaValues, nrpn_bursts_stay_contiguous_at_zero_delay)
{
    if (!deviceObservable())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    loadValues();
    ASSERT_TRUE(runLua(setNrpnPairs(nrpnPairs)));

    EXPECT_EQ(takeSendsAfterDelivery(400), contiguousBursts(nrpnPairs));
}

// ----------------------------------------------------------------------------------------------
// Lookups
// ----------------------------------------------------------------------------------------------

/** getModulatorByName, getComponent and getLabel return nil (no error) for an unknown name. */
TEST_F(LuaValues, lookup_of_unknown_name_returns_nil)
{
    loadValues();
    ASSERT_TRUE(runLua("_mod = (panel:getModulatorByName('nope') == nil) and 1 or 0\n"
                       "_comp = (panel:getComponent('nope') == nil) and 1 or 0\n"
                       "_label = (panel:getLabel('nope') == nil) and 1 or 0"));
    EXPECT_EQ(luaNumber("_mod"), 1.0);
    EXPECT_EQ(luaNumber("_comp"), 1.0);
    EXPECT_EQ(luaNumber("_label"), 1.0);
}

/**
 * The typed getters return the component as its own class, and nil for a component of another
 * type. mod:getComponent() is the same component as panel:getComponent(name).
 */
TEST_F(LuaValues, typed_component_getters_return_their_class_or_nil)
{
    loadValues();
    ASSERT_TRUE(runLua(
        "_slider = class_info(panel:getComponent('target')).name\n"
        "_modComp = class_info(panel:getModulatorByName('target'):getComponent()).name\n"
        "_label = class_info(panel:getLabel('aLabel')).name\n"
        "_labelComponent = class_info(panel:getLabelComponent('aLabel')).name\n"
        "_combo = class_info(panel:getComboComponent('aCombo')).name\n"
        "_list = class_info(panel:getListBox('aList')).name\n"
        "_labelOfSlider = (panel:getLabel('target') == nil) and 1 or 0\n"
        "_comboOfLabel = (panel:getComboComponent('aLabel') == nil) and 1 or 0\n"
        "panel:getModulatorByName('target'):getComponent():setPropertyString('componentVisibleName', 'renamed')\n"
        "_sameComponent = panel:getComponent('target'):getProperty('componentVisibleName')"));

    EXPECT_EQ(luaString("_slider"), "CtrlrSlider");
    EXPECT_EQ(luaString("_modComp"), "CtrlrSlider");
    EXPECT_EQ(luaString("_label"), "CtrlrLabel");
    EXPECT_EQ(luaString("_labelComponent"), "CtrlrLabel");
    EXPECT_EQ(luaString("_combo"), "CtrlrCombo");
    EXPECT_EQ(luaString("_list"), "CtrlrListBox");
    EXPECT_EQ(luaNumber("_labelOfSlider"), 1.0);
    EXPECT_EQ(luaNumber("_comboOfLabel"), 1.0);
    EXPECT_EQ(luaString("_sameComponent"), "renamed");
}

/**
 * getModulatorWithProperty finds a modulator by a property value given as a string (the
 * string.format("%d", i) form) or as a number. On a miss it returns a placeholder modulator,
 * not nil; the placeholder has no component.
 *
 * Characterization: the miss is not nil (CtrlrManager's invalid modulator).
 */
TEST_F(LuaValues, modulator_with_property_finds_by_string_or_number_and_misses_to_a_placeholder)
{
    loadValues();
    ASSERT_TRUE(runLua(
        "_byString = panel:getModulatorWithProperty('vstIndex', string.format('%d', 2)):getVstIndex()\n"
        "_byNumber = panel:getModulatorWithProperty('vstIndex', 2):getVstIndex()\n"
        "local miss = panel:getModulatorWithProperty('vstIndex', '99')\n"
        "_missIsNil = (miss == nil) and 1 or 0\n"
        "_missComponentIsNil = (miss:getComponent() == nil) and 1 or 0"));

    EXPECT_EQ(luaNumber("_byString"), 2.0);
    EXPECT_EQ(luaNumber("_byNumber"), 2.0);
    EXPECT_EQ(luaNumber("_missIsNil"), 0.0);
    EXPECT_EQ(luaNumber("_missComponentIsNil"), 1.0);
}

/**
 * Wiring check for the suite: the fixture loads, a chunk runs in the panel's Lua state, and a
 * value set from Lua reads back from Lua.
 */
TEST_F(LuaValues, lua_chunk_sets_and_reads_back_a_modulator_value)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local m = panel:getModulatorByName('target')\n"
                       "m:setValue(42, true)\n"
                       "_readBack = m:getValueInt()"));
    EXPECT_EQ(luaNumber("_readBack"), 42.0);
}
