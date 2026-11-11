#include "test_ProcessorFixture.h"

#include "CtrlrManager.h"
#include "CtrlrPanel.h"
#include "CtrlrModulator.h"
#include "CtrlrLuaManager.h"

#include <chrono>
#include <thread>
#include <vector>

/**
 * MIDI handling in Lua scripts.
 *
 * Pins down the Lua-side MIDI invariants at the same black-box seam as the C++ MIDI suites 
 * (processBlock / setParameter / injectMidiInput in, * StrictMock device + host MidiBuffer out). 
 * Panels routinely receive and send MIDI from Lua.
 *
 * Two observation strategies (no production code is touched):
 *  - RECEIVE callbacks (midiReceived / multiMidiReceived) bump an invocation counter and write
 *    it to a modulator value; the test reads getParameter() and asserts it equals 1. This
 *    distinguishes "fired once" from "fired twice".
 *  - SEND from Lua is triggered through a modulator's luaModulatorValueChange (the "Request
 *    Program" idiom) via setParameter(); the test counts the resulting message on the device.
 *
 * Accepted failures are listed in Tests/known_failures.txt.
 *
 * Fixtures (generated minimal panels, see Tests/fixtures/fixture_lua_*.panel):
 *  - fixture_lua_midi_received : luaPanelMidiReceived -> counter + round-trip sentinel
 *  - fixture_lua_multi_received: luaPanelMidiMultiReceived -> counter
 *  - fixture_lua_send          : modulator luaModulatorValueChange -> the send variants
 *  - fixture_lua_nrpn          : NRPN via Lua setValue (vstIndex 2) + hand-rolled burst (vstIndex 3)
 */

class MidiLua : public ProcessorInstance
{
protected:
    // Drain host output across a few idle blocks (queued sends flow through the MidiMessageCollector).
    std::vector<juce::MidiMessage> drainHostOutput(int blocks = 6)
    {
        std::vector<juce::MidiMessage> host;
        for (int i = 0; i < blocks; i++)
        {
            midiMessages.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(23));
            processor->processBlock(buffer, midiMessages);
            for (const auto meta : midiMessages)
                host.push_back(meta.getMessage());
        }
        return host;
    }

    // Trigger a Lua send: changing the modulator value fires its luaModulatorValueChange method.
    // The callback runs from the modulator's async update on the message thread, so the loop must
    // be pumped after setParameter for the Lua method to actually execute.
    void triggerLua(int vstIndex, float value)
    {
        processor->prepareToPlay(44100, BLOCK_SIZE);
        processor->setParameter(vstIndex, value);
        processor->processBlock(buffer, midiMessages);
        midiMessages.clear();
        pumpInputThreads(300);
    }
};

// ----------------------------------------------------------------------------------------------
// RECEIVE side
// ----------------------------------------------------------------------------------------------

/**
 * R1: a device-injected MIDI message invokes the Lua midiReceived callback exactly once.
 * A NON-matching CC is used so only the panel-level callback runs (no modulator comparator match);
 * the callback writes its invocation count to the modulator, so getParameter == 1 proves once-ness.
 */
TEST_F(MidiLua, device_input_invokes_lua_midiReceived_once)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_midi_received.panel"), nullptr);
    processor->prepareToPlay(44100, BLOCK_SIZE);

    MockMidi::getInstance()->injectMidiInput(juce::MidiMessage::controllerEvent(1, 20, 100)); // non-matching
    pumpInputThreads();
    drainHostOutput();

    EXPECT_FLOAT_EQ(processor->getParameter(1), 1.0f / 127.0f)
        << "Lua midiReceived should fire exactly once per device message (2/127 would mean a duplicate)";
}

/**
 * R2: host MIDI input invokes the Lua midiReceived callback exactly once. SPEC -- host input routes
 * through the panel processor's processBlock rather than the input-thread comparator, so if host
 * input never reaches panelReceivedMidi this documents a gap (allowlisted) rather than a hard fail.
 */
TEST_F(MidiLua, host_input_invokes_lua_midiReceived_once)
{
    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_midi_received.panel"), nullptr);

    juce::MidiBuffer in;
    in.addEvent(juce::MidiMessage::controllerEvent(1, 20, 100), 0); // non-matching
    runHostBlocks(in);
    pumpInputThreads(500); // host input is compared on the panel's input thread, asynchronously

    EXPECT_FLOAT_EQ(processor->getParameter(1), 1.0f / 127.0f)
        << "Lua midiReceived should fire once for host input too";
}

/**
 * R3: assembled multi-message input invokes the Lua multiMidiReceived callback once. SPEC/best-effort
 * -- no shipped panel uses multiMidiReceived, so the multi-message assembly path is the least
 * exercised; if it does not fire under a plain injected message this documents the gap.
 */
