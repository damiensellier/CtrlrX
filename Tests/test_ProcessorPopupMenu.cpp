#include "test_ProcessorFixture.h"

#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrComponentSelection.h"
#include "CtrlrComponents/CtrlrComponent.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanel/CtrlrPanelCanvasLayer.h"
#include "CtrlrLua/MethodEditor/CtrlrLuaMethodEditor.h"
#include "CtrlrLuaManager.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
#include "CtrlrComponents/Specials/CtrlrXYSurface.h"
#include "CtrlrComponents/Groups/CtrlrTabsComponent.h"
#include "CtrlrLua/CtrlrLuaConsole.h"
#include "CtrlrPanel/CtrlrPanelResourceEditor.h"
#include "CtrlrPanel/CtrlrPanelProperties.h"

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
    bool answerFirstItem = false; // or choose the first item
    juce::StringArray askedFirstItems;
    bool answerMissing = false;

    void SetUp() override
    {
        ProcessorInstance::SetUp();
        allowAndRecordDeviceSends(); // the panels open the mock devices; MIDI is not the subject
        PU::testAnswer() = [this] (const juce::PopupMenu &menu, const juce::PopupMenu::Options &) {
            juce::PopupMenu::MenuItemIterator first (menu);
            askedFirstItems.add (first.next() ? first.getItem().text : juce::String());
            if (answerFirstItem)
                return first.getItem().itemID;
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

// The property editors: the multi-MIDI list's Add menu and the SysEx editor's token menu.
static juce::Button *buttonNamed (juce::Component &c, const juce::String &name)
{
    for (auto *child : c.getChildren())
        if (auto *b = dynamic_cast<juce::Button *> (child))
            if (b->getName() == name)
                return b;
    return nullptr;
}

TEST_F(PopupMenuCallers, multi_midi_add_appends_the_chosen_message_in_the_callback)
{
    juce::Value value (juce::var (""));
    CtrlrMultiMidiPropertyComponent property (value);
    juce::Button *add = buttonNamed (property, "Add Multi");
    ASSERT_NE(add, nullptr);
    answerText = "Program Change";

    property.buttonClicked (add);
    EXPECT_EQ(askedFirstItems.size(), 1);
    EXPECT_EQ(value.toString(), "");

    pumpUntil ([&] { return value.toString().isNotEmpty(); });
    EXPECT_EQ(value.toString(), "ProgramChange,-1");
    EXPECT_FALSE(answerMissing);
}

TEST_F(PopupMenuCallers, multi_midi_add_deleted_before_the_answer_is_left_alone)
{
    juce::Value value (juce::var ("CC,-2,-1"));
    auto property = std::make_unique<CtrlrMultiMidiPropertyComponent> (value);
    juce::Button *add = buttonNamed (*property, "Add Multi");
    ASSERT_NE(add, nullptr);
    answerText = "SysEx";

    property->buttonClicked (add);
    property.reset();

    pump();
    EXPECT_EQ(value.toString(), "CC,-2,-1");
}

TEST_F(PopupMenuCallers, sysex_token_menu_sets_the_label_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    juce::Value value (juce::var ("F0 00 F7"));
    CtrlrSysExEditor editor (value, panel);
    juce::Label target;
    target.setText ("00", juce::dontSendNotification);
    answerText = "SysEx Start";

    editor.showTokenMenuForLabel (&target);
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Insert variable"));
    EXPECT_EQ(target.getText(), "00");

    pumpUntil ([&] { return target.getText() == "f0"; });
    EXPECT_EQ(target.getText(), "f0");
    EXPECT_FALSE(answerMissing);
}

TEST_F(PopupMenuCallers, sysex_token_menu_label_deleted_before_the_answer_is_left_alone)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    juce::Value value (juce::var ("F0 00 F7"));
    CtrlrSysExEditor editor (value, panel);
    auto target = std::make_unique<juce::Label>();
    answerText = "SysEx EOM";

    editor.showTokenMenuForLabel (target.get());
    target.reset();

    pump(); // no use of the deleted label (memcheck)
    EXPECT_EQ(value.toString(), "F0 00 F7");
}

// Components: the XY surface's menu and the tabs' "remove tab" menu. They are added through the
// canvas, as the editor does.
static CtrlrComponent *addComponent (CtrlrPanel &panel, const juce::Identifier &type)
{
    CtrlrPanelCanvas *canvas = panel.getEditor (false)->getCanvas();
    return canvas->addNewComponent (type, juce::Point<int> (10, 10), canvas->getLayerFromArray (0));
}

TEST_F(PopupMenuCallers, xy_surface_menu_runs_the_action_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    auto *xy = dynamic_cast<CtrlrXYSurface *> (addComponent (*panel, Ids::uiXYSurface));
    ASSERT_NE(xy, nullptr);
    const bool before = (bool) xy->getProperty (Ids::uiXYSuraceXFlip);
    answerText = "Flip min/max for X axis";

    xy->showPopupMenu();
    EXPECT_EQ(askedFirstItems.size(), 1);
    EXPECT_EQ((bool) xy->getProperty (Ids::uiXYSuraceXFlip), before);

    pumpUntil ([&] { return (bool) xy->getProperty (Ids::uiXYSuraceXFlip) != before; });
    EXPECT_NE((bool) xy->getProperty (Ids::uiXYSuraceXFlip), before);
    EXPECT_FALSE(answerMissing);
}

TEST_F(PopupMenuCallers, xy_surface_menu_does_nothing_when_the_panel_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    auto *xy = dynamic_cast<CtrlrXYSurface *> (addComponent (*panel, Ids::uiXYSurface));
    ASSERT_NE(xy, nullptr);
    answerText = "Flip min/max for Y axis";

    xy->showPopupMenu();
    ASSERT_EQ(askedFirstItems.size(), 1);
    processor->getManager().removePanel (panel->getEditor (false));
    ASSERT_EQ(processor->getManager().getNumPanels(), 0);

    pump(); // no use of the deleted surface (memcheck)
}

// In edit mode a right-click on the surface opens only the editor's component menu (the canvas
// opens that one). With the blocking menus the surface menu opened first and the editor's menu
// after it closed; with asynchronous menus both would open at once.
TEST_F(PopupMenuCallers, xy_surface_right_click_in_edit_mode_opens_no_surface_menu)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    ASSERT_TRUE(panel->getEditMode());
    auto *xy = dynamic_cast<CtrlrXYSurface *> (addComponent (*panel, Ids::uiXYSurface));
    ASSERT_NE(xy, nullptr);

    xy->mouseDown (rightClick (xy));
    pump();
    EXPECT_EQ(askedFirstItems.size(), 0) << askedFirstItems.joinIntoString (", ");
}

