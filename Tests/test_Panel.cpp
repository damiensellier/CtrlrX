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

TEST_F(ProcessorInstance, test_panel_loads_ok)
{
    //ASSERT_TRUE(std::filesystem::exists("test.panel")); // only since c++17
    ASSERT_TRUE(file_exists("test.panel"));

    // to get some extra output for the Release build:
    processor.getCtrlrLog().addListener(new PrintToStdOutListener());

    ASSERT_NO_THROW(processor.openFileFromCli(File("test.panel")));

    ASSERT_EQ(processor.getManager().getNumPanels(), 1) << "Expected 1 panel to be loaded";
    
    CtrlrPanel* panel = processor.getManager().getPanel("Test Panel");
    ASSERT_NE(panel, nullptr) << "CtrlrX can't find the panel 'Test Panel'...";
    
    CtrlrModulator* modulator = panel->getModulator("test-modulator-1");
    ASSERT_NE(modulator, nullptr) << "CtrlrX can't find the modulator 'test-modulator-1'";

    EXPECT_EQ(modulator->getVstIndex(), 1);
    juce::String s = modulator->getProperty(Ids::modulatorCustomName);
    EXPECT_EQ(s.toStdString(), "test_modulator_1");
}
