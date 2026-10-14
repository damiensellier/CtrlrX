#include "test_ProcessorLuaFixture.h"

/**
 * Panel, modulator and component state reachable from Lua: properties (setPropertyString /
 * getProperty and friends, incl. the ones with side effects like uiSliderMin/Max and
 * componentVisibility), component colour state, save/restore and program state.
 */

class LuaState : public LuaPanel {};

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
