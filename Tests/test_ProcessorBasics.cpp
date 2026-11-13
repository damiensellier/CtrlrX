#include "test_ProcessorFixture.h"
#include "CtrlrInlineUtilitiesGUI.h"
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

// JUCE 8's LookAndFeel::findColour() asserts on an undefined colour ID; 5.6.37's JUCE 6 returned
// black silently. Components read Label "when editing" and ScrollBar track colours from the default
// LookAndFeel in their constructors, before they have a parent. CtrlrX defines those IDs as black,
// and must not change a colour the LookAndFeel already defines.
TEST_F(ProcessorInstance, lookandfeel_defaults_add_missing_colours_as_black_only)
{
    const juce::LookAndFeel &defaultLnf = juce::LookAndFeel::getDefaultLookAndFeel();
    EXPECT_TRUE(defaultLnf.isColourSpecified(juce::Label::backgroundWhenEditingColourId))
        << "the processor did not prepare the default LookAndFeel";
    EXPECT_TRUE(defaultLnf.isColourSpecified(juce::ScrollBar::trackColourId));

    juce::LookAndFeel_V3 v3;
    ASSERT_FALSE(v3.isColourSpecified(juce::Label::textWhenEditingColourId));
    gui::initLookAndFeelDefaults(v3);
    EXPECT_EQ(v3.findColour(juce::Label::textWhenEditingColourId), juce::Colours::black);
    EXPECT_EQ(v3.findColour(juce::ScrollBar::trackColourId), juce::Colours::black);

    juce::LookAndFeel_V4 v4;
    const juce::Colour thumb = v4.findColour(juce::ScrollBar::thumbColourId);
    const juce::Colour editText = v4.findColour(juce::Label::textWhenEditingColourId);
    ASSERT_NE(thumb, juce::Colours::black);
    gui::initLookAndFeelDefaults(v4);
    EXPECT_EQ(v4.findColour(juce::ScrollBar::thumbColourId), thumb);
    EXPECT_EQ(v4.findColour(juce::Label::textWhenEditingColourId), editText);
}

// patches/juce8_codeeditor_setfont.patch (5.6.37: v5.6.34, #140): a font of size 0, e.g. a Lua
// editor font size set to 0 by the user, must not give the code editor a zero char width or line
// height; it divides by them.
TEST_F(ProcessorInstance, code_editor_font_of_size_zero_keeps_a_non_zero_cell)
{
    juce::CodeDocument document;
    juce::CodeEditorComponent editor(document, nullptr);

    editor.setFont(juce::Font(juce::FontOptions(0.0f))); // without the patch: SIGFPE here

    EXPECT_GT(editor.getCharWidth(), 0.0f);
    EXPECT_GE(editor.getLineHeight(), 1);
}

void ProcessorInstance::expectNoMidiMessagesInBuffer(std::string message) {
    EXPECT_EQ(midiMessages.getNumEvents(), 0) << "Unexpected midi events " << message;
    // print, to help to debug:
    for (auto it = midiMessages.begin(); it != midiMessages.end(); it++) {
        std::cout << "at sample " << (*it).samplePosition << ": " << (*it).getMessage().getDescription() << std::endl;
    }
}

/**
 * This test will check that for incoming midi messages from the host, 
 * - the message and the timing are preserved at the output to the host, 
 * - and are not echoed in later processing blocks.
 * The problem is that MidiMessageCollector depends on a 'real time' counter (juce::Time::getMillisecondCounterHiRes).
 * Therefore, we allow some delta in the answers, as we can't mock juce::Time::getMillisecondCounterHiRes
 * 
 * The client of this function can indicate how many 'idle' processBlocks can be called and what to check then.
 */
