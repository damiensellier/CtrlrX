#include "test_ProcessorLuaFixture.h"

#include "CtrlrModulator.h"
#include "CtrlrComponents/CtrlrComponent.h"

/**
 * Panel, modulator and component state reachable from Lua: properties (setPropertyString /
 * getProperty and friends, incl. the ones with side effects like uiSliderMin/Max and
 * componentVisibility), component colour state, save/restore and program state.
 *
 * Neither large panel the suites were derived from uses component colour state or the panel
 * save/restore hooks; those tests pin CtrlrX behaviour rather than mirror panel usage.
 */

class LuaState : public LuaPanel
{
protected:
    CtrlrComponent* component(const char* modulatorName)
    {
        CtrlrModulator* m = panel->getModulator(modulatorName);
        return m != nullptr ? m->getComponent() : nullptr;
    }

    // The first child of `c` of type T (CtrlrX components wrap a JUCE widget as a child).
    template <typename T>
    static T* child(juce::Component* c)
    {
        for (int i = 0; c != nullptr && i < c->getNumChildComponents(); i++)
            if (auto* t = dynamic_cast<T*>(c->getChildComponent(i)))
                return t;
        return nullptr;
    }

    int count(const char* global)
    {
        const double v = luaNumber(global);
        return std::isnan(v) ? 0 : (int) v;
    }

    // What a host does with a saved project: take the plugin state, throw the instance away and
    // restore the state into a fresh one. `panel` points into the new instance afterwards.
    void reopenFromState()
    {
        juce::MemoryBlock state;
        processor->getStateInformation(state);
        ASSERT_GT(state.getSize(), 0u);

        panel = nullptr;
        processor.reset();
        processor = std::make_shared<CtrlrProcessor>();
        if (midi_mock.hasSubsystemMock())
            processor->getManager().getCtrlrMIDIDeviceManager().refreshDevices();

        processor->setStateInformation(state.getData(), (int) state.getSize());
        const int n = processor->getManager().getNumPanels();
        ASSERT_EQ(n, 1);
        panel = processor->getManager().getPanel(0);
        processor->prepareToPlay(44100, BLOCK_SIZE);
        pumpInputThreads(200);
    }
};

// ----------------------------------------------------------------------------------------------
// Properties
// ----------------------------------------------------------------------------------------------

/**
 * Wiring check for the suite: a component property set from Lua reads back from Lua.
 */
TEST_F(LuaState, component_property_set_from_lua_reads_back)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local c = panel:getComponent('target')\n"
                       "c:setPropertyString('componentVisibleName', 'renamed')\n"
                       "_same = (L(c:getProperty('componentVisibleName')) == 'renamed') and 1 or 0"));
    EXPECT_EQ(luaNumber("_same"), 1.0);
}

/**
 * getProperty returns what was stored: a string after setPropertyString, a number after
 * setPropertyInt or a numeric setProperty, on the panel, a modulator and a component alike.
 * Numeric properties loaded from the panel file read back as numbers on a modulator but as
 * strings on a component. Unknown (non-schema) property names are accepted.
 */
TEST_F(LuaState, get_property_returns_the_type_that_was_stored)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);

    ASSERT_TRUE(runLua("local m = panel:getModulatorByName('target')\n"
                       "local c = m:getComponent()\n"
                       "local function kind(v) return type(v) .. ':' .. tostring(v) end\n"
                       "c:setPropertyString('myText', '42')\n"
                       "c:setPropertyInt('myInt', 42)\n"
                       "c:setProperty('myNum', 7, false)\n"
                       "m:setPropertyString('myModText', 'x')\n"
                       "panel:setPropertyInt('myPanelInt', 3)\n"
                       "_text = kind(c:getProperty('myText'))\n"
                       "_int = kind(c:getProperty('myInt'))\n"
                       "_num = kind(c:getProperty('myNum'))\n"
                       "_modText = kind(m:getProperty('myModText'))\n"
                       "_panelInt = kind(panel:getProperty('myPanelInt'))\n"
                       "_intAsString = L(c:getPropertyString('myInt'))\n"
                       "_textAsInt = c:getPropertyInt('myText')\n"
                       "_loadedMax = kind(m:getProperty('modulatorMax'))\n"
                       "_loadedSliderMax = kind(c:getProperty('uiSliderMax'))\n"
                       "_loadedVisibility = c:getPropertyInt('componentVisibility')\n"
                       "_callbackName = kind(m:getProperty('luaModulatorValueChange'))"));
    EXPECT_EQ(luaString("_text"), "string:42");
    EXPECT_EQ(luaString("_int"), "number:42");
    EXPECT_EQ(luaString("_num"), "number:7");
    EXPECT_EQ(luaString("_modText"), "string:x");
    EXPECT_EQ(luaString("_panelInt"), "number:3");
    EXPECT_EQ(luaString("_intAsString"), "42");
    EXPECT_EQ(luaNumber("_textAsInt"), 42.0);
    EXPECT_EQ(luaString("_loadedMax"), "number:127");
    // Characterization: the modulator's numeric properties are numbers, the component's are the
    // strings from the file (panels wrap them in tonumber()).
    EXPECT_EQ(luaString("_loadedSliderMax"), "string:127.0");
    EXPECT_EQ(luaNumber("_loadedVisibility"), 1.0);
    EXPECT_EQ(luaString("_callbackName"), "string:onTargetChange");
}

