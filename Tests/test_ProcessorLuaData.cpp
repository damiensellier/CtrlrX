#include "test_ProcessorLuaFixture.h"

/**
 * Byte and data handling from Lua: MemoryBlock (hex round-trip, getByte/setByte, size),
 * BigInteger bit fields (getBitRangeAsInt / setBitRangeAsInt, used to pack SysEx), the data of a
 * CtrlrMidiMessage, and File I/O inside a temporary directory the test creates.
 */

class LuaData : public LuaPanel
{
protected:
    juce::File tempDir;

    void TearDown() override
    {
        if (tempDir != juce::File())
            tempDir.deleteRecursively();
        LuaPanel::TearDown();
    }

    // A fresh, empty directory under the system temp dir, also published to Lua as `_dir`.
    juce::File makeTempDir()
    {
        tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                      .getNonexistentChildFile("ctrlrx_lua_data_test", "", false);
        EXPECT_TRUE(tempDir.createDirectory().wasOk());
        EXPECT_TRUE(runLua("_dir = '" + tempDir.getFullPathName().replace("\\", "\\\\") + "'"));
        return tempDir;
    }

    // Send `message` from the mock device and let the panel's input thread process it.
    void injectFromDevice(const juce::MidiMessage& message)
    {
        processor->prepareToPlay(44100, BLOCK_SIZE);
        MockMidi::getInstance()->injectMidiInput(message);
        for (int i = 0; i < 24; i++)
            juce::MessageManager::getInstance()->runDispatchLoopUntil(25);
        for (int i = 0; i < 4; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(23));
            processor->processBlock(buffer, midiMessages);
            midiMessages.clear();
        }
        pumpInputThreads(100);
    }
};

// ----------------------------------------------------------------------------------------------
// MemoryBlock
// ----------------------------------------------------------------------------------------------

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

/**
 * Hex in, hex out: the string constructor and loadFromHexString accept any case and spacing,
 * toHexString(1) writes lower-case bytes separated by spaces and toHexString(0) without them.
 */
TEST_F(LuaData, memoryblock_hex_string_round_trip)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local a = MemoryBlock('F0 7d 01 0A F7')\n"
                       "_spaced = a:toHexString(1)\n"
                       "_packed = a:toHexString(0)\n"
                       "local b = MemoryBlock()\n"
                       "b:loadFromHexString(a:toHexString(1))\n"
                       "_sizeB = b:getSize()\n"
                       "_same = (b:toHexString(1) == _spaced) and 1 or 0"));
    EXPECT_EQ(luaString("_spaced"), "f0 7d 01 0a f7");
    EXPECT_EQ(luaString("_packed"), "f07d010af7");
    EXPECT_EQ(luaNumber("_sizeB"), 5.0);
    EXPECT_EQ(luaNumber("_same"), 1.0);
}

/**
 * Building and editing a buffer the way a librarian panel does: a zero-filled block of a fixed
 * size, setByte/getByte inside it, setSize to grow (zero-filled), append, getRange to slice.
 */
TEST_F(LuaData, memoryblock_set_get_resize_append_and_slice)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local mb = MemoryBlock(4, true)\n"
                       "_zeroed = mb:toHexString(1)\n"
                       "mb:setByte(0, 0xF0)\n"
                       "mb:setByte(3, 0x7F)\n"
                       "_edited = mb:toHexString(1)\n"
                       "mb:setSize(6, true)\n"
                       "_grown = mb:toHexString(1)\n"
                       "mb:append(MemoryBlock('F7'))\n"
                       "_appended = mb:toHexString(1)\n"
                       "_slice = mb:getRange(3, 2):toHexString(1)\n"
                       "_last = mb:getByte(mb:getSize() - 1)"));
    EXPECT_EQ(luaString("_zeroed"), "00 00 00 00");
    EXPECT_EQ(luaString("_edited"), "f0 00 00 7f");
    EXPECT_EQ(luaString("_grown"), "f0 00 00 7f 00 00");
    EXPECT_EQ(luaString("_appended"), "f0 00 00 7f 00 00 f7");
    EXPECT_EQ(luaString("_slice"), "7f 00");
    EXPECT_EQ(luaNumber("_last"), 0xF7);
}

/**
 * Out-of-range access is lenient (as the manual warns): getByte past the end returns 0, setByte
 * past the end is ignored, getRange past the end returns an empty block. Only offsets of at least
 * size + 1 are used here: the bounds check is `getSize() >= position`, so offset == size still
 * reads or writes one byte past the block (valgrind: invalid read / invalid write).
 */
