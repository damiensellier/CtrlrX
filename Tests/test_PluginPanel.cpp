#include "test_fixture.h"

#include "CtrlrLog.h"
#include "CtrlrManager.h"
#include "CtrlrPanel.h"
#include "CtrlrModulator.h"

#include <fstream>

bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
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
    
    ASSERT_NO_THROW(processor->openFileFromCli(File("test.panel")));
    
    ASSERT_EQ(processor->getManager().getNumPanels(), 1) << "Expected 1 panel to be loaded";
}


TEST_F(ProcessorInstance, test_panel_loads_ok)
{
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
    load_test_panel();

    CtrlrPanel* panel = processor->getManager().getPanel("Test Panel");

    // we want to make sure that the panel will pass through the MIDI data:
    ASSERT_TRUE(panel->getMidiOptionBool(panelMidiThruH2H));
    EXPECT_FALSE(panel->getMidiOptionBool(panelMidiThruH2HChannelize));

    test_midi_block_processing();
}

TEST_F(ProcessorInstance, test_panel_sends_midi_to_host_after_value_change)
{
    load_test_panel();
    
    processor->prepareToPlay(44100, BLOCK_SIZE);

    // Functions under test:
    processor->setParameter(1, 0.5);
    processor->processBlock(buffer, midiMessages);    

    // Expecting a midi event at the first sample position
    ASSERT_EQ(midiMessages.getNumEvents(), 1) << "Didn't receive the vent in the first process block after setting the parameter";

    auto midi_iter = midiMessages.begin();
    EXPECT_TRUE((*midi_iter).getMessage().isController());
    EXPECT_EQ((*midi_iter).samplePosition, 0);

}