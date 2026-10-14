#include "test_ProcessorLuaFixture.h"

/**
 * Lua value paths and the value-change callbacks they fire.
 *
 * Pins, per path (mod:setValue, mod:setModulatorValue with its vst/midi/ui flags, host
 * parameter, incoming MIDI), the value afterwards, whether luaModulatorValueChange ran, when
 * (inside the calling chunk or only after the message loop ran), the `source` it received and
 * the MIDI emitted. Also lookups (getModulatorByName and the typed component getters).
 *
 * Fixtures:
 *  - fixture_lua_values : `target` / `other`, each with a callback recording
 *                         _<name>Calls / _<name>Value / _<name>Source as Lua globals
 */

class LuaValues : public LuaPanel {};

/**
 * Wiring check for the suite: the fixture loads, a chunk runs in the panel's Lua state, and a
 * value set from Lua reads back from Lua.
 */
TEST_F(LuaValues, lua_chunk_sets_and_reads_back_a_modulator_value)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local m = panel:getModulatorByName('target')\n"
                       "m:setValue(42, true)\n"
                       "_readBack = m:getValueInt()"));
    EXPECT_EQ(luaNumber("_readBack"), 42.0);
}