// After a panel is loaded in edit mode, bootstrapPanel() tells the panel "not in edit mode" (it
// reads the editor's property from the panel's own tree), while the editor and its canvas are in
// edit mode. The surface must follow the editor, as the canvas does: still no surface menu.
TEST_F(PopupMenuCallers, xy_surface_right_click_follows_the_editors_edit_mode)
{
    CtrlrPanel *panel = panelInEditMode();
    ASSERT_NE(panel, nullptr);
    auto *xy = dynamic_cast<CtrlrXYSurface *> (addComponent (*panel, Ids::uiXYSurface));
    ASSERT_NE(xy, nullptr);
    panel->editModeChanged (false); // as bootstrapPanel() after a load in edit mode
    ASSERT_FALSE(panel->getEditMode());
    ASSERT_TRUE((bool) panel->getEditor (false)->getProperty (Ids::uiPanelEditMode));

    xy->mouseDown (rightClick (xy));
    pump();
    EXPECT_EQ(askedFirstItems.size(), 0) << askedFirstItems.joinIntoString (", ");
}

TEST_F(PopupMenuCallers, xy_surface_right_click_outside_edit_mode_opens_the_surface_menu)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    ASSERT_FALSE(panel->getEditMode());
    auto *xy = dynamic_cast<CtrlrXYSurface *> (addComponent (*panel, Ids::uiXYSurface));
    ASSERT_NE(xy, nullptr);

    xy->mouseDown (rightClick (xy));
    ASSERT_EQ(askedFirstItems.size(), 1);
    EXPECT_EQ(askedFirstItems[0], "Flip min/max for X axis");
}

static int numTabs (CtrlrComponent &tabs)
{
    int n = 0;
    for (const auto &child : tabs.getComponentTree())
        n += child.hasType (Ids::uiTabsTab) ? 1 : 0;
    return n;
}

// Choosing a tab asks "Are you sure ...?" with a blocking alert, which a test can't answer (and
// which needs a window): the choice itself is checked by hand in the GUI. Here: the property
// that opens the menu is reset at once, as before, and a dismissed menu removes nothing.
TEST_F(PopupMenuCallers, tabs_remove_menu_dismissed_removes_nothing)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *tabs = addComponent (*panel, Ids::uiTabs);
    ASSERT_NE(tabs, nullptr);
    tabs->setProperty (Ids::uiTabsAddTab, true); // as the property panel's "add tab"
    tabs->setProperty (Ids::uiTabsAddTab, true);
    const int before = numTabs (*tabs);
    ASSERT_GT(before, 0);
    answerText = ""; // dismissed

    tabs->setProperty (Ids::uiTabsRemoveTab, true);
    EXPECT_EQ(askedFirstItems.size(), 1);
    EXPECT_FALSE((bool) tabs->getProperty (Ids::uiTabsRemoveTab)) << "the property is reset at once, as before";

    pump();
    EXPECT_EQ(numTabs (*tabs), before);
}

static juce::PropertyComponent *firstPropertyRow (juce::Component *root)
{
    if (root == nullptr)
        return nullptr;
    if (auto *row = dynamic_cast<juce::PropertyComponent *> (root))
        return row;
    for (auto *child : root->getChildren())
        if (auto *row = firstPropertyRow (child))
            return row;
    return nullptr;
}

