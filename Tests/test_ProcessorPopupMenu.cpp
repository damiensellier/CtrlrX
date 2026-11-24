#include "test_ProcessorFixture.h"

#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrComponentSelection.h"
#include "CtrlrComponents/CtrlrComponent.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanel/CtrlrPanelCanvasLayer.h"
#include "CtrlrLua/MethodEditor/CtrlrLuaMethodEditor.h"
#include "CtrlrLuaManager.h"

/**
 * The asynchronous popup-menu helpers in CtrlrInlineUtilitiesGUI.h (namespace PU), through their
 * test hook: no menu is shown, the hook chooses the item. Each helper must pass the options of
 * the blocking PopupMenu call it replaces (so the menu opens where it did), call back only later
 * from the message loop (never inside the call), and give 0 when the menu was dismissed.
 */
class PopupMenuHelpers : public ::testing::Test
{
protected:
    int hookCalls = 0;
    int answer = 0;
    juce::PopupMenu askedMenu;
    juce::Component *askedTarget = nullptr;
    juce::Rectangle<int> askedArea;

    void SetUp() override
    {
        PU::testAnswer() = [this] (const juce::PopupMenu &menu, const juce::PopupMenu::Options &options) {
            ++hookCalls;
            askedMenu = menu;
            askedTarget = options.getTargetComponent();
            askedArea = options.getTargetScreenArea();
            return answer;
        };
    }

    void TearDown() override { PU::testAnswer() = nullptr; }

    static void pump() { juce::MessageManager::getInstance()->runDispatchLoopUntil (50); }

    static juce::PopupMenu twoItems()
    {
        juce::PopupMenu m;
        m.addItem (1, "First");
        m.addItem (2, "Second");
        return m;
    }
};

TEST_F(PopupMenuHelpers, show_opens_at_the_mouse_and_calls_back_later)
{
    answer = 2;
    int calls = 0;
    int got = -1;
    PU::show (twoItems(), [&] (int result) {
        ++calls;
        got = result;
    });

    EXPECT_EQ(hookCalls, 1);
    EXPECT_EQ(askedMenu.getNumItems(), 2);
    // PopupMenu::show(): no target, so JUCE opens the menu at the mouse position
    EXPECT_EQ(askedTarget, nullptr);
    EXPECT_TRUE(askedArea.isEmpty());
    EXPECT_EQ(calls, 0) << "the callback must not run inside the call";

    pump();
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(got, 2);
}

TEST_F(PopupMenuHelpers, show_at_targets_the_component)
{
    answer = 1;
    juce::Component button;
    int got = -1;
    PU::showAt (twoItems(), &button, [&] (int result) { got = result; });
    // PopupMenu::showAt (component)
    EXPECT_EQ(askedTarget, &button);
    pump();
    EXPECT_EQ(got, 1);
}

TEST_F(PopupMenuHelpers, show_at_without_a_component_is_show)
{
    PU::showAt (twoItems(), nullptr, [] (int) {});
    EXPECT_EQ(hookCalls, 1);
    EXPECT_EQ(askedTarget, nullptr);
    EXPECT_TRUE(askedArea.isEmpty());
    pump();
}

TEST_F(PopupMenuHelpers, dismissed_gives_0_and_still_calls_back)
{
    answer = 0;
    int calls = 0;
    int got = -1;
    PU::show (twoItems(), [&] (int result) {
        ++calls;
        got = result;
    });
    pump();
    EXPECT_EQ(calls, 1) << "a dismissed menu still calls back";
    EXPECT_EQ(got, 0);
}

// JUCE shows nothing for an empty menu and never calls back; the blocking show() returned 0.
TEST_F(PopupMenuHelpers, empty_menu_calls_back_with_0_without_asking)
{
    answer = 5;
    int calls = 0;
    int got = -1;
    PU::show (juce::PopupMenu(), [&] (int result) {
        ++calls;
        got = result;
    });
    EXPECT_EQ(hookCalls, 0) << "no menu is shown";
    EXPECT_EQ(calls, 0) << "the callback must not run inside the call";
    pump();
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(got, 0);
}

/**
 * The C++ callers of the helpers: the action after the menu runs in its callback, and a callback
 * whose panel or component is gone does nothing (closing a panel while its menu is open must not
 * touch freed memory; the memcheck gate runs these too).
 */
class PopupMenuCallers : public ProcessorInstance
{
protected:
    juce::String answerText; // the item to choose, by its text; empty = dismiss
    juce::StringArray askedFirstItems;
    bool answerMissing = false;