/**
 * The properties panels drive their UI with take effect on the JUCE component: uiSliderMin/Max
 * re-range the slider, componentVisibility hides it (in run mode), componentDisabled disables its
 * widget, uiLabelText and CtrlrLabel:setText are the same state, uiComboContent replaces items.
 * Also: setEnabled(false) disables the component without touching componentDisabled, and
 * console() (panels log through it) works without an editor.
 */
TEST_F(LuaState, ui_properties_set_from_lua_reach_the_components)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);
    ASSERT_NE(component("target"), nullptr);
    ASSERT_NE(component("aLabel"), nullptr);
    ASSERT_NE(component("aCombo"), nullptr);
    EXPECT_TRUE(component("target")->isVisible());

    ASSERT_TRUE(runLua("local c = panel:getComponent('target')\n"
                       "c:setPropertyString('uiSliderMin', '10')\n"
                       "c:setPropertyString('uiSliderMax', '20')\n"
                       "_min = c:getMinimum()\n"
                       "_max = c:getMaximum()\n"
                       "c:setPropertyString('componentVisibility', '0')\n"
                       "panel:getComponent('other'):setPropertyString('componentDisabled', '1')\n"
                       "local lbl = panel:getLabel('aLabel')\n"
                       "lbl:setPropertyString('uiLabelText', 'by property')\n"
                       "_labelText1 = L(lbl:getText())\n"
                       "lbl:setText('by setText')\n"
                       "_labelProp2 = L(lbl:getProperty('uiLabelText'))\n"
                       "panel:getComponent('aCombo'):setPropertyString('uiComboContent', 'a\\nb\\nc\\nd')\n"
                       "local lc = panel:getComponent('aLabel')\n"
                       "lc:setEnabled(false)\n"
                       "_disabledProp = lc:getPropertyInt('componentDisabled')\n"
                       "console('console() is callable headless')"));

    EXPECT_EQ(luaNumber("_min"), 10.0);
    EXPECT_EQ(luaNumber("_max"), 20.0);
    EXPECT_FALSE(component("target")->isVisible());
    auto* otherWidget = child<juce::Slider>(component("other"));
    ASSERT_NE(otherWidget, nullptr);
    EXPECT_FALSE(otherWidget->isEnabled());
    EXPECT_EQ(luaString("_labelText1"), "by property");
    EXPECT_EQ(luaString("_labelProp2"), "by setText");
    auto* comboBox = child<juce::ComboBox>(component("aCombo"));
    ASSERT_NE(comboBox, nullptr);
    EXPECT_EQ(comboBox->getNumItems(), 4);
    // setEnabled is JUCE's: it disables the component but leaves the componentDisabled property
    EXPECT_FALSE(component("aLabel")->isEnabled());
    EXPECT_EQ(luaNumber("_disabledProp"), 0.0);
}

/**
 * Selecting a combo item from Lua with notification moves the modulator value to the item
 * index; with dontNotify = true only the widget changes.
 */
