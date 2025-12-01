#include "test_fixture.h"

#include "CtrlrLog.h"
#include "CtrlrManager.h"
#include "CtrlrPanel.h"
#include "CtrlrModulator.h"

#include <chrono>
#include <fstream>
#include <thread>

bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

// outside operator overloading
namespace juce {
    bool operator==(const MidiMessage& lhs, const MidiMessage& rhs)
    {
        bool equal_data = true;
        for (size_t idx = 0; idx < lhs.getRawDataSize() && idx < rhs.getRawDataSize(); idx++)
            equal_data &= (lhs.getRawData()[idx] == rhs.getRawData()[idx]);
        return (lhs.getRawDataSize() == rhs.getRawDataSize()) && equal_data;
    }
}


class PrintToStdOutListener : public CtrlrLog::Listener {
    ~PrintToStdOutListener() override {}
    void messageLogged (CtrlrLog::CtrlrLogMessage message) override {
        std::cout << CtrlrLog::levelToString(message.level).toStdString() << ": " << message.message.toStdString() << std::endl;
    }
};


void ProcessorInstance::load_test_panel()
{
    ASSERT_TRUE(file_exists("test.panel"));
    
    // to get some extra output for the Release build:
    processor->getCtrlrLog().addListener(new PrintToStdOutListener());
    
    ASSERT_NO_THROW(processor->openFileFromCli(File::getCurrentWorkingDirectory().getChildFile ("test.panel")));
    
    ASSERT_EQ(processor->getManager().getNumPanels(), 1) << "Expected 1 panel to be loaded";
}


TEST_F(ProcessorInstance, test_panel_loads_ok)
{
    // first, set expectations, if we've implemented the mock for the OS's midi subsystem
    if (midi_mock.hasSubsystemMock()) {
        std::cout << "Mock is implemented, setting hardware device expectations" << std::endl;
        EXPECT_CALL(midi_mock, openOutput(0, 1));
    }
    
    // then act:
    load_test_panel();

    EXPECT_EQ(processor->getName().toStdString(), "CtrlrX");
    
    CtrlrPanel* panel = processor->getManager().getPanel("Test Panel");
    ASSERT_NE(panel, nullptr) << "CtrlrX can't find the panel 'Test Panel'...";
    
    CtrlrModulator* modulator = panel->getModulator("test-modulator-1");
    ASSERT_NE(modulator, nullptr) << "CtrlrX can't find the modulator 'test-modulator-1'";

    EXPECT_EQ(modulator->getVstIndex(), 1);
    juce::String s = modulator->getProperty(Ids::modulatorCustomName);
    EXPECT_EQ(s.toStdString(), "test_modulator_1");
}

TEST_F(ProcessorInstance, test_panel_midi_block_processing)
{
    // first, set expectations, if we've implemented the mock for the OS's midi subsystem
    if (midi_mock.hasSubsystemMock()) {
        std::cout << "Mock is implemented, setting hardware device expectations" << std::endl;
        EXPECT_CALL(midi_mock, openOutput(0, 1));
    }
    // then act:
    load_test_panel();

    CtrlrPanel* panel = processor->getManager().getPanel("Test Panel");
    
    // we want to make sure that the panel will pass through the MIDI data:
    ASSERT_FALSE(panel->isMidiInPaused());
    ASSERT_FALSE(panel->isMidiOutPaused());
    ASSERT_TRUE(panel->getMidiOptionBool(panelMidiThruH2H));
    ASSERT_TRUE(panel->getMidiOptionBool(panelMidiOutputToHost));
    EXPECT_FALSE(panel->getMidiOptionBool(panelMidiThruH2HChannelize));

    panel->setProperty(Ids::panelMidiGlobalDelay, 5.0, false); // set 5 ms global delay

    test_midi_block_processing();
}

TEST_F(ProcessorInstance, test_panel_sends_midi_to_host_and_device_after_value_change)
{
    // first, set expectations, if we've implemented the mock for the OS's midi subsystem
    if (midi_mock.hasSubsystemMock()) {
        std::cout << "Mock is implemented, setting hardware device expectations" << std::endl;
        EXPECT_CALL(midi_mock, openOutput(0, 1));
        EXPECT_CALL(midi_mock, sendMidiEvent(0, 1, juce::MidiMessage::controllerEvent(4, 3, 64))).Times(1);
    }
    // then act:
    load_test_panel();
    
    processor->prepareToPlay(44100, BLOCK_SIZE);

    std::cout << "Host sending parameter change" << std::endl;
    // Functions under test:
    processor->setParameter(1, 0.5);
    processor->processBlock(buffer, midiMessages);

    // Expecting a midi event at the first sample position
    ASSERT_EQ(midiMessages.getNumEvents(), 1) << "Didn't receive the vent in the first process block after setting the parameter";

    auto midi_iter = midiMessages.begin();
    EXPECT_TRUE((*midi_iter).getMessage().isController());
    EXPECT_EQ((*midi_iter).samplePosition, 0);

    process_block_without_midi_messages_and_expect_no_midi_output("in round 2");
}
