#include "test_ProcessorLuaFixture.h"

/**
 * The Lua `timer` object (CtrlrLuaMultiTimer): setCallback / startTimer / stopTimer /
 * isTimerRunning / isRegistered, a callback that stops its own timer, restarting with a new
 * interval, two timers. Assertions are on counts (>= n), never on wall-clock time: intervals
 * are 10 ms and the message loop is pumped for 200 ms, so ">= 3" has a wide margin.
 *
 * Timer callbacks run on the message thread while the loop is pumped, never inside the chunk
 * that started the timer.
 *
 * Not covered: a callback that raises a Lua error. CtrlrLuaMultiTimer::timerCallback disables
 * it and reports through a modal AlertWindow, which blocks a headless test.
 */

class LuaTimers : public LuaPanel
{
protected:
    void TearDown() override
    {
        if (panel != nullptr)
            runLua("for id = 1, 10 do timer:stopTimer(id) end");
        LuaPanel::TearDown();
    }

    int count(const char* global) { return luaIsNil(global) ? 0 : (int) luaNumber(global); }
};

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

/**
 * A registered, started timer fires repeatedly with its id as the argument, only once the
 * message loop runs, until stopTimer; after that it no longer fires and is no longer running,
 * but stays registered.
 */
TEST_F(LuaTimers, timer_fires_repeatedly_with_its_id_until_stopped)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_fires, _arg = 0, -1\n"
                       "timer:setCallback(1, function(id) _fires = _fires + 1 _arg = id end)\n"
                       "_registered = timer:isRegistered(1) and 1 or 0\n"
                       "_runningBeforeStart = timer:isTimerRunning(1) and 1 or 0\n"
                       "timer:startTimer(1, 10)\n"
                       "_running = timer:isTimerRunning(1) and 1 or 0\n"
                       "_interval = timer:getTimerInterval(1)\n"
                       "_firesInChunk = _fires"));
    EXPECT_EQ(luaNumber("_registered"), 1.0);
    EXPECT_EQ(luaNumber("_runningBeforeStart"), 0.0) << "setCallback alone does not start it";
    EXPECT_EQ(luaNumber("_running"), 1.0);
    EXPECT_EQ(luaNumber("_interval"), 10.0);
    EXPECT_EQ(luaNumber("_firesInChunk"), 0.0);

    pumpInputThreads(200);
    EXPECT_GE(count("_fires"), 3);
    EXPECT_EQ(luaNumber("_arg"), 1.0) << "the callback receives the timer id";

    ASSERT_TRUE(runLua("timer:stopTimer(1)\n"
                       "_firesAtStop = _fires\n"
                       "_runningAfterStop = timer:isTimerRunning(1) and 1 or 0\n"
                       "_registeredAfterStop = timer:isRegistered(1) and 1 or 0"));
    pumpInputThreads(150);
    EXPECT_EQ(count("_fires"), count("_firesAtStop")) << "no fire after stopTimer";
    EXPECT_EQ(luaNumber("_runningAfterStop"), 0.0);
    EXPECT_EQ(luaNumber("_registeredAfterStop"), 1.0);
}

/** A callback that stops its own timer runs exactly once (the "one-shot" pattern). */
TEST_F(LuaTimers, callback_stopping_its_own_timer_fires_once)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_fires = 0\n"
                       "timer:setCallback(2, function(id) _fires = _fires + 1 timer:stopTimer(id) end)\n"
                       "timer:startTimer(2, 10)"));
    pumpInputThreads(200);
    ASSERT_TRUE(runLua("_running = timer:isTimerRunning(2) and 1 or 0"));

    EXPECT_EQ(count("_fires"), 1);
    EXPECT_EQ(luaNumber("_running"), 0.0);
}

/**
 * startTimer on a running timer replaces its interval (the "restart the timeout" pattern): a
 * 5 s timer restarted at 10 ms fires within the 200 ms pump.
 */