TEST_F(LuaData, memoryblock_access_beyond_the_end_is_ignored)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local mb = MemoryBlock('01 02 03')\n"
                       "_get = mb:getByte(4) + mb:getByte(100)\n"
                       "mb:setByte(100, 0x55)\n"
                       "_afterSet = mb:toHexString(1)\n"
                       "_range = mb:getRange(2, 5):getSize()"));
    EXPECT_EQ(luaNumber("_get"), 0.0);
    EXPECT_EQ(luaString("_afterSet"), "01 02 03");
    EXPECT_EQ(luaNumber("_range"), 0.0);
}

// ----------------------------------------------------------------------------------------------
// BigInteger bit fields
// ----------------------------------------------------------------------------------------------

/**
 * Packing SysEx bit fields: setBitRangeAsInt writes fields side by side (one of them crossing
 * the byte boundary at bit 8), getBitRangeAsInt reads each back, toInteger gives the whole.
 */
TEST_F(LuaData, biginteger_bit_fields_round_trip_across_a_byte_boundary)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);

    ASSERT_TRUE(runLua("local b = BigInteger()\n"
                       "b:setBitRangeAsInt(0, 7, 0x55)\n"   // bits 0..6
                       "b:setBitRangeAsInt(7, 5, 0x1B)\n"   // bits 7..11: crosses into the 2nd byte
                       "b:setBitRangeAsInt(12, 1, 1)\n"     // bit 12
                       "_f0 = b:getBitRangeAsInt(0, 7)\n"
                       "_f1 = b:getBitRangeAsInt(7, 5)\n"
                       "_f2 = b:getBitRangeAsInt(12, 1)\n"
                       "_all = b:toInteger()\n"
                       "local c = BigInteger(_all)\n"
                       "_fromInt = c:getBitRangeAsInt(7, 5)\n"
                       "b:setBitRangeAsInt(7, 5, 0)\n"      // clearing one field leaves the others
                       "_cleared = b:toInteger()"));
    EXPECT_EQ(luaNumber("_f0"), 0x55);
    EXPECT_EQ(luaNumber("_f1"), 0x1B);
    EXPECT_EQ(luaNumber("_f2"), 1.0);
    EXPECT_EQ(luaNumber("_all"), 0x55 | (0x1B << 7) | (1 << 12));
    EXPECT_EQ(luaNumber("_fromInt"), 0x1B);
    EXPECT_EQ(luaNumber("_cleared"), 0x55 | (1 << 12));
}

// ----------------------------------------------------------------------------------------------
// MemoryBlock <-> CtrlrMidiMessage
// ----------------------------------------------------------------------------------------------

/** A MemoryBlock wrapped in a CtrlrMidiMessage reaches the device byte for byte. */
TEST_F(LuaData, memoryblock_sent_as_midi_message_reaches_the_device)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device output cannot be observed";

    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);
    takeSendsAfterDelivery(0);

    ASSERT_TRUE(runLua("local mb = MemoryBlock('F0 7D 01 02 03 F7')\n"
                       "local m = CtrlrMidiMessage(mb)\n"
                       "_size = m:getSize()\n"
                       "panel:sendMidiMessageNow(m)"));
    const auto sends = takeSendsAfterDelivery();

    const juce::uint8 sysex[] = { 0xF0, 0x7D, 0x01, 0x02, 0x03, 0xF7 };
    EXPECT_EQ(luaNumber("_size"), 6.0);
    EXPECT_EQ(sends, (std::vector<juce::MidiMessage>{ juce::MidiMessage(sysex, (int) sizeof(sysex)) }));
}

/**
 * luaPanelMidiReceived gets a CtrlrMidiMessage: getSize, getData and getLuaData (the same thing
 * under another name) expose the received bytes as a MemoryBlock.
 */
TEST_F(LuaData, panel_midi_received_exposes_the_message_bytes)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform; device input cannot be injected";

    ASSERT_NE(loadLuaPanel("fixture_lua_state.panel"), nullptr);

    const juce::uint8 sysex[] = { 0xF0, 0x7D, 0x11, 0x22, 0xF7 };
    injectFromDevice(juce::MidiMessage(sysex, (int) sizeof(sysex)));

    EXPECT_EQ(luaNumber("_rxCalls"), 1.0);
    EXPECT_EQ(luaNumber("_rxSize"), 5.0);
    EXPECT_EQ(luaString("_rxHex"), "f0 7d 11 22 f7");
    EXPECT_EQ(luaNumber("_rxByte2"), 0x11);
}

