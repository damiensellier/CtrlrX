#include "test_fixture.h"


TEST_F(ProcessorInstance, basic_processor_properties)
{
    EXPECT_EQ(processor.getName().toStdString(), "CtrlrX");
    EXPECT_EQ(processor.getTotalNumInputChannels(), 2);
    EXPECT_EQ(processor.getTotalNumOutputChannels(), 2);
}

TEST_F(ProcessorInstance, midi_io_capabilities)
{
    EXPECT_TRUE(processor.acceptsMidi());
    EXPECT_TRUE(processor.producesMidi());
    EXPECT_FALSE(processor.isMidiEffect());
}

TEST_F(ProcessorInstance, midi_processing_pass_through)
{
    midiMessages.addEvent(juce::MidiMessage::noteOn(1,2, 1.0f), 0);
    midiMessages.addEvent(juce::MidiMessage::noteOff(2,3, 0.5f), 128);

    processor.prepareToPlay(44100, BLOCK_SIZE);
    processor.processBlock(buffer, midiMessages);

    // expecting pass-through behavior:
    ASSERT_EQ(midiMessages.getNumEvents(), 2);

    MidiMessage m;
    int sample_pos;
    MidiBuffer::Iterator midi_iter(midiMessages);

    midi_iter.getNextEvent(m, sample_pos);
    EXPECT_TRUE(m.isNoteOn());
    EXPECT_EQ(sample_pos, 0) << "First midi event should be at sample position 0";

    midi_iter.getNextEvent(m, sample_pos);
    EXPECT_TRUE(m.isNoteOff());
    EXPECT_EQ(sample_pos, 128) << "Second midi event should be at sample position 128";
}

TEST_F(ProcessorInstance, audio_processing_clears_audio_buffer)
{
    processor.prepareToPlay(44100, BLOCK_SIZE);
    processor.processBlock(buffer, midiMessages);

    // expecting audio buffer to be cleared:
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        ASSERT_DOUBLE_EQ(buffer.getSample(0, i), 0.0f);
        ASSERT_DOUBLE_EQ(buffer.getSample(1, i), 0.0f);
    }
}


