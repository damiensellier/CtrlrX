#include "test_ProcessorFixture.h"

#include "CtrlrManager.h"
#include "CtrlrModulator.h"
#include "CtrlrPanel.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

/**
 * MIDI routing tests (host<->device<->host).
 *
 * These pin down the deterministic invariants of the MIDI routing:
 *  - message COUNT and ONCE-ness (no duplicate output),
 *  - which sink (host buffer vs. device) a message reaches,
 *  - pause-out / pause-in gating.
 *
 * Most are characterization tests (locking down behavior we believe correct). A small number are
 * *specification* tests asserting the DESIRED behavior for the suspected duplicate-output bug
 * (see CtrlrProcessor.cpp:179 unconditional echo + CtrlrPanel::sendMidi host fan-out).
 *
 * Accepted failures are listed in Tests/known_failures.txt.
 *
 * Seam: black-box only. We drive processBlock / setParameter / the mock's injectMidiInput, and
 * observe the returned MidiBuffer (host output) plus the StrictMock<MockMidi> device.
 */

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Invoke;

// Shared helpers (allowAndRecordDeviceSends / countDeviceSends / loadPanel / runHostBlocks /
// pumpInputThreads) and count_equal now live in test_ProcessorFixture.h.
class MidiRouting : public ProcessorInstance {};

/**
 * H2H, single panel: one host CC must reach the host output exactly once.
 * SPEC: desired once-ness in the face of the unconditional echo at CtrlrProcessor.cpp:179.
 * A CC that does NOT match the panel modulator is used so only the thru/echo path is exercised.
 */
TEST_F(MidiRouting, h2h_single_panel_host_cc_reaches_host_once) {
	allowAndRecordDeviceSends();
	ASSERT_NE(loadPanel("fixture_host2host_cc.panel"), nullptr);

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 20, 100); // non-matching
	juce::MidiBuffer in;
	in.addEvent(cc, 0);

	auto host = runHostBlocks(in);
	EXPECT_EQ(count_equal(host, cc), 1) << "host CC should be forwarded to host exactly once";
}

/**
 * H2H, two panels both with H2H + outputToHost: one host CC must reach the host ONCE, not
 * once-per-panel. SPEC.
 */
TEST_F(MidiRouting, h2h_multi_panel_host_cc_reaches_host_once) {
	allowAndRecordDeviceSends();
	ASSERT_NE(loadPanel("fixture_host2host_cc.panel"), nullptr);
	ASSERT_NE(loadPanel("fixture_host2host_cc.panel"), nullptr);
	ASSERT_EQ(processor->getManager().getNumPanels(), 2);

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 20, 100); // non-matching
	juce::MidiBuffer in;
	in.addEvent(cc, 0);

	auto host = runHostBlocks(in);
	EXPECT_EQ(count_equal(host, cc), 1) << "host CC must reach host once, not once-per-panel";
}

/**
 * H2D: a host CC reaches the device exactly once, and is NOT additionally (wrongly) echoed to the
 * host. fixture_host2device_cc.panel has H2D + H2H + outputToHost on, so the desired host count is
 * exactly one (the H2H thru), while today the unconditional echo + sendMidi's host fan-out produce
 * a duplicate. SPEC (matches the already-known-failing Host2Device case).
 */
TEST_F(MidiRouting, h2d_host_cc_reaches_device_once_and_host_once) {
	allowAndRecordDeviceSends();
	ASSERT_NE(loadPanel("fixture_host2device_cc.panel"), nullptr);

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 20, 100); // non-matching
	juce::MidiBuffer in;
	in.addEvent(cc, 0);

	auto host = runHostBlocks(in);

	EXPECT_EQ(countDeviceSends(cc), 1) << "H2D should send the host CC to the device once";
	EXPECT_EQ(count_equal(host, cc), 1) << "host CC must not be duplicated on the host output";
}