// ----------------------------------------------------------------------------------------------
// File I/O, only inside a temporary directory the test creates and removes
// ----------------------------------------------------------------------------------------------

/** Text and MemoryBlock data written from Lua read back unchanged; the file queries agree. */
TEST_F(LuaData, file_text_and_data_round_trip_in_a_temp_dir)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);
    const juce::File dir = makeTempDir();

    ASSERT_TRUE(runLua("local d = File(_dir)\n"
                       "_dirOk = (d:isDirectory() and d:isValid()) and 1 or 0\n"
                       "local t = d:getChildFile('notes.txt')\n"
                       "_created = t:create():wasOk() and 1 or 0\n"
                       "_wroteText = t:replaceWithText('hello\\nworld', false, false, '\\n') and 1 or 0\n"
                       "_text = t:loadFileAsString()\n"
                       "local b = d:getChildFile('patch.syx')\n"
                       "_wroteData = b:replaceWithData(MemoryBlock('F0 7D 00 01 F7')) and 1 or 0\n"
                       "local mb = MemoryBlock()\n"
                       "_readData = b:loadFileAsData(mb) and 1 or 0\n"
                       "_data = mb:toHexString(1)\n"
                       "_fileSize = b:getSize()\n"
                       "_isFile = b:existsAsFile() and 1 or 0\n"
                       "_name = b:getFileName()\n"
                       "_parentSame = (b:getParentDirectory():getFullPathName() == d:getFullPathName()) and 1 or 0"));

    EXPECT_EQ(luaNumber("_dirOk"), 1.0);
    EXPECT_EQ(luaNumber("_created"), 1.0);
    EXPECT_EQ(luaNumber("_wroteText"), 1.0);
    EXPECT_EQ(luaString("_text"), "hello\nworld");
    EXPECT_EQ(luaNumber("_wroteData"), 1.0);
    EXPECT_EQ(luaNumber("_readData"), 1.0);
    EXPECT_EQ(luaString("_data"), "f0 7d 00 01 f7");
    EXPECT_EQ(luaNumber("_fileSize"), 5.0);
    EXPECT_EQ(luaNumber("_isFile"), 1.0);
    EXPECT_EQ(luaString("_name"), "patch.syx");
    EXPECT_EQ(luaNumber("_parentSame"), 1.0);

    // and the files really are on disk where Lua said
    EXPECT_EQ(dir.getChildFile("notes.txt").loadFileAsString(), "hello\nworld");
    EXPECT_EQ(dir.getChildFile("patch.syx").getSize(), 5);
}

/**
 * findChildFiles fills a Lua table (1-based) with File objects; File.findFiles and a wildcard
 * filter as in JUCE. A default-constructed path is not valid; getSpecialLocation returns a path.
 */
TEST_F(LuaData, file_find_child_files_and_special_location)
{
    ASSERT_NE(loadLuaPanel("fixture_lua_values.panel"), nullptr);
    const juce::File dir = makeTempDir();
    ASSERT_TRUE(dir.getChildFile("a.syx").replaceWithText("a"));
    ASSERT_TRUE(dir.getChildFile("b.syx").replaceWithText("bb"));
    ASSERT_TRUE(dir.getChildFile("c.txt").replaceWithText("ccc"));
    ASSERT_TRUE(dir.getChildFile("sub").createDirectory().wasOk());

    ASSERT_TRUE(runLua("local found = {}\n"
                       "File(_dir):findChildFiles(found, File.findFiles, false, '*.syx')\n"
                       "_count = #found\n"
                       "local names, bytes = {}, 0\n"
                       "for _, f in ipairs(found) do names[#names + 1] = f:getFileName(); bytes = bytes + f:getSize() end\n"
                       "table.sort(names)\n"
                       "_names = table.concat(names, ',')\n"
                       "_bytes = bytes\n"
                       "local all = {}\n"
                       "File(_dir):findChildFiles(all, File.findFilesAndDirectories, false, '*')\n"
                       "_all = #all\n"
                       "_tempPath = File.getSpecialLocation(File.tempDirectory):getFullPathName()\n"
                       "_emptyValid = File(''):isValid() and 1 or 0"));

    EXPECT_EQ(luaNumber("_count"), 2.0);
    EXPECT_EQ(luaString("_names"), "a.syx,b.syx");
    EXPECT_EQ(luaNumber("_bytes"), 3.0);
    EXPECT_EQ(luaNumber("_all"), 4.0);
    EXPECT_FALSE(luaString("_tempPath").empty());
    EXPECT_EQ(luaNumber("_emptyValid"), 0.0);
}