TEST_F(MidiLua, device_input_invokes_lua_multiMidiReceived_once)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_multi_received.panel"), nullptr);
    processor->prepareToPlay(44100, BLOCK_SIZE);

    // Inject the NRPN sequence that matches the Multi modulator's input mask (channel 2,
    // param 10 -> CC99=0/CC98=10, data -> CC6/CC38). The reassembled multi message drives
    // multiMidiReceived, whose Lua method re-sends a distinct sentinel (CC ch8/cc88/val88).
    MockMidi::getInstance()->injectMidiInput(juce::MidiMessage::controllerEvent(2, 99, 0));
    MockMidi::getInstance()->injectMidiInput(juce::MidiMessage::controllerEvent(2, 98, 10));
    MockMidi::getInstance()->injectMidiInput(juce::MidiMessage::controllerEvent(2, 6, 0));
    MockMidi::getInstance()->injectMidiInput(juce::MidiMessage::controllerEvent(2, 38, 64));
    pumpInputThreads(800);

    EXPECT_EQ(countDeviceSends(juce::MidiMessage::controllerEvent(8, 88, 88)), 1)
        << "a matched multi (NRPN) input should invoke Lua multiMidiReceived exactly once";
}

// ----------------------------------------------------------------------------------------------
// SEND side (triggered via modulator luaModulatorValueChange -> doSend)
// ----------------------------------------------------------------------------------------------

/**
 * S1: panel:sendMidiMessageNow(obj) from Lua (object built from a MemoryBlock, not the string
 * parser) reaches the device exactly once.
 */
TEST_F(MidiLua, lua_sendMidiMessageNow_object_reaches_device_once)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_send.panel"), nullptr);
    triggerLua(1, 0.5f);

    auto sends = takeSendsAfterDelivery();
    EXPECT_EQ(count_equal(sends, juce::MidiMessage::controllerEvent(1, 12, 64)), 1)
        << "Lua sendMidiMessageNow(object) should reach the device exactly once";
}

/**
 * S2: panel:sendMidi(msg, delay) from Lua reaches the device exactly once and is not duplicated on
 * the host output. SPEC for the no-host-duplicate concern.
 */
TEST_F(MidiLua, lua_sendMidi_reaches_device_once_not_duplicated_to_host)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_send.panel"), nullptr);
    triggerLua(1, 0.5f);

    const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 13, 65);
    auto host = drainHostOutput();
    auto sends = takeSendsAfterDelivery();

    EXPECT_EQ(count_equal(sends, cc), 1) << "Lua sendMidi should reach the device exactly once";
    EXPECT_LE(count_equal(host, cc), 1) << "Lua sendMidi must not be duplicated on the host output";
}

/**
 * S3: the hex-string overload panel:sendMidiMessageNow("B0 0E 42") produces the correct CC bytes
 * (ch1, controller 14, value 66) and reaches the device once. SPEC -- asserts the desired bytes;
 * stays red until the CtrlrMidiMessage(String) constructor parses correctly.
 */
TEST_F(MidiLua, lua_sendMidiMessageNow_hex_string_cc_correct_bytes)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_send.panel"), nullptr);
    triggerLua(1, 0.5f);

    auto sends = takeSendsAfterDelivery();
    EXPECT_EQ(count_equal(sends, juce::MidiMessage::controllerEvent(1, 14, 66)), 1)
        << "hex-string overload should send CC ch1/cc14/val66 exactly once (B0 0E 42)";
}

/**
 * SX-i: sysex sent from Lua via a CtrlrMidiMessage object (built from a MemoryBlock) reaches the
 * device once with the correct bytes (F0 7E 41 F7). Baseline that does NOT use the string parser.
 */
TEST_F(MidiLua, lua_sysex_object_correct_bytes_once)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_send.panel"), nullptr);
    triggerLua(1, 0.5f);

    const juce::uint8 raw[] = { 0xF0, 0x7E, 0x41, 0xF7 };
    const juce::MidiMessage sysex(raw, (int) sizeof(raw));
    auto sends = takeSendsAfterDelivery();
    EXPECT_EQ(count_equal(sends, sysex), 1)
        << "Lua sysex via object should reach the device exactly once with correct bytes";
}

/**
 * SX-ii: sysex sent from Lua via the hex-string overload (F0 7D 40 F7). SPEC -- contrasts with SX-i:
 * both target identical expected bytes, so a green SX-i and red SX-ii localizes the bug to the
 * CtrlrMidiMessage(String) constructor.
 */
TEST_F(MidiLua, lua_sysex_hex_string_correct_bytes_once)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_send.panel"), nullptr);
    triggerLua(1, 0.5f);

    const juce::uint8 raw[] = { 0xF0, 0x7D, 0x40, 0xF7 };
    const juce::MidiMessage sysex(raw, (int) sizeof(raw));
    auto sends = takeSendsAfterDelivery();
    EXPECT_EQ(count_equal(sends, sysex), 1)
        << "Lua sysex via hex string should reach the device exactly once with correct bytes";
}

