#include "test_ProcessorFixture.h"

#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrApplicationWindow/CtrlrEditor.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "Native/CtrlrNative.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
#include "CtrlrMIDI/CtrlrMIDIBuffer.h"

/**
 * The asynchronous file-chooser helpers in CtrlrInlineUtilitiesGUI.h (namespace FC), through
 * their test hook: no chooser is shown, the hook gives the answer. Each helper must ask with the
 * flags of the blocking browseFor...() call it replaces, call back only later from the message
 * loop (never inside the call), and give an empty File / array for Cancel.
 */
class FileChooserHelpers : public ::testing::Test
{
protected:
    juce::String askedTitle;
    int askedFlags = -1;
    int hookCalls = 0;
    juce::Array<juce::File> answer;

    void SetUp() override
    {
        FC::testAnswer() = [this] (const juce::String &title, int flags) {
            ++hookCalls;
            askedTitle = title;
            askedFlags = flags;
            return answer;
        };
    }

    void TearDown() override { FC::testAnswer() = nullptr; }

    static void pump() { juce::MessageManager::getInstance()->runDispatchLoopUntil (50); }

    static juce::File file (const char *name)
    {
        return juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile (name);
    }
};

TEST_F(FileChooserHelpers, save_asks_to_save_with_the_overwrite_question_and_calls_back_later)
{
    answer = { file ("a.panel") };
    int calls = 0;
    juce::File got;
    FC::saveFileAsync ("Save panel", juce::File(), "*.panel", false, [&] (const juce::File &f) {
        ++calls;
        got = f;
    });

    EXPECT_EQ(hookCalls, 1);
    EXPECT_EQ(askedTitle, "Save panel");
    // browseForFileToSave (true)
    EXPECT_EQ(askedFlags, juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles
                              | juce::FileBrowserComponent::warnAboutOverwriting);
    EXPECT_EQ(calls, 0) << "the callback must not run inside the call";

    pump();
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(got, file ("a.panel"));
}

TEST_F(FileChooserHelpers, open_asks_for_one_existing_file)
{
    answer = { file ("b.panel") };
    juce::File got;
    FC::openFileAsync ("Open", juce::File(), "*.panel", false, [&] (const juce::File &f) { got = f; });
    // browseForFileToOpen()
    EXPECT_EQ(askedFlags, juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles);
    pump();
    EXPECT_EQ(got, file ("b.panel"));
}

TEST_F(FileChooserHelpers, open_multiple_gives_every_answered_file)
{
    answer = { file ("c.lua"), file ("d.lua") };
    juce::Array<juce::File> got;
    FC::openMultipleFilesAsync ("Add files", juce::File(), "*.lua", false,
                                [&] (const juce::Array<juce::File> &files) { got = files; });
    // browseForMultipleFilesToOpen()
    EXPECT_EQ(askedFlags, juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles
                              | juce::FileBrowserComponent::canSelectMultipleItems);
    pump();
    EXPECT_EQ(got, answer);
}

TEST_F(FileChooserHelpers, choose_directory_asks_for_a_directory)
{
    answer = { juce::File::getSpecialLocation (juce::File::tempDirectory) };
    juce::File got;
    FC::chooseDirectoryAsync ("Export", juce::File(), ".vst3", false, [&] (const juce::File &f) { got = f; });
    // browseForDirectory()
    EXPECT_EQ(askedFlags, juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories);
    pump();
    EXPECT_EQ(got, answer.getFirst());
}

TEST_F(FileChooserHelpers, cancel_gives_an_empty_file_or_array)
{
    answer.clear();
    int calls = 0;
    juce::File got = file ("not-cleared");
    juce::Array<juce::File> gotMany = { file ("not-cleared") };
    FC::saveFileAsync ("Save", juce::File(), "*", false, [&] (const juce::File &f) { ++calls; got = f; });
    FC::openMultipleFilesAsync ("Open", juce::File(), "*", false,
                                [&] (const juce::Array<juce::File> &files) { ++calls; gotMany = files; });
    pump();
    EXPECT_EQ(calls, 2) << "Cancel still calls back";
    EXPECT_EQ(got, juce::File());
    EXPECT_TRUE(gotMany.isEmpty());
}

/**
 * The C++ callers of the helpers, with a panel: the work after the chooser runs in its callback,
 * and a callback whose panel or manager is gone does nothing (closing a panel while its chooser
 * is open must not write a file or touch freed memory; the memcheck gate runs these too).
 */
class FileChooserCallers : public ProcessorInstance
{
protected:
    juce::File dir;
    juce::Array<juce::File> answer;
    juce::StringArray askedTitles;

