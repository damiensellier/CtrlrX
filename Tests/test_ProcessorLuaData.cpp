#include "test_ProcessorLuaFixture.h"

/**
 * Byte and data handling from Lua: MemoryBlock (hex round-trip, getByte/setByte, size),
 * BigInteger bit fields (getBitRangeAsInt / setBitRangeAsInt, used to pack SysEx), the data of a
 * CtrlrMidiMessage, and File I/O inside a temporary directory the test creates.
 */

class LuaData : public LuaPanel {};

/**
 * Wiring check for the suite: a MemoryBlock built from a hex string exposes its bytes.
 */
TEST_F(LuaData, memoryblock_from_hex_string_exposes_bytes)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local mb = MemoryBlock()\n"
                       "mb:loadFromHexString('F0 41 7F F7')\n"
                       "_size = mb:getSize()\n"
                       "_byte1 = mb:getByte(1)"));
    EXPECT_EQ(luaNumber("_size"), 4.0);
    EXPECT_EQ(luaNumber("_byte1"), 0x41);
}