TEST_F(LuaState, combo_set_selected_item_index_moves_the_modulator_value_only_when_notifying)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);

    ASSERT_TRUE(runLua("local cb = panel:getCombo('aCombo')\n"
                       "local m = panel:getModulatorByName('aCombo')\n"
                       "cb:setSelectedItemIndex(2, true)\n"
                       "_quietIndex = cb:getSelectedItemIndex()\n"
                       "_quietValue = m:getModulatorValue()\n"
                       "cb:setSelectedItemIndex(1, false)\n"
                       "_loudIndex = cb:getSelectedItemIndex()\n"
                       "_loudValue = m:getModulatorValue()\n"
                       "_text = L(cb:getText())"));
    pumpInputThreads(100);
    ASSERT_TRUE(runLua("_valueAfterPump = panel:getModulatorByName('aCombo'):getModulatorValue()"));

    EXPECT_EQ(luaNumber("_quietIndex"), 2.0);
    EXPECT_EQ(luaNumber("_quietValue"), 0.0);
    EXPECT_EQ(luaNumber("_loudIndex"), 1.0);
    EXPECT_EQ(luaNumber("_loudValue"), 1.0);
    EXPECT_EQ(luaString("_text"), "two");
    EXPECT_EQ(luaNumber("_valueAfterPump"), 1.0);
}

// ----------------------------------------------------------------------------------------------
// Colour
// ----------------------------------------------------------------------------------------------

/**
 * Colour properties the panels set (uiButtonTextColourOff/On) reach the button's JUCE colour
 * IDs, from the panel file and when changed from Lua. uiListBoxHighlightBgColour has no colour
 * ID: it is read at paint time, so only its property round-trip is observable here.
 */
TEST_F(LuaState, colour_properties_reach_the_juce_colour_ids)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);
    auto* textButton = child<juce::TextButton>(component("aButton"));
    ASSERT_NE(textButton, nullptr);

    EXPECT_EQ(textButton->findColour(juce::TextButton::textColourOffId).getARGB(), 0xff102030u);
    EXPECT_EQ(textButton->findColour(juce::TextButton::textColourOnId).getARGB(), 0xff405060u);

    ASSERT_TRUE(runLua("local b = panel:getComponent('aButton')\n"
                       "b:setPropertyString('uiButtonTextColourOff', 'ffaabbcc')\n"
                       "b:setPropertyString('uiButtonTextColourOn', 'ff00ff00')\n"
                       "local l = panel:getComponent('aList')\n"
                       "l:setPropertyString('uiListBoxHighlightBgColour', 'ff123456')\n"
                       "_highlight = L(l:getProperty('uiListBoxHighlightBgColour'))"));

    EXPECT_EQ(textButton->findColour(juce::TextButton::textColourOffId).getARGB(), 0xffaabbccu);
    EXPECT_EQ(textButton->findColour(juce::TextButton::textColourOnId).getARGB(), 0xff00ff00u);
    EXPECT_EQ(luaString("_highlight"), "ff123456");
}

/**
 * Component colour state through the JUCE Component binding: setColour then findColour returns
 * it, isColourSpecified tracks setColour/removeColour.
 */
TEST_F(LuaState, component_set_colour_find_colour_and_is_colour_specified)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);

    // An ID no LookAndFeel defines, so nothing but setColour can specify it.
    ASSERT_TRUE(runLua("local id = 0x7ff00001\n"
                       "local c = panel:getComponent('target')\n"
                       "_before = c:isColourSpecified(id) and 1 or 0\n"
                       "c:setColour(id, Colour(0xff112233))\n"
                       "_specified = c:isColourSpecified(id) and 1 or 0\n"
                       "_argb = c:findColour(id, false):getARGB()\n"
                       "c:removeColour(id)\n"
                       "_afterRemove = c:isColourSpecified(id) and 1 or 0"));

    EXPECT_EQ(luaNumber("_before"), 0.0);
    EXPECT_EQ(luaNumber("_specified"), 1.0);
    EXPECT_EQ(luaNumber("_argb"), (double) 0xff112233u);
    EXPECT_EQ(luaNumber("_afterRemove"), 0.0);
    EXPECT_EQ(component("target")->findColour(0x7ff00001).getARGB(), 0xff000000u)
        << "removed: back to JUCE's default for an unknown ID, opaque black";
}

// ----------------------------------------------------------------------------------------------
// Save / restore (plugin state) and program state
// ----------------------------------------------------------------------------------------------

/**
 * A host save + reload into a fresh instance: modulator values and custom properties set from
 * Lua come back; luaPanelSaveState / luaPanelRestoreState carry custom data (the pattern in the
 * manual: a MemoryBlock as base64 in the state tree); luaPanelLoaded runs again in the new
 * instance after the restore hook.
 * Characterization: restoring values runs no value-change callback, as during bootstrap (the
 * panel is in restore state meanwhile).
 */
