#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>

#include "gtest/gtest.h"

#include "JuceHeader.h"
#include <CtrlrProcessor.h>
#include <CtrlrManager.h>
#include <CtrlrMIDIDeviceManager.h>

#include "mock_MidiDevice.h"

class CtrlrPanel;

#define BLOCK_SIZE 1024

// byte-wise MidiMessage equality, used throughout the MIDI test suites.
namespace juce {
    inline bool operator==(const MidiMessage& lhs, const MidiMessage& rhs)
    {
        bool equal_data = true;
        for (size_t idx = 0; idx < (size_t) lhs.getRawDataSize() && idx < (size_t) rhs.getRawDataSize(); idx++)
            equal_data &= (lhs.getRawData()[idx] == rhs.getRawData()[idx]);
        return (lhs.getRawDataSize() == rhs.getRawDataSize()) && equal_data;
    }
    // NOTE: PrintTo(const MidiMessage&, std::ostream*) is defined in mock_MidiDevice.h (included
    // above), where it precedes the MockMidi class so gmock's argument printer picks it up.
}


class ProcessorInstance : public testing::Test {
protected:
    ::testing::StrictMock<MockMidi> midi_mock;

    std::shared_ptr<CtrlrProcessor> processor;
    juce::AudioSampleBuffer buffer;
    juce::MidiBuffer midiMessages;

    ProcessorInstance() : buffer(2, BLOCK_SIZE) {}