    void SetUp() override
    {
        ProcessorInstance::SetUp();
        allowAndRecordDeviceSends(); // the panels open the mock devices; MIDI is not the subject
        PU::testAnswer() = [this] (const juce::PopupMenu &menu, const juce::PopupMenu::Options &) {
            juce::PopupMenu::MenuItemIterator first (menu);
            askedFirstItems.add (first.next() ? first.getItem().text : juce::String());
            if (answerText.isEmpty())
                return 0;
            const int id = idOf (menu, answerText);
            answerMissing = answerMissing || id == 0;
            return id;
        };
    }

    void TearDown() override
    {
        PU::testAnswer() = nullptr;
        ProcessorInstance::TearDown();
    }

    static int idOf (const juce::PopupMenu &menu, const juce::String &text)
    {
        for (juce::PopupMenu::MenuItemIterator it (menu, true); it.next();)
            if (it.getItem().text == text && it.getItem().itemID != 0)
                return it.getItem().itemID;
        return 0;
    }

    // As FileChooserCallers: pump until the result shows (5 s at most), or a fixed 500 ms for
    // "nothing happens".
    static void pump() { juce::MessageManager::getInstance()->runDispatchLoopUntil (500); }

    static bool pumpUntil (std::function<bool()> done)
    {
        for (int i = 0; i < 100 && ! done(); ++i)
            juce::MessageManager::getInstance()->runDispatchLoopUntil (50);
        return done();
    }

    // A right-click on `c` as the canvas gets it (the canvas listens to its components' mouse).
    static juce::MouseEvent rightClick (juce::Component *c, juce::ModifierKeys extra = {})
    {
        const juce::Point<float> pos (5.0f, 5.0f);
        const juce::Time now = juce::Time::getCurrentTime();
        const juce::ModifierKeys mods (juce::ModifierKeys::rightButtonModifier | extra.getRawFlags());
        return juce::MouseEvent (juce::Desktop::getInstance().getMainMouseSource(), pos, mods,
                                 juce::MouseInputSource::invalidPressure, juce::MouseInputSource::invalidOrientation,
                                 juce::MouseInputSource::invalidRotation, juce::MouseInputSource::invalidTiltX,
                                 juce::MouseInputSource::invalidTiltY, c, c, now, pos, now, 1, false);
    }

    CtrlrPanel *panelInEditMode()
    {
        CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
        if (panel == nullptr || panel->getEditor (false) == nullptr)
            return nullptr;
        panel->getEditor (false)->setProperty (Ids::uiPanelEditMode, true);
        return panel;
    }
};

TEST_F(PopupMenuCallers, canvas_component_menu_runs_the_action_in_the_callback)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *c = panel->getModulator ("target")->getComponent();
    ASSERT_NE(c, nullptr);
    answerText = "Send to back";

    panel->getEditor (false)->getCanvas()->mouseDown (rightClick (c));
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Actions"));
    EXPECT_FALSE((bool) c->getProperty (Ids::componentSentBack)) << "nothing happens before the answer";

    pumpUntil ([&] { return (bool) c->getProperty (Ids::componentSentBack); });
    EXPECT_TRUE((bool) c->getProperty (Ids::componentSentBack));
    EXPECT_FALSE(answerMissing);
}

TEST_F(PopupMenuCallers, canvas_component_menu_does_nothing_when_the_panel_closed_before_the_answer)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *c = panel->getModulator ("target")->getComponent();
    answerText = "Delete";

    panel->getEditor (false)->getCanvas()->mouseDown (rightClick (c));
    ASSERT_EQ(askedFirstItems.size(), 1);
    processor->getManager().removePanel (panel->getEditor (false));
    ASSERT_EQ(processor->getManager().getNumPanels(), 0);

    pump(); // no crash, no use of the freed canvas (memcheck)
}

// Ctrl (Cmd on macOS) + right-click: after the blocking menu, mouseDown also toggled the
// component in the selection. That still happens after the action: Delete acts on the selected
// component (in the other order the toggle would empty the selection first and nothing would be
// deleted), and the toggle is skipped because the component is gone.
TEST_F(PopupMenuCallers, canvas_ctrl_right_click_toggles_the_selection_after_the_action)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *c = panel->getModulator ("target")->getComponent();
    ASSERT_NE(c, nullptr);
    answerText = "Delete";

    panel->getEditor (false)->getCanvas()->mouseDown (rightClick (c, juce::ModifierKeys::commandModifier));
    ASSERT_EQ(askedFirstItems.size(), 1);
    ASSERT_NE(panel->getEditor (false)->getSelection(), nullptr);
    EXPECT_TRUE(panel->getEditor (false)->getSelection()->isSelected (c)) << "the toggle waits for the menu";

    pumpUntil ([&] { return panel->getModulator ("target") == nullptr; });
    EXPECT_EQ(panel->getModulator ("target"), nullptr) << "Delete ran on the selected component";
}