TEST_F(LuaState, host_save_and_restore_round_trips_values_custom_data_and_skips_callbacks)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);
    EXPECT_EQ(count("_loads"), 1);

    ASSERT_TRUE(runLua("patch = MemoryBlock('F0 7D 01 02 03 F7')\n"
                       "panel:getModulatorByName('target'):setValue(42, false)\n"
                       "panel:getModulatorByName('other'):setValue(17, false)\n"
                       "panel:getComponent('target'):setPropertyString('myCustom', 'kept')\n"
                       "panel:getModulatorByName('other'):setPropertyString('myModCustom', 'also kept')"));
    pumpInputThreads(100);
    EXPECT_EQ(count("_targetCalls"), 1);

    reopenFromState();
    ASSERT_NE(panel, nullptr);

    ASSERT_TRUE(runLua("_target = panel:getModulatorByName('target'):getModulatorValue()\n"
                       "_other = panel:getModulatorByName('other'):getModulatorValue()\n"
                       "_custom = L(panel:getComponent('target'):getProperty('myCustom'))\n"
                       "_modCustom = L(panel:getModulatorByName('other'):getProperty('myModCustom'))\n"
                       "_patch = patch and patch:toHexString(1) or ''"));
    EXPECT_EQ(luaNumber("_target"), 42.0);
    EXPECT_EQ(luaNumber("_other"), 17.0);
    EXPECT_EQ(luaString("_custom"), "kept");
    EXPECT_EQ(luaString("_modCustom"), "also kept");
    EXPECT_EQ(count("_restoreCalls"), 1);
    EXPECT_EQ(luaString("_patch"), "f0 7d 01 02 03 f7");
    EXPECT_EQ(count("_targetCalls"), 0) << "restoring values runs no value-change callback";
    EXPECT_EQ(count("_otherCalls"), 0);
    EXPECT_EQ(count("_loads"), 1) << "the new instance's luaPanelLoaded";
}

/**
 * Program state is false in normal operation, also in value callbacks. Applying a program (the
 * value list getProgram() returns) with setProgram restores the values silently: no MIDI (unless
 * panelMidiSnapshotAfterProgramChange) and no value-change callback, because each modulator is in
 * restore state meanwhile. It then calls luaPanelProgramChanged, where getProgramState() is
 * true, and is false again afterwards.
 * Characterization: every panel load applies the panel's initial program the same way, so
 * luaPanelProgramChanged also runs once on load, before luaPanelLoaded.
 */
TEST_F(LuaState, program_state_is_true_only_while_a_program_is_applied)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);
    EXPECT_EQ(count("_programChangedCalls"), 1) << "the initial program, applied on load";
    EXPECT_EQ(luaNumber("_programChangedBeforeLoaded"), 1.0);

    ASSERT_TRUE(runLua("_idle = panel:getProgramState() and 1 or 0\n"
                       "panel:getModulatorByName('target'):setValue(33, false)"));
    pumpInputThreads(100);
    EXPECT_EQ(luaNumber("_idle"), 0.0);
    EXPECT_EQ(luaNumber("_targetProgramState"), 0.0);

    const juce::ValueTree program = panel->getProgram();
    ASSERT_TRUE(runLua("panel:getModulatorByName('target'):setValue(5, false)"));
    pumpInputThreads(100);
    ASSERT_TRUE(runLua("_targetCalls = 0\n_programChangedCalls = 0"));
    takeSendsAfterDelivery();

    panel->setProgram(program);
    ASSERT_TRUE(runLua("_after = panel:getProgramState() and 1 or 0\n"
                       "_value = panel:getModulatorByName('target'):getModulatorValue()"));
    pumpInputThreads(200);
    const auto sends = takeSendsAfterDelivery(150);

    EXPECT_EQ(luaNumber("_value"), 33.0);
    EXPECT_EQ(count("_targetCalls"), 0) << "no value-change callback while a program is applied";
    EXPECT_EQ(count("_programChangedCalls"), 1);
    EXPECT_EQ(luaNumber("_programChangedProgramState"), 1.0);
    EXPECT_EQ(luaNumber("_after"), 0.0);
    EXPECT_TRUE(sends.empty()) << "a modulator in restore state sends no MIDI";
}
