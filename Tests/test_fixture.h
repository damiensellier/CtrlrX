#include <iostream>

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

    }
    virtual void TearDown() override {
        midiMessages.clear();
    }

    void load_test_panel();

    void test_midi_block_processing();
    void process_block_without_midi_messages_and_expect_no_midi_output(std::string message = "");
};

