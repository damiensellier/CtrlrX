#include "test_ProcessorLuaFixture.h"

/**
 * The Lua `timer` object (CtrlrLuaMultiTimer): setCallback / startTimer / stopTimer /
 * isTimerRunning / isRegistered, a callback that stops its own timer, restarting with a new
 * interval, two timers. Assertions are on counts (>= n), never on wall-clock time.
 */

class LuaTimers : public LuaPanel {};

/**
 * Wiring check for the suite: the `timer` global exists in a loaded panel and an id that was
 * never registered reports as neither registered nor running.
 */
TEST_F(LuaTimers, unregistered_timer_id_is_neither_registered_nor_running)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_registered = timer:isRegistered(77) and 1 or 0\n"
                       "_running = timer:isTimerRunning(77) and 1 or 0"));
    EXPECT_EQ(luaNumber("_registered"), 0.0);
    EXPECT_EQ(luaNumber("_running"), 0.0);
}