/**
 * D2H thru: a CC injected from the device is forwarded to the host output exactly once.
 * Uses a non-matching CC so only the D2H thru path is exercised (no modulator send). Characterization.
 *
 * TIMING (do not "simplify" the loop below into pump-then-drain):
 * This works around a MIDI handling bug that will be addressed later.
 * CtrlrProcessor::addMidiToOutputQueue stamps queued messages with their sample position (1 or 2),
 * which juce::MidiMessageCollector reads as *seconds* and compares against
 * Time::getMillisecondCounterHiRes() * 0.001 -- so they sit at a hugely negative sample position.
 * Whenever the gap between two removeNextBlockOfMessages() calls exceeds (numSamples << 5) samples
 * (743ms at 44.1kHz / 1024), the collector sets startSample > 0, skips past every such message via
 * findNextSamplePosition(), and then clear()s the queue -- destroying them permanently.
 *
 * Interleaving processBlock with the pump keeps every gap at ~25ms, far below the threshold.
 * Production code is left as-is deliberately.
 */
TEST_F(MidiRouting, d2h_injected_cc_reaches_host_once) {
	if (!midi_mock.hasSubsystemMock())
		GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

	allowAndRecordDeviceSends();
	CtrlrPanel *panel = loadPanel("fixture_device_input_cc.panel");
	ASSERT_NE(panel, nullptr);
	processor->prepareToPlay(44100, BLOCK_SIZE);

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 20, 100); // non-matching
	MockMidi::getInstance()->injectMidiInput(cc);

	// Pump the message loop and drain host output in the same loop, so no two processBlock calls
	// are more than ~25ms apart. ~1.2s total covers the panel input thread's 500ms process() tick
	// (the D2H thru is queued from that thread, not from processBlock).
	std::vector<juce::MidiMessage> host;
	for (int i = 0; i < 48; i++) {
		juce::MessageManager::getInstance()->runDispatchLoopUntil(25);

		midiMessages.clear();
		processor->processBlock(buffer, midiMessages);
		for (const auto meta : midiMessages)
			host.push_back(meta.getMessage());
	}

	EXPECT_EQ(count_equal(host, cc), 1) << "device-injected CC should reach host once via D2H";
}

/**
 * Device input that matches a modulator's comparator updates that modulator's parameter.
 * Characterization of the input-compare path on the device side.
 */
TEST_F(MidiRouting, device_input_matching_cc_updates_modulator) {
	if (!midi_mock.hasSubsystemMock())
		GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

	allowAndRecordDeviceSends();
	CtrlrPanel *panel = loadPanel("fixture_device_input_cc.panel");
	ASSERT_NE(panel, nullptr);
	processor->prepareToPlay(44100, BLOCK_SIZE);

	// Matching CC: channel 4 (panelMidiOutputChannelDevice), controller 3 (midiMessageCtrlrNumber).
	EXPECT_GT(openInputCalls, 0) << "panel should have opened the input device on load";

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(4, 3, 100);
	MockMidi::getInstance()->injectMidiInput(cc);
	pumpInputThreads();
	// let any posted change messages settle:
	for (int i = 0; i < 4; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(23));
		processor->processBlock(buffer, midiMessages);
		midiMessages.clear();
	}

	EXPECT_FLOAT_EQ(processor->getParameter(1), 100.0f / 127.0f)
		<< "modulator parameter should track the matched device CC value";
}

/**
 * Pause-out gating: with panelMidiPauseOut set, a parameter change produces NO device send.
 * Deterministic, no timing involved. Characterization.
 */
TEST_F(MidiRouting, pause_out_blocks_device_send_on_value_change) {
	// Allow device open (happens during load), but set NO sendMidiEvent expectation: with the
	// StrictMock, any device send while paused-out fails the test. Expectations must be in place
	// before loadPanel(), which opens the output device.
	if (midi_mock.hasSubsystemMock())
		EXPECT_CALL(midi_mock, openOutput(_, _)).Times(AnyNumber());

	CtrlrPanel *panel = loadPanel("fixture_host2host_cc.panel");
	ASSERT_NE(panel, nullptr);

	panel->setProperty(Ids::panelMidiPauseOut, true, false);
	ASSERT_TRUE(panel->isMidiOutPaused());

	processor->prepareToPlay(44100, BLOCK_SIZE);
	processor->setParameter(1, 0.5);
	processor->processBlock(buffer, midiMessages);
}