// The properties panel is refreshed after the menu, as before, not while it is open: a refresh
// rebuilds the property rows and scrolls the panel to the top, under the open menu.
TEST_F(PopupMenuCallers, tabs_remove_menu_refreshes_the_properties_after_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *tabs = addComponent (*panel, Ids::uiTabs);
    ASSERT_NE(tabs, nullptr);
    tabs->setProperty (Ids::uiTabsAddTab, true); // as the property panel's "add tab"
    pump();
    juce::Component::SafePointer<juce::PropertyComponent> row (firstPropertyRow (panel->getEditor (false)->getPropertiesPanel()));
    ASSERT_NE(row, nullptr);
    answerText = ""; // dismissed

    tabs->setProperty (Ids::uiTabsRemoveTab, true);
    ASSERT_EQ(askedFirstItems.size(), 1);
    EXPECT_NE(row, nullptr) << "the properties must not be rebuilt while the menu is open";

    EXPECT_TRUE(pumpUntil ([&] { return row == nullptr; })) << "the properties are rebuilt after the answer";
}

// With the tabs gone the callback does nothing: no question (it would block the test).
TEST_F(PopupMenuCallers, tabs_remove_menu_does_nothing_when_the_panel_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    CtrlrComponent *tabs = addComponent (*panel, Ids::uiTabs);
    ASSERT_NE(tabs, nullptr);
    tabs->setProperty (Ids::uiTabsAddTab, true); // as the property panel's "add tab"
    tabs->setProperty (Ids::uiTabsAddTab, true);
    answerFirstItem = true;

    tabs->setProperty (Ids::uiTabsRemoveTab, true);
    ASSERT_EQ(askedFirstItems.size(), 1);
    processor->getManager().removePanel (panel->getEditor (false));
    ASSERT_EQ(processor->getManager().getNumPanels(), 0);

    pump();
}

// The rest: the Lua console's snips menu, the resource editor's row menu, and the macOS signing
// identity menu.
TEST_F(PopupMenuCallers, lua_console_snips_menu_runs_the_action_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    CtrlrLuaConsole console (*panel);
    juce::TextButton snipsButton ("Snips");
    const bool before = (bool) panel->getProperty (Ids::uiLuaConsoleInputRemoveAfterRun);
    answerText = "Toggle input removal after run";

    console.snipsItemClicked (&snipsButton);
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Add input to snips"));
    EXPECT_EQ((bool) panel->getProperty (Ids::uiLuaConsoleInputRemoveAfterRun), before);

    pumpUntil ([&] { return (bool) panel->getProperty (Ids::uiLuaConsoleInputRemoveAfterRun) != before; });
    EXPECT_NE((bool) panel->getProperty (Ids::uiLuaConsoleInputRemoveAfterRun), before);
    EXPECT_FALSE(answerMissing);
}

TEST_F(PopupMenuCallers, lua_console_deleted_before_the_answer_is_left_alone)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    auto console = std::make_unique<CtrlrLuaConsole> (*panel);
    juce::TextButton snipsButton ("Snips");
    const bool before = (bool) panel->getProperty (Ids::uiLuaConsoleInputRemoveAfterRun);
    answerText = "Toggle input removal after run";

    console->snipsItemClicked (&snipsButton);
    console.reset();

    pump();
    EXPECT_EQ((bool) panel->getProperty (Ids::uiLuaConsoleInputRemoveAfterRun), before);
}

TEST_F(PopupMenuCallers, resource_editor_row_menu_does_nothing_when_the_editor_closed_before_the_answer)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    auto editor = std::make_unique<CtrlrPanelResourceEditor> (*panel->getEditor (false));
    answerText = "More info";

    editor->cellClicked (0, 1, rightClick (editor.get()));
    EXPECT_EQ(askedFirstItems, juce::StringArray ("Resource"));
    editor.reset();

    pump(); // no use of the deleted editor (memcheck)
}

#if JUCE_MAC
// Only macOS has `security find-identity`. With no signing identity on the machine the menu has
// CtrlrX's ad-hoc entry; either way the first item is an identity.
TEST_F(PopupMenuCallers, mac_signing_identity_menu_sets_the_identity_in_the_callback)
{
    CtrlrPanel *panel = loadPanel ("fixture_lua_values.panel");
    ASSERT_NE(panel, nullptr);
    juce::String firstItem;
    PU::testAnswer() = [&] (const juce::PopupMenu &menu, const juce::PopupMenu::Options &) {
        juce::PopupMenu::MenuItemIterator it (menu);
        if (! it.next())
            return 0;
        firstItem = it.getItem().text;
        return it.getItem().itemID;
    };
    panel->setProperty (Ids::panelCertificateMacId, "");

    panel->setProperty (Ids::panelCertificateMacSelectId, true);
    ASSERT_TRUE(firstItem.isNotEmpty());
    EXPECT_FALSE((bool) panel->getProperty (Ids::panelCertificateMacSelectId)) << "the button property is reset at once";
    EXPECT_EQ(panel->getProperty (Ids::panelCertificateMacId).toString(), "");

    pumpUntil ([&] { return panel->getProperty (Ids::panelCertificateMacId).toString() == firstItem; });
    EXPECT_EQ(panel->getProperty (Ids::panelCertificateMacId).toString(), firstItem);
}
#endif