/**
 * SX-iii: a 301-byte sysex sent from Lua reaches the device as one message with
 * all its bytes. On macOS JUCE splits a sysex this long over several MIDISendEventList calls; the
 * CoreMIDI mock used to drop it (it reassembled each call on its own). Same dump as the probe's
 * fixture_probe_midi.panel (self-test scenario 28).
 */
TEST_F(MidiLua, long_sysex_reaches_device_whole)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    CtrlrPanel* panel = loadPanel("fixture_lua_send.panel");
    ASSERT_NE(panel, nullptr);

    ASSERT_TRUE(panel->getCtrlrLuaManager().runCode(
        "local bytes = {0xF0, 0x7D, 0x10}\n"
        "for i = 1, 297 do bytes[#bytes + 1] = i % 128 end\n"
        "bytes[#bytes + 1] = 0xF7\n"
        "panel:sendMidiMessageNow(CtrlrMidiMessage(MemoryBlock(bytes)))\n",
        "long_sysex"));

    std::vector<juce::uint8> raw = { 0xF0, 0x7D, 0x10 };
    for (int i = 1; i <= 297; i++)
        raw.push_back((juce::uint8) (i % 128));
    raw.push_back(0xF7);
    ASSERT_EQ(raw.size(), 301u);
    const juce::MidiMessage sysex(raw.data(), (int) raw.size());

    auto sends = takeSendsAfterDelivery(500);
    ASSERT_EQ(sends.size(), 1u) << "the device should get exactly one message (the whole sysex)";
    EXPECT_EQ(sends[0].getRawDataSize(), 301);
    EXPECT_EQ(count_equal(sends, sysex), 1) << "the 301-byte sysex should arrive whole, bytes in order";
}

// ----------------------------------------------------------------------------------------------
// NRPN from Lua
// ----------------------------------------------------------------------------------------------

/**
 * NR-a: driving an NRPN-output modulator's value from Lua (mod:setValue) emits the NRPN header
 * (CC99/98) then data (CC6/38) once, and a second Lua-driven change streams data only (header
 * latched). Same once-ness/latch invariant as the C++ NRPN path, but triggered through Lua.
 */
TEST_F(MidiLua, lua_setvalue_on_nrpn_modulator_emits_header_once_then_data)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_nrpn.panel"), nullptr);

    // vstIndex 2 is the trigger modulator whose Lua method calls nrpnModulator:setValue(value, true).
    triggerLua(2, 0.4f);
    EXPECT_EQ(controllerNumbers(takeSendsAfterDelivery()), (std::vector<int>{99, 98, 6, 38}))
        << "first Lua-driven NRPN should emit header CC99/CC98 then data CC6/CC38";

    processor->setParameter(2, 0.8f);
    processor->processBlock(buffer, midiMessages);
    pumpInputThreads(300);
    EXPECT_EQ(controllerNumbers(takeSendsAfterDelivery()), (std::vector<int>{6, 38}))
        << "second Lua-driven NRPN should stream data only (header latched)";
}

/**
 * NR-b: a hand-rolled NRPN CC burst built in Lua (MemoryBlock objects) and sent via sendMidi arrives
 * on the wire in order (CC99/98/6/38, channel 5), each once. Isolated from the string parser.
 */
TEST_F(MidiLua, lua_hand_rolled_nrpn_burst_in_order_once)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device sends cannot be observed";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_nrpn.panel"), nullptr);

    triggerLua(3, 0.5f); // vstIndex 3 -> sendRawNrpn

    // keep only the channel-5 controllers the hand-rolled burst produced
    std::vector<int> burst;
    for (const auto& m : takeSendsAfterDelivery())
        if (m.isController() && m.getChannel() == 5)
            burst.push_back(m.getControllerNumber());

    EXPECT_EQ(burst, (std::vector<int>{99, 98, 6, 38}))
        << "hand-rolled NRPN burst should arrive in order, each once";
}

// ----------------------------------------------------------------------------------------------
// Round-trip
// ----------------------------------------------------------------------------------------------

/**
 * RT: device input -> Lua midiReceived -> panel:sendMidi(sentinel) reaches the device exactly once.
 * SPEC -- the real-world "respond to incoming MIDI" idiom; a duplicate midiReceived would emit the
 * sentinel twice. Sentinel is a distinct CC (ch7/cc77/val77) so it cannot collide with the input.
 */
TEST_F(MidiLua, roundtrip_device_input_triggers_single_lua_send)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

    allowAndRecordDeviceSends();
    ASSERT_NE(loadPanel("fixture_lua_midi_received.panel"), nullptr);
    processor->prepareToPlay(44100, BLOCK_SIZE);

    MockMidi::getInstance()->injectMidiInput(juce::MidiMessage::controllerEvent(1, 20, 100)); // non-matching
    pumpInputThreads();

    EXPECT_EQ(countDeviceSends(juce::MidiMessage::controllerEvent(7, 77, 77)), 1)
        << "a single device input should produce exactly one Lua-generated sentinel send";
}
