#include <iostream>

#include "gtest/gtest.h"

#include "JuceHeader.h"
#include <CtrlrProcessor.h>

#define BLOCK_SIZE 1024


class ProcessorInstance : public testing::Test {
protected:
    CtrlrProcessor processor;
    juce::AudioSampleBuffer buffer;
    juce::MidiBuffer midiMessages;

    ProcessorInstance() : buffer(2, BLOCK_SIZE) {}

    virtual void SetUp() override {
        // initialize buffer with non-zero 'audio'
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            buffer.setSample(0, i, 1.0f * i);
            buffer.setSample(1, i, 2.0f * i);
        }
        // clear the midi buffer
        midiMessages.clear();
    }
    virtual void TearDown() override {}
};