void ProcessorInstance::testMidiBlockProcessing(
    const juce::MidiBuffer messages_to_send, 
    const std::function <void (std::string)>& function_to_call_after_idle_processing,
    int num_iterations_to_idle)
{
    int SAMPLE_RATE = 44100;
    int ALLOWED_DELTA = 3; // 4 samples < 0.1ms

    for (auto msg = messages_to_send.begin(); msg != messages_to_send.end(); msg++)
    {
        midiMessages.addEvent((*msg).getMessage(), (*msg).samplePosition);
    }
    // midiMessages.addEvent(juce::MidiMessage::noteOn(1,2, 1.0f), FIRST_SAMPLE_POS);
    // midiMessages.addEvent(juce::MidiMessage::noteOff(2,3, 0.5f), SECOND_SAMPLE_POS);

    processor->prepareToPlay(SAMPLE_RATE, BLOCK_SIZE);
    ///////////// Round 1 ////////////////////
    // host midi messages directly after a 'prepareToPlay'
    //////////////////////////////////////////
    // Function under test:
    processor->processBlock(buffer, midiMessages);

    // Expecting pass-through behavior.
    // Since MidiMessageCollector depends on a 'real time' counter (getMillisecondCounterHiRes), we allow some delta...
    EXPECT_EQ(midiMessages.getNumEvents(), messages_to_send.getNumEvents()) << "Was expecting an equal amount of midi events back after we have sent them";
    for (auto expected_message = messages_to_send.begin(), received_message = midiMessages.begin()
        ; expected_message != messages_to_send.end() && received_message != midiMessages.end()
        ; expected_message++, received_message++)
    {
        int expected_samplePosition = (*expected_message).samplePosition;
        EXPECT_EQ((*received_message).getMessage(), (*expected_message).getMessage());
        EXPECT_GE((*received_message).samplePosition, expected_samplePosition - ALLOWED_DELTA) << "midi event of first processBlock should be around sample position " << expected_samplePosition;
        EXPECT_LE((*received_message).samplePosition, expected_samplePosition + ALLOWED_DELTA) << "midi event of first processBlock should be around sample position " << expected_samplePosition;
    }

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
    // std::cout << "Samples shifted wrt sleep = " << samples_shifted_wrt_sleep << std::endl;
    // std::cout << "FIRST_SAMPLE_POS after sleep = " << FIRST_SAMPLE_POS << std::endl;
    // std::cout << "SECOND_SAMPLE_POS after sleep = " << SECOND_SAMPLE_POS << std::endl;

    // Re-using the same midiMessages
    // Function under test:
    processor->processBlock(buffer, midiMessages);

    // Expecting pass-through behavior again:
    EXPECT_EQ(midiMessages.getNumEvents(), messages_to_send.getNumEvents()) << "Was expecting midi events back after we have sent them again...";

    for (auto expected_message = messages_to_send.begin(), received_message = midiMessages.begin()
        ; expected_message != messages_to_send.end() && received_message != midiMessages.end()
        ; expected_message++, received_message++)
    {
        int expected_samplePosition = (*expected_message).samplePosition;
        expected_samplePosition += samples_shifted_wrt_sleep;
        expected_samplePosition = juce::jlimit(0, BLOCK_SIZE, expected_samplePosition);

        EXPECT_EQ((*received_message).getMessage(), (*expected_message).getMessage());
        EXPECT_GE((*received_message).samplePosition, expected_samplePosition - ALLOWED_DELTA) << "midi event of next processBlock should be around sample position " << expected_samplePosition;
        EXPECT_LE((*received_message).samplePosition, expected_samplePosition + ALLOWED_DELTA) << "midi event of next processBlock should be around sample position " << expected_samplePosition;
    }

    ///////////// Round 3 -- 3+num_iterations_to_wait ////////////////////
    // no more host messages, see what processing does
    for (int i = 0; i < num_iterations_to_idle; i++) {
        processBlockWithoutMidiMessages("in idle round " + std::to_string(i), function_to_call_after_idle_processing);
    }
}

/**
 * This test will check that calling processBlock after a 23.22ms wait some expectations hold.
 * (note: output to devices is checked by having a StrictMock)
 */
void ProcessorInstance::processBlockWithoutMidiMessages(
    std::string message,
    const std::function <void (std::string)>& function_to_call_after_processing)
{
    midiMessages.clear();

    using namespace std::chrono_literals;
    // don't care for the timing, just need to know that there were no more midi messages to the host...
    // 1024 samples at 44100 is about 23.22ms.
    std::this_thread::sleep_for(23.22ms);

    // Function under test:
    processor->processBlock(buffer, midiMessages);

    if (function_to_call_after_processing)
        function_to_call_after_processing(message);
}

TEST_F(ProcessorInstance, midi_block_processing_without_panel)
{
    juce::MidiBuffer messages_to_send;
    messages_to_send.addEvent(juce::MidiMessage::noteOn(1, 2, 1.0f), 0);
    messages_to_send.addEvent(juce::MidiMessage::noteOff(2, 3, 0.5f), BLOCK_SIZE / 2);
    testMidiBlockProcessing(messages_to_send, [=](std::string msg) {
        this->expectNoMidiMessagesInBuffer(msg);
    });
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