TEST_F(PopupMenuCallers, canvas_shift_right_click_dismissed_keeps_the_selection)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *c = panel->getModulator ("target")->getComponent();
    answerText = ""; // dismissed

    panel->getEditor (false)->getCanvas()->mouseDown (rightClick (c, juce::ModifierKeys::shiftModifier));
    pump();
    EXPECT_TRUE(panel->getEditor (false)->getSelection()->isSelected (c));
    EXPECT_NE(panel->getModulator ("target"), nullptr);
}

// Right-click with more than one component selected: the multi-selection menu.
TEST_F(PopupMenuCallers, canvas_multi_selection_menu_runs_the_action_in_the_callback)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *a = panel->getModulator ("target")->getComponent();
    CtrlrComponent *b = panel->getModulator ("other")->getComponent();
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    CtrlrComponentSelection *selection = panel->getEditor (false)->getSelection();
    ASSERT_NE(selection, nullptr);
    selection->selectOnly (a);
    selection->addToSelection (b);
    answerText = "Delete";

    panel->getEditor (false)->getCanvas()->mouseDown (rightClick (a));
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Edit"));
    EXPECT_NE(panel->getModulator ("target"), nullptr) << "nothing happens before the answer";

    pumpUntil ([&] { return panel->getModulator ("target") == nullptr && panel->getModulator ("other") == nullptr; });
    EXPECT_EQ(panel->getModulator ("target"), nullptr);
    EXPECT_EQ(panel->getModulator ("other"), nullptr);
    EXPECT_FALSE(answerMissing);
}

TEST_F(PopupMenuCallers, canvas_layer_menu_adds_the_component_in_the_callback)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    CtrlrPanelCanvas *canvas = panel->getEditor (false)->getCanvas();
    CtrlrPanelCanvasLayer *layer = canvas->getLayerFromArray (0);
    ASSERT_NE(layer, nullptr);
    const int before = panel->getNumModulators();
    answerText = Ids::uiSlider.toString();

    canvas->mouseDown (rightClick (layer));
    EXPECT_EQ(askedFirstItems.size(), 1);
    EXPECT_EQ(panel->getNumModulators(), before);

    pumpUntil ([&] { return panel->getNumModulators() == before + 1; });
    EXPECT_EQ(panel->getNumModulators(), before + 1);
    EXPECT_FALSE(answerMissing);
}

// The Lua method editor's tree: the group menu and the method menu.
static juce::StringArray methodNames (const juce::ValueTree &tree)
{
    juce::StringArray names;
    for (const auto &child : tree)
        if (child.hasProperty (Ids::luaMethodName))
            names.add (child.getProperty (Ids::luaMethodName).toString());
    return names;
}

TEST_F(PopupMenuCallers, lua_editor_group_menu_runs_the_action_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    CtrlrLuaMethodEditor editor (*panel);
    juce::ValueTree root = panel->getCtrlrLuaManager().getMethodManager().getManagerTree();
    juce::StringArray sorted = methodNames (root);
    sorted.sortNatural();
    ASSERT_NE(methodNames (root), sorted) << "the fixture's methods must start unsorted";
    answerText = "Sort by name";

    editor.itemClicked (rightClick (&editor), root);
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Group operations"));
    EXPECT_NE(methodNames (root), sorted) << "nothing happens before the answer";

    pumpUntil ([&] { return methodNames (root) == sorted; });
    EXPECT_EQ(methodNames (root), sorted);
    EXPECT_FALSE(answerMissing);
}

// "Remove method" asks "Delete the selected method?" after the menu. With the editor gone the
// callback does nothing: no question (it would block the test), the method stays.
TEST_F(PopupMenuCallers, lua_editor_method_menu_does_nothing_when_the_editor_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    auto editor = std::make_unique<CtrlrLuaMethodEditor> (*panel);
    juce::ValueTree root = panel->getCtrlrLuaManager().getMethodManager().getManagerTree();
    juce::ValueTree method = root.getChildWithProperty (Ids::luaMethodName, "onTargetChange");
    ASSERT_TRUE(method.isValid());
    const int before = methodNames (root).size();
    answerText = "Remove method";

    editor->itemClicked (rightClick (editor.get()), method);
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Method onTargetChange"));
    editor.reset();

    pump();
    EXPECT_EQ(methodNames (root).size(), before);
    EXPECT_FALSE(answerMissing);
}