TEST_F(LuaTimers, starting_a_running_timer_again_replaces_its_interval)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_fires = 0\n"
                       "timer:setCallback(3, function(id) _fires = _fires + 1 end)\n"
                       "timer:startTimer(3, 5000)\n"
                       "timer:startTimer(3, 10)\n"
                       "_interval = timer:getTimerInterval(3)"));
    pumpInputThreads(200);

    EXPECT_EQ(luaNumber("_interval"), 10.0);
    EXPECT_GE(count("_fires"), 3);
}

/** A timer callback can stop itself and start another timer, which then fires (a two-step sequence). */
TEST_F(LuaTimers, timer_callback_can_start_another_timer)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_first, _second = 0, 0\n"
                       "timer:setCallback(5, function(id) _second = _second + 1 timer:stopTimer(5) end)\n"
                       "timer:setCallback(4, function(id) _first = _first + 1 timer:stopTimer(4) timer:startTimer(5, 10) end)\n"
                       "timer:startTimer(4, 10)"));
    pumpInputThreads(300);

    EXPECT_EQ(count("_first"), 1);
    EXPECT_EQ(count("_second"), 1);
}

/** Two timers run independently: stopping one leaves the other firing. */
TEST_F(LuaTimers, two_timers_run_independently)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_a, _b = 0, 0\n"
                       "timer:setCallback(6, function(id) _a = _a + 1 end)\n"
                       "timer:setCallback(7, function(id) _b = _b + 1 end)\n"
                       "timer:startTimer(6, 10)\n"
                       "timer:startTimer(7, 10)"));
    pumpInputThreads(200);
    ASSERT_TRUE(runLua("timer:stopTimer(6)\n"
                       "_aAtStop, _bAtStop = _a, _b"));
    EXPECT_GE(count("_aAtStop"), 3);
    EXPECT_GE(count("_bAtStop"), 3);

    pumpInputThreads(200);
    EXPECT_EQ(count("_a"), count("_aAtStop")) << "the stopped timer no longer fires";
    EXPECT_GE(count("_b"), count("_bAtStop") + 3) << "the other one still does";
}

/** setCallback on an id that already has one replaces it; only the new callback runs. */
TEST_F(LuaTimers, setting_a_callback_again_replaces_it)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("_old, _new = 0, 0\n"
                       "timer:setCallback(8, function(id) _old = _old + 1 end)\n"
                       "timer:setCallback(8, function(id) _new = _new + 1 timer:stopTimer(id) end)\n"
                       "timer:startTimer(8, 10)"));
    pumpInputThreads(200);

    EXPECT_EQ(count("_old"), 0);
    EXPECT_EQ(count("_new"), 1);
}

/** startTimer on an id without a callback is harmless: the timer runs but is not registered. */
TEST_F(LuaTimers, timer_without_callback_runs_but_is_not_registered)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("timer:startTimer(9, 10)\n"
                       "_running = timer:isTimerRunning(9) and 1 or 0\n"
                       "_registered = timer:isRegistered(9) and 1 or 0"));
    pumpInputThreads(100);

    EXPECT_EQ(luaNumber("_running"), 1.0);
    EXPECT_EQ(luaNumber("_registered"), 0.0);
}

/**
 * A value set from a timer callback runs the modulator's luaModulatorValueChange once, with
 * source Lua (5) -- the same as a set from any other Lua code.
 */
TEST_F(LuaTimers, value_set_from_a_timer_callback_reports_source_lua)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);
    pumpInputThreads(50);

    ASSERT_TRUE(runLua("_targetCalls = nil\n"
                       "timer:setCallback(10, function(id)\n"
                       "  timer:stopTimer(id)\n"
                       "  panel:getModulatorByName('target'):setValue(77, false)\n"
                       "end)\n"
                       "timer:startTimer(10, 10)"));
    pumpInputThreads(300);

    EXPECT_EQ(count("_targetCalls"), 1);
    EXPECT_EQ(luaNumber("_targetValue"), 77.0);
    EXPECT_EQ(luaNumber("_targetSource"), 5.0);
}