    virtual void SetUp() override {
        midi_mock.setDefaultBehavior();
        
        // needed for some of JUCE's asserts:
        MessageManager::getInstance()->setCurrentThreadAsMessageThread();

        processor = std::make_shared<CtrlrProcessor>();

        // JUCE 8 enumerates MIDI devices asynchronously: the device cache is only populated once
        // the (mocked) ALSA sequencer thread delivers a port-change event and the resulting
        // AsyncUpdater is dispatched on the message thread. CtrlrProcessor's constructor already
        // ran refreshDevices() against an empty cache, so pump the message loop until the mock
        // devices appear, then refresh CtrlrX's device list again.
        if (midi_mock.hasSubsystemMock())
        {
            for (int i = 0; i < 40 && juce::MidiOutput::getAvailableDevices().isEmpty(); ++i)
                juce::MessageManager::getInstance()->runDispatchLoopUntil(25);

            processor->getManager().getCtrlrMIDIDeviceManager().refreshDevices();
        }

        // initialize buffer with non-zero 'audio'
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            buffer.setSample(0, i, 1.0f * i);
            buffer.setSample(1, i, 2.0f * i);
        }
        // clear the midi buffer
        midiMessages.clear();
        std::cout << "-------- setup done --------" << std::endl;
    }
    virtual void TearDown() override {
        std::cout << "-------- teardown starting --------" << std::endl;
        midiMessages.clear();
        // Destroy the processor here, not in the fixture's destructor: members are destroyed in
        // reverse order, so deviceSends would go before `processor` while JUCE's MidiOutput
        // thread may still be delivering a send into it (use-after-free that corrupts the heap
        // for the next test in the same process). Destroying the processor joins that thread.
        processor.reset();
    }

    void expectNoMidiMessagesInBuffer(std::string message);

    void testMidiBlockProcessing(
        const juce::MidiBuffer messages_to_send, 
        const std::function <void (std::string)>& function_to_call_after_idle_processing = nullptr,
        int num_iterations_to_idle = 3);
    void processBlockWithoutMidiMessages(
        std::string message = "",
        const std::function <void (std::string)>& function_to_call_after_processing = nullptr);

    // ---- Shared MIDI test helpers (used across the MIDI routing / sysex / lua suites) ----
    //
    // Device output recorded from the StrictMock so tests can count/inspect it freely.
    // sendMidiEvent fires on JUCE's MidiOutput background thread, so the vector is guarded.
    std::mutex sendsMutex;
    std::vector<juce::MidiMessage> deviceSends; // guarded by sendsMutex (written from MIDI out thread)
    int openInputCalls = 0;

    // Permit (and record) any number of device sends / opens. Use when a test asserts on the
    // recorded vector rather than on exact EXPECT_CALL counts. Expectations must be installed
    // before loadPanel(), which opens the devices.
    void allowAndRecordDeviceSends()
    {
        if (midi_mock.hasSubsystemMock())
        {
            EXPECT_CALL(midi_mock, openOutput(::testing::_, ::testing::_)).Times(::testing::AnyNumber());
            EXPECT_CALL(midi_mock, openInput(::testing::_, ::testing::_))
                .Times(::testing::AnyNumber())
                .WillRepeatedly(::testing::Invoke([this](int, int) { openInputCalls++; }));
            EXPECT_CALL(midi_mock, sendMidiEvent(::testing::_, ::testing::_, ::testing::_))
                .Times(::testing::AnyNumber())
                .WillRepeatedly(::testing::Invoke([this](int, int, juce::MidiMessage m) {
                    std::lock_guard<std::mutex> l(sendsMutex);
                    deviceSends.push_back(m);
                }));
        }
    }

    int countDeviceSends(const juce::MidiMessage& needle)
    {
        std::lock_guard<std::mutex> l(sendsMutex);
        int n = 0;
        for (const auto& m : deviceSends)
            if (m == needle)
                n++;
        return n;
    }

    // Give JUCE's MidiOutput background thread time to flush queued messages, then take & clear
    // the recorded device sends.
    std::vector<juce::MidiMessage> takeSendsAfterDelivery(int waitMs = 250)
    {
        for (int waited = 0; waited < waitMs; waited += 20)
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        std::lock_guard<std::mutex> l(sendsMutex);
        auto copy = deviceSends;
        deviceSends.clear();
        return copy;
    }

    // Pump both the JUCE message loop (delivers injected device input to handleMIDIFromDevice) and
    // give the per-panel input thread time to run its process() loop (comparator + D2H/D2D).
    void pumpInputThreads(int milliseconds = 600)
    {
        const int step = 25;
        for (int waited = 0; waited < milliseconds; waited += step)
            juce::MessageManager::getInstance()->runDispatchLoopUntil(step);
    }

    // Run one processBlock carrying `input`, then `idleBlocks` empty blocks (each after a ~one-block
    // sleep so the MidiMessageCollector flushes), collecting every host-output message produced.
    std::vector<juce::MidiMessage> runHostBlocks(const juce::MidiBuffer& input, int idleBlocks = 4)
    {
        std::vector<juce::MidiMessage> out;

        processor->prepareToPlay(44100, BLOCK_SIZE);

        midiMessages.clear();
        for (const auto meta : input)
            midiMessages.addEvent(meta.getMessage(), meta.samplePosition);

        processor->processBlock(buffer, midiMessages);
        for (const auto meta : midiMessages)
            out.push_back(meta.getMessage());

        for (int i = 0; i < idleBlocks; i++)
        {
            midiMessages.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(23));
            processor->processBlock(buffer, midiMessages);
            for (const auto meta : midiMessages)
                out.push_back(meta.getMessage());
        }
        return out;
    }

    // Load a panel from Tests/fixtures (copied into <build>/Tests/fixtures/) and return the newest panel.
    CtrlrPanel* loadPanel(const std::string& file)
    {
        EXPECT_NO_THROW(processor->openFileFromCli(
            juce::File::getCurrentWorkingDirectory().getChildFile("fixtures/" + file)));
        const int n = processor->getManager().getNumPanels();
        return n > 0 ? processor->getManager().getPanel(n - 1) : nullptr;
    }

    static std::vector<int> controllerNumbers(const std::vector<juce::MidiMessage>& msgs)
    {
        std::vector<int> cc;
        for (const auto& m : msgs)
            if (m.isController())
                cc.push_back(m.getControllerNumber());
        return cc;
    }
};

// Count how many messages in `haystack` byte-equal `needle` (uses the juce::operator== below).
inline int count_equal(const std::vector<juce::MidiMessage>& haystack, const juce::MidiMessage& needle)
{
    int n = 0;
    for (const auto& m : haystack)
        if (m == needle)
            n++;
    return n;
}

/**
 * parameterized ProcessorInstance, parameters:
 * - panel filename
 * - expected panel title
 */
class ProcessorInstanceWithPanel 
    : public ProcessorInstance
    , public testing::WithParamInterface<std::tuple<std::string, std::string>> 
{
    public:
        void loadTestPanel();

};