    void SetUp() override
    {
        ProcessorInstance::SetUp();
        allowAndRecordDeviceSends(); // the panels open the mock devices; MIDI is not the subject
        dir = juce::File::getSpecialLocation (juce::File::tempDirectory).getNonexistentChildFile ("ctrlrx-fc-test", "");
        ASSERT_TRUE(dir.createDirectory());
        FC::testAnswer() = [this] (const juce::String &title, int) {
            askedTitles.add (title);
            return answer;
        };
    }

    void TearDown() override
    {
        FC::testAnswer() = nullptr;
        dir.deleteRecursively();
        ProcessorInstance::TearDown();
    }

    // A loaded panel queues its own messages, and a slow runner (macOS CI) may not reach the
    // answer within a short pump: positive tests pump until their result shows (5 s at most),
    // and "nothing happens" tests pump a fixed 500 ms.
    static void pump() { juce::MessageManager::getInstance()->runDispatchLoopUntil (500); }

    static bool pumpUntil (std::function<bool()> done)
    {
        for (int i = 0; i < 100 && ! done(); ++i)
            juce::MessageManager::getInstance()->runDispatchLoopUntil (50);
        return done();
    }
};

TEST_F(FileChooserCallers, save_as_writes_the_answered_file_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    const juce::File target = dir.getChildFile ("saved.panelz");
    answer = { target };

    panel->savePanelAs (CtrlrEditor::doExportFileZText);
    EXPECT_EQ(askedTitles, juce::StringArray ("Panel file XML compressed"));
    EXPECT_FALSE(target.exists()) << "nothing is written before the answer";

    pumpUntil ([&] { return target.existsAsFile() && panel->getProperty (Ids::panelFilePath).toString() == target.getFullPathName(); });
    EXPECT_TRUE(target.existsAsFile());
    EXPECT_EQ(panel->getProperty (Ids::panelFilePath).toString(), target.getFullPathName());
}

TEST_F(FileChooserCallers, save_as_does_nothing_when_the_panel_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    ASSERT_NE(panel->getEditor (false), nullptr);
    const juce::File target = dir.getChildFile ("saved.panel");
    answer = { target };

    panel->savePanelAs (CtrlrEditor::doExportFileText);
    processor->getManager().removePanel (panel->getEditor (false));
    ASSERT_EQ(processor->getManager().getNumPanels(), 0);

    pump();
    EXPECT_FALSE(target.exists());
}

TEST_F(FileChooserCallers, open_opens_the_answered_panel_in_the_callback)
{
    answer = { juce::File::getCurrentWorkingDirectory().getChildFile ("fixtures/fixture_lua_values.panel") };
    const int before = processor->getManager().getNumPanels();

    processor->getManager().openPanelFromFile (nullptr);
    EXPECT_EQ(askedTitles, juce::StringArray ("Open panel"));
    EXPECT_EQ(processor->getManager().getNumPanels(), before);

    pumpUntil ([&] { return processor->getManager().getNumPanels() == before + 1; });
    EXPECT_EQ(processor->getManager().getNumPanels(), before + 1);
}

// Instance export, through the platform's CtrlrNative. Only Cancel and the closed panel: an
// answered export copies the test binary (and on Linux writes ~/.local desktop files).
TEST_F(FileChooserCallers, instance_export_reports_cancel_after_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    answer = {};
    int calls = 0;
    juce::Result got = juce::Result::ok();

    CtrlrNative &native = processor->getManager().getNativeObject();
    native.exportWithDefaultPanel (panel, false, false, [&] (juce::Result r) { ++calls; got = r; });
    EXPECT_EQ(askedTitles, juce::StringArray (juce::String (CTRLR_NEW_INSTANCE_DIALOG_TITLE)));
    EXPECT_EQ(calls, 0) << "the result comes after the answer";

    pumpUntil ([&] { return calls > 0; });
    EXPECT_EQ(calls, 1);
    // savePanelAs() treats exactly this text as a quiet cancel (no alert).
    EXPECT_EQ(got.getErrorMessage(), "User cancelled the export operation.");
}

TEST_F(FileChooserCallers, instance_export_does_nothing_when_the_panel_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    ASSERT_NE(panel->getEditor (false), nullptr);
    const juce::File target = dir.getChildFile ("instance");
    answer = { target };

    panel->savePanelAs (CtrlrEditor::doExportFileInstance);
    ASSERT_EQ(askedTitles.size(), 1);
    processor->getManager().removePanel (panel->getEditor (false));
    ASSERT_EQ(processor->getManager().getNumPanels(), 0);

    pump();
    // Nothing under dir: Windows adds the extension, macOS writes inside the answered directory.
    EXPECT_EQ(dir.getNumberOfChildFiles (juce::File::findFilesAndDirectories), 0) << "no export for a closed panel";
}