/**
 * Pause-in gating: with panelMidiPauseIn set, host input is not echoed to the host (the panel's
 * queueMessageForHostOutput early-returns while paused). Characterization.
 *
 * Note: the unconditional echo at CtrlrProcessor.cpp:179 is NOT gated by pause-in, so today the
 * host CC may still appear. This asserts the DESIRED gating; allowlisted if it fails. SPEC.
 */
TEST_F(MidiRouting, pause_in_blocks_host_echo) {
	allowAndRecordDeviceSends();
	CtrlrPanel *panel = loadPanel("fixture_host2host_cc.panel");
	ASSERT_NE(panel, nullptr);

	panel->setProperty(Ids::panelMidiPauseIn, true, false);
	ASSERT_TRUE(panel->isMidiInPaused());

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 20, 100);
	juce::MidiBuffer in;
	in.addEvent(cc, 0);

	auto host = runHostBlocks(in);
	EXPECT_EQ(count_equal(host, cc), 0) << "paused-in panel should not echo host input back to host";
}

/**
 * Program change fan-out: setting panelMidiProgram triggers a bank-select + program-change burst
 * that fans out to the device (and, with outputToHost, to the host) -- each program change once.
 * This fires outside processBlock (a ValueTree property change), so we wait for the async device
 * delivery and drain the host output afterwards. P1.
 */
TEST_F(MidiRouting, program_change_fans_out_to_device_and_host_once) {
	allowAndRecordDeviceSends();
	CtrlrPanel *panel = loadPanel("fixture_program_change.panel");
	ASSERT_NE(panel, nullptr);
	processor->prepareToPlay(44100, BLOCK_SIZE);

	// Channel 4 == panelMidiOutputChannelDevice; banks are 0 in the fixture.
	const juce::MidiMessage pc = juce::MidiMessage::programChange(4, 7);

	panel->setProperty(Ids::panelMidiProgram, 7, false); // triggers sendMidiProgramChange()

	// Drain host output (program change is queued to the host collector) and let the device's
	// background thread flush.
	std::vector<juce::MidiMessage> host;
	for (int i = 0; i < 6; i++) {
		midiMessages.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(23));
		processor->processBlock(buffer, midiMessages);
		for (const auto meta : midiMessages)
			host.push_back(meta.getMessage());
	}

	EXPECT_EQ(countDeviceSends(pc), 1) << "program change should reach the device exactly once";
	EXPECT_EQ(count_equal(host, pc), 1) << "program change should reach the host exactly once";
}

/**
 * D2D: a CC injected from the device is forwarded to the device output exactly once. Uses a
 * non-matching CC (so no modulator send) and toggles D2D on / D2H off in-test. P1.
 */
TEST_F(MidiRouting, d2d_injected_cc_forwarded_to_device_once) {
	if (!midi_mock.hasSubsystemMock())
		GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

	allowAndRecordDeviceSends();
	CtrlrPanel *panel = loadPanel("fixture_device_input_cc.panel");
	ASSERT_NE(panel, nullptr);
	panel->setProperty(Ids::panelMidiThruD2D, true, false);
	panel->setProperty(Ids::panelMidiThruD2H, false, false);
	processor->prepareToPlay(44100, BLOCK_SIZE);

	const juce::MidiMessage cc = juce::MidiMessage::controllerEvent(1, 20, 100); // non-matching
	MockMidi::getInstance()->injectMidiInput(cc);
	pumpInputThreads();

	EXPECT_EQ(countDeviceSends(cc), 1) << "D2D should forward the device CC to the device output once";
}
