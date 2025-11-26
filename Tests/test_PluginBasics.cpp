#include "test_fixture.h"
#include <chrono>
#include <thread>



TEST_F(ProcessorInstance, basic_processor_properties)
{
    EXPECT_EQ(processor->getName().toStdString(), "CtrlrX");
    EXPECT_EQ(processor->getTotalNumInputChannels(), 2);
    EXPECT_EQ(processor->getTotalNumOutputChannels(), 2);
}

TEST_F(ProcessorInstance, midi_io_capabilities)
{
    EXPECT_TRUE(processor->acceptsMidi());
    EXPECT_TRUE(processor->producesMidi());
    EXPECT_FALSE(processor->isMidiEffect());
}


/**
 * This test will check that for incoming midi messages from the host, the message and the timing are preserved at the output to the host.
 * The problem is that MidiMessageCollector depends on a 'real time' counter (juce::Time::getMillisecondCounterHiRes).
 * Therefore, we allow some delta in the answers, as we can't mock juce::Time::getMillisecondCounterHiRes
 */
void ProcessorInstance::test_midi_block_processing()
{
    int SAMPLE_RATE=44100;
    int ALLOWED_DELTA = 3; // 4 samples < 0.1ms
    int FIRST_SAMPLE_POS=0;
    int SECOND_SAMPLE_POS = 512;
    midiMessages.addEvent(juce::MidiMessage::noteOn(1,2, 1.0f), FIRST_SAMPLE_POS);
    midiMessages.addEvent(juce::MidiMessage::noteOff(2,3, 0.5f), SECOND_SAMPLE_POS);
    
    processor->prepareToPlay(SAMPLE_RATE, BLOCK_SIZE);
    ///////////// Round 1 ////////////////////
    // host midi messages directly after a 'prepareToPlay'
    //////////////////////////////////////////
    // Function under test:
    processor->processBlock(buffer, midiMessages);
    
    // Expecting pass-through behavior. 
    // Since MidiMessageCollector depends on a 'real time' counter (getMillisecondCounterHiRes), we allow some delta...
    ASSERT_EQ(midiMessages.getNumEvents(), 2);
    auto midi_iter = midiMessages.begin();
    EXPECT_TRUE((*midi_iter).getMessage().isNoteOn());
    EXPECT_EQ((*midi_iter).samplePosition, FIRST_SAMPLE_POS) << "First midi event after reset should be at sample position 0";

    midi_iter++;
    EXPECT_TRUE((*midi_iter).getMessage().isNoteOff());
    EXPECT_GE((*midi_iter).samplePosition, SECOND_SAMPLE_POS - ALLOWED_DELTA) << "Second midi event after reset should be around sample position " << SECOND_SAMPLE_POS;
    EXPECT_LE((*midi_iter).samplePosition, SECOND_SAMPLE_POS + ALLOWED_DELTA) << "Second midi event after reset should be around sample position " << SECOND_SAMPLE_POS;

    ///////////// Round 2 ////////////////////
    // host midi messages after a previous 'processBlock'
    // 1024 samples at 44100 is about 23.22ms.
    // Sleeping longer means the event comes 'earlier' in terms of sample position.
    //////////////////////////////////////////
    using namespace std::chrono_literals;
    double time_start = Time::getMillisecondCounterHiRes();
    std::this_thread::sleep_for(23.22ms);
    double time_finish = Time::getMillisecondCounterHiRes();

    // Since MidiMessageCollector depends on a 'real time' counter (getMillisecondCounterHiRes), 
    // and sleeping is never super accurate, we correct the FIRST_/SECOND_SAMPLE_POS for the actual time slept.
    double block_time = static_cast<double>(BLOCK_SIZE) / SAMPLE_RATE * 1000.0f; // in ms
    int samples_shifted_wrt_sleep = SAMPLE_RATE * (block_time - (time_finish - time_start)) / 1000.0f;
    if (samples_shifted_wrt_sleep < 0)
        samples_shifted_wrt_sleep /= 2; // half of what you'd expect if we sleep longer?
    FIRST_SAMPLE_POS += samples_shifted_wrt_sleep;
    SECOND_SAMPLE_POS += samples_shifted_wrt_sleep;
    // clamp to 0, BLOCKSIZE:
    FIRST_SAMPLE_POS = juce::jlimit(0, BLOCK_SIZE, FIRST_SAMPLE_POS);
    SECOND_SAMPLE_POS = juce::jlimit(0, BLOCK_SIZE, SECOND_SAMPLE_POS);
    // std::cout << "Samples shifted wrt sleep = " << samples_shifted_wrt_sleep << std::endl;
    // std::cout << "FIRST_SAMPLE_POS after sleep = " << FIRST_SAMPLE_POS << std::endl;
    // std::cout << "SECOND_SAMPLE_POS after sleep = " << SECOND_SAMPLE_POS << std::endl;
    
    // Re-using the same midiMessages
    // Function under test:
    processor->processBlock(buffer, midiMessages);
    
    // Expecting pass-through behavior again:
    ASSERT_EQ(midiMessages.getNumEvents(), 2);
    midi_iter = midiMessages.begin();
    EXPECT_TRUE((*midi_iter).getMessage().isNoteOn());
    EXPECT_LE((*midi_iter).samplePosition, FIRST_SAMPLE_POS + ALLOWED_DELTA) << "First midi event of next processBlock should be at sample position 0";
    

    midi_iter++;
    EXPECT_TRUE((*midi_iter).getMessage().isNoteOff());
    // std::cout << "Time slept = " << (time_finish - time_start) << std::endl;
    // std::cout << "sample pos = " << (*midi_iter).samplePosition << std::endl;
    EXPECT_GE((*midi_iter).samplePosition, SECOND_SAMPLE_POS - ALLOWED_DELTA) << "Second midi event of next processBlock should be around sample position " << SECOND_SAMPLE_POS;
    EXPECT_LE((*midi_iter).samplePosition, SECOND_SAMPLE_POS + ALLOWED_DELTA) << "Second midi event of next processBlock should be around sample position " << SECOND_SAMPLE_POS;

    ///////////// Round 3 ////////////////////
    // no more host messages.
    // don't care for the timing, just need to know that there were no more midi messages to the host...
    // 1024 samples at 44100 is about 23.22ms.
    //////////////////////////////////////////
    midiMessages.clear();
    std::this_thread::sleep_for(23.22ms);

    // Function under test:
    processor->processBlock(buffer, midiMessages);
    EXPECT_EQ(midiMessages.getNumEvents(), 0) << "Was not expecting more midi messages, because the host didn't send anything";

    ///////////// Round 4 ////////////////////
    // no more host messages.
    // don't care for the timing, just need to know that there were no more midi messages to the host...
    // 1024 samples at 44100 is about 23.22ms.
    // Sleeping longer means the event comes 'earlier' in terms of sample position.
    //////////////////////////////////////////
    midiMessages.clear();
    std::this_thread::sleep_for(23.22ms);

    // Function under test:
    processor->processBlock(buffer, midiMessages);
    EXPECT_EQ(midiMessages.getNumEvents(), 0) << "Was not expecting more midi messages, because the host didn't send anything";
}

TEST_F(ProcessorInstance, midi_block_processing_without_panel)
{
    test_midi_block_processing();
}

TEST_F(ProcessorInstance, audio_processing_clears_audio_buffer)
{
    processor->prepareToPlay(44100, BLOCK_SIZE);
    processor->processBlock(buffer, midiMessages);

    // expecting audio buffer to be cleared:
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        ASSERT_DOUBLE_EQ(buffer.getSample(0, i), 0.0f);
        ASSERT_DOUBLE_EQ(buffer.getSample(1, i), 0.0f);
    }
}


