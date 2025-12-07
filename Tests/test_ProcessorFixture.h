#include <iostream>
#include <tuple>
#include <string>

#include "gtest/gtest.h"

#include "JuceHeader.h"
#include <CtrlrProcessor.h>

#include "mock_MidiDevice.h"

#define BLOCK_SIZE 1024


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
    }

    void expect_no_midi_messages_in_buffer(std::string message);

    void test_midi_block_processing(
        const juce::MidiBuffer messages_to_send, 
        const std::function <void (std::string)>& function_to_call_after_idle_processing = nullptr,
        int num_iterations_to_idle = 3);
    void process_block_without_midi_messages(
        std::string message = "", 
        const std::function <void (std::string)>& function_to_call_after_processing = nullptr);
};

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
        void load_test_panel();

};

// outside operator overloading
namespace juce {
    inline bool operator==(const MidiMessage& lhs, const MidiMessage& rhs)
    {
        bool equal_data = true;
        for (size_t idx = 0; idx < lhs.getRawDataSize() && idx < rhs.getRawDataSize(); idx++)
            equal_data &= (lhs.getRawData()[idx] == rhs.getRawData()[idx]);
        return (lhs.getRawDataSize() == rhs.getRawDataSize()) && equal_data;
    }
}