// The UI sites. Each one: the answer is used in the callback, and nothing happens (no crash) if
// the component that asked is gone before the answer.
TEST_F(FileChooserCallers, canvas_component_export_writes_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    ASSERT_NE(panel->getEditor (false), nullptr);
    CtrlrModulator *m = panel->getModulator ("target");
    ASSERT_NE(m, nullptr);
    const juce::File target = dir.getChildFile ("target.component");
    answer = { target };

    panel->getEditor (false)->getCanvas()->exportComponent (m->getComponent());
    EXPECT_EQ(askedTitles, juce::StringArray ("Ctrlr component file"));
    EXPECT_FALSE(target.exists());

    pumpUntil ([&] { return target.existsAsFile() && target.getSize() > 0; });
    ASSERT_TRUE(target.existsAsFile());
    EXPECT_GT(target.getSize(), 0);
}

TEST_F(FileChooserCallers, canvas_component_export_does_nothing_when_the_panel_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    ASSERT_NE(panel->getEditor (false), nullptr);
    CtrlrModulator *m = panel->getModulator ("target");
    ASSERT_NE(m, nullptr);
    answer = { dir.getChildFile ("target.component") };

    panel->getEditor (false)->getCanvas()->exportComponent (m->getComponent());
    processor->getManager().removePanel (panel->getEditor (false));
    ASSERT_EQ(processor->getManager().getNumPanels(), 0);

    pump();
    EXPECT_EQ(dir.getNumberOfChildFiles (juce::File::findFilesAndDirectories), 0);
}

static juce::Button *firstButtonOf (juce::Component &c)
{
    for (auto *child : c.getChildren())
        if (auto *b = dynamic_cast<juce::Button *> (child))
            return b;
    return nullptr;
}

TEST_F(FileChooserCallers, file_property_sets_the_value_in_the_callback)
{
    juce::Value value (juce::var ("before"));
    CtrlrFileProperty property (value);
    juce::Button *browse = firstButtonOf (property);
    ASSERT_NE(browse, nullptr);
    const juce::File chosen = dir.getChildFile ("chosen.bin");
    answer = { chosen };

    property.buttonClicked (browse);
    EXPECT_EQ(askedTitles, juce::StringArray ("Select a file"));
    EXPECT_EQ(value.toString(), "before");

    pumpUntil ([&] { return value.toString() == chosen.getFullPathName(); });
    EXPECT_EQ(value.toString(), chosen.getFullPathName());
}

TEST_F(FileChooserCallers, file_property_deleted_before_the_answer_is_left_alone)
{
    juce::Value value (juce::var ("before"));
    auto property = std::make_unique<CtrlrFileProperty> (value);
    juce::Button *browse = firstButtonOf (*property);
    ASSERT_NE(browse, nullptr);
    answer = { dir.getChildFile ("chosen.bin") };

    property->buttonClicked (browse);
    property.reset();

    pump();
    EXPECT_EQ(value.toString(), "before");
}

TEST_F(FileChooserCallers, midi_buffer_loads_the_answered_file_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    const juce::File syx = dir.getChildFile ("data.syx");
    const uint8 bytes[] = { 0xf0, 0x01, 0x02, 0xf7 };
    ASSERT_TRUE(syx.replaceWithData (bytes, sizeof (bytes)));
    answer = { syx };

    CtrlrMIDIBuffer buffer (*panel);
    buffer.loadBinFile (juce::File());
    EXPECT_EQ(askedTitles, juce::StringArray ("Open file"));
    EXPECT_EQ(buffer.getData().getSize(), 0u);

    pumpUntil ([&] { return buffer.getData().getSize() == sizeof (bytes); });
    EXPECT_EQ(buffer.getData(), juce::MemoryBlock (bytes, sizeof (bytes)));
}

TEST_F(FileChooserCallers, midi_buffer_deleted_before_the_answer_is_left_alone)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    const juce::File syx = dir.getChildFile ("data.syx");
    const uint8 bytes[] = { 0xf0, 0x01, 0x02, 0xf7 };
    ASSERT_TRUE(syx.replaceWithData (bytes, sizeof (bytes)));
    answer = { syx };

    auto buffer = std::make_unique<CtrlrMIDIBuffer> (*panel);
    buffer->loadTextFile (juce::File());
    buffer.reset();

    pump(); // must not touch the deleted buffer
    SUCCEED();
}
