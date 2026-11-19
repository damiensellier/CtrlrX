#include "test_ProcessorLuaFixture.h"

#include "CtrlrManager.h"
#include "CtrlrPanel.h"

#include <chrono>
#include <sstream>
#include <string>
#include <vector>

/**
 * Long SysEx on every MIDI path.
 *
 * A user reported that CtrlrX crashed on a single 70k SysEx dump from a synth
 * (https://github.com/RomanKubiak/ctrlr/discussions/791#discussioncomment-19573214). These tests send
 * one SysEx of N bytes over each of the 8 paths and check that it arrives exactly once and intact.
 *
 * Message: N bytes in total, F0 7D <payload> F7, payload byte i = (i*31 + floor(i/127)) % 128.
 * It is followed on the same path by the sentinel F0 7D 00 F7; once the sentinel reaches the sink,
 * everything sent before it has arrived too, so the test waits for the sentinel (with a generous
 * ceiling for slow CI runners) instead of sleeping a fixed time.
 *
 * Paths, each isolated through the panel's MIDI options (fixture_long_sysex.panel):
 *   device -> Lua, host -> Lua            luaPanelMidiReceived checks the bytes in Lua (rx_* globals)
 *   Lua -> device, Lua -> host            panel:sendMidiMessageNow
 *   thru D2D, D2H, H2D, H2H               panelMidiThru* flags
 *
 * Sinks:
 *  - Lua and device: exactly one copy, byte-identical, nothing else.
 *  - Host output: at least one copy, every copy byte-identical, nothing else. Host input is also
 *    echoed to the host output unconditionally (CtrlrProcessor::processBlock), so the copy count
 *    there belongs to the MidiRouting tests and the MIDI rework (Doc/midi/, R2), not to these.
 *    For the same reason "thru H2H" is really "host in -> host out by any route".
 *  - Lua -> host cannot be isolated from Lua -> device: CtrlrPanel::sendMidi(CtrlrMidiMessage&)
 *    sends nothing without an open output device, and with one it always sends there too.
 *
 * Seam: black-box -- injectMidiInput / processBlock / Lua in; Lua globals, StrictMock device and
 * host MidiBuffer out. No production code is touched.
 */

namespace {
enum class LsxPath
{
    DeviceToLua,
    HostToLua,
    LuaToDevice,
    LuaToHost,
    ThruD2D,
    ThruD2H,
    ThruH2D,
    ThruH2H
};

enum class Source
{
    Device,
    Host,
    Lua
};
enum class Sink
{
    Lua,
    Device,
    Host
};

const char *pathName(LsxPath p)
{
    switch (p) {
    case LsxPath::DeviceToLua:
        return "device_to_lua";
    case LsxPath::HostToLua:
        return "host_to_lua";
    case LsxPath::LuaToDevice:
        return "lua_to_device";
    case LsxPath::LuaToHost:
        return "lua_to_host";
    case LsxPath::ThruD2D:
        return "thru_d2d";
    case LsxPath::ThruD2H:
        return "thru_d2h";
    case LsxPath::ThruH2D:
        return "thru_h2d";
    case LsxPath::ThruH2H:
        return "thru_h2h";
    }
    return "?";
}

Source sourceOf(LsxPath p)
{
    switch (p) {
    case LsxPath::DeviceToLua:
    case LsxPath::ThruD2D:
    case LsxPath::ThruD2H:
        return Source::Device;
    case LsxPath::HostToLua:
    case LsxPath::ThruH2D:
    case LsxPath::ThruH2H:
        return Source::Host;
    default:
        return Source::Lua;
    }
}

Sink sinkOf(LsxPath p)
{
    switch (p) {
    case LsxPath::DeviceToLua:
    case LsxPath::HostToLua:
        return Sink::Lua;
    case LsxPath::LuaToDevice:
    case LsxPath::ThruD2D:
    case LsxPath::ThruH2D:
        return Sink::Device;
    default:
        return Sink::Host;
    }
}

// Same message the fixture's lsx_message(n) builds.
juce::MidiMessage longSysex(int n)
{
    std::vector<juce::uint8> d((size_t)n);
    d[0] = 0xF0;
    d[1] = 0x7D;
    for (int i = 0; i <= n - 4; i++)
        d[(size_t)i + 2] = (juce::uint8)((i * 31 + i / 127) % 128);
    d[(size_t)n - 1] = 0xF7;
    return juce::MidiMessage(d.data(), n);
}

juce::MidiMessage sentinel()
{
    const juce::uint8 raw[] = {0xF0, 0x7D, 0x00, 0xF7};
    return juce::MidiMessage(raw, (int)sizeof(raw));
}

// First byte index where `m` differs from `expected`; -1 = identical (sizes included).
int firstMismatch(const juce::MidiMessage &m, const juce::MidiMessage &expected)
{
    const int n = juce::jmin(m.getRawDataSize(), expected.getRawDataSize());
    for (int i = 0; i < n; i++)
        if (m.getRawData()[i] != expected.getRawData()[i])
            return i;
    return m.getRawDataSize() == expected.getRawDataSize() ? -1 : n;
}
} // namespace

/** What arrived at the sink of one transfer. */
struct LsxOutcome
{
        bool sentinelArrived = false;
        double elapsedMs = 0;
        int exactCopies = 0;       // copies byte-identical to the sent message
        int badMessages = 0;       // anything else besides the sentinel
        int firstBadSize = -1;     // size of the first such message
        int firstBadMismatch = -1; // first differing byte index in it (vs the sent message)

        // Lua sinks report through the fixture's rx_* globals instead.
        bool fromLua = false;
        int luaCount = 0, luaSize = 0, luaFirstMismatch = -1, luaOther = 0;

        std::string describe() const
        {
            std::ostringstream s;
            s << (sentinelArrived ? "sentinel arrived" : "SENTINEL NEVER ARRIVED (path stalled or buffer corrupted)")
              << " after " << (int)elapsedMs << " ms; ";
            if (fromLua)
                s << "Lua: rx_count=" << luaCount << " rx_size=" << luaSize << " rx_first_mismatch=" << luaFirstMismatch
                  << " rx_other=" << luaOther;
            else
                s << "exact copies=" << exactCopies << " other messages=" << badMessages
                  << " (first: size=" << firstBadSize << " first mismatch at byte " << firstBadMismatch << ")";
            return s.str();
        }
};

class LongSysex : public LuaPanel
{
    protected:
        // Ceiling for the sentinel. Generous: GitHub runners can be slow, and a passing transfer ends
        // as soon as the sentinel shows up. Stays below the ctest TIMEOUT (120 s, Tests/CMakeLists.txt).
        static constexpr int kSentinelCeilingMs = 60000;

        std::vector<juce::MidiMessage> hostOut;

        // Panel MIDI options for one path: only that path's route on.
        void isolatePath(LsxPath path)
        {
            panel->setProperty(Ids::panelMidiThruD2D, path == LsxPath::ThruD2D, false);
            panel->setProperty(Ids::panelMidiThruD2H, path == LsxPath::ThruD2H, false);
            panel->setProperty(Ids::panelMidiThruH2D, path == LsxPath::ThruH2D, false);
            panel->setProperty(Ids::panelMidiThruH2H, path == LsxPath::ThruH2H, false);
            panel->setProperty(Ids::panelMidiOutputToHost, sinkOf(path) == Sink::Host, false);
            panel->setProperty(Ids::panelMidiInputFromHost, true, false);
            panel->setProperty(Ids::panelMidiInputFromHostCompare, path == LsxPath::HostToLua, false);
        }

        // One host block carrying `m` (or nothing), collecting the host output.
        void hostBlock(const juce::MidiMessage *m)
        {
            midiMessages.clear();
            if (m != nullptr)
                midiMessages.addEvent(*m, 0);
            processor->processBlock(buffer, midiMessages);
            for (const auto meta : midiMessages)
                hostOut.push_back(meta.getMessage());
            midiMessages.clear();
        }

        std::vector<juce::MidiMessage> sinkMessages(Sink sink)
        {
            if (sink == Sink::Host)
                return hostOut;
            std::lock_guard<std::mutex> l(sendsMutex);
            return deviceSends;
        }

        bool sentinelSeen(Sink sink)
        {
            if (sink == Sink::Lua) {
                const double v = luaNumber("rx_sentinel");
                return v == v && v >= 1; // not NaN
            }
            return count_equal(sinkMessages(sink), sentinel()) > 0;
        }

        /** Send one SysEx of `n` bytes, then the sentinel, over `path`; wait for the sentinel. */
        LsxOutcome transfer(LsxPath path, int n, int ceilingMs = kSentinelCeilingMs)
        {
            LsxOutcome out;
            const Source source = sourceOf(path);
            const Sink sink = sinkOf(path);

            isolatePath(path);
            pumpInputThreads(100);
            EXPECT_TRUE(runLua("lsx_reset()"));
            hostBlock(nullptr);
            hostOut.clear();
            {
                std::lock_guard<std::mutex> l(sendsMutex);
                deviceSends.clear();
            }

            const juce::MidiMessage message = longSysex(n);
            // Each path delivers in order, so once this short sentinel arrives the long message must
            // have arrived too. We wait for it instead of a fixed sleep: fast locally, robust on slow CI.
            const juce::MidiMessage end = sentinel();
            const auto start = std::chrono::steady_clock::now();

            switch (source) {
            case Source::Device:
                MockMidi::getInstance()->injectMidiInput(message);
                MockMidi::getInstance()->injectMidiInput(end);
                break;
            case Source::Host:
                hostBlock(&message);
                hostBlock(&end);
                break;
            case Source::Lua:
                EXPECT_TRUE(runLua("lsx_send(" + juce::String(n) + ")"));
                break;
            }

            // One "block" per iteration: pump the message loop (device input, Lua callbacks, the
            // host-output collector) and run an empty host block to collect host output.
            while (!(out.sentinelArrived = sentinelSeen(sink))) {
                const auto waited = std::chrono::steady_clock::now() - start;
                if (std::chrono::duration_cast<std::chrono::milliseconds>(waited).count() > ceilingMs)
                    break;
                juce::MessageManager::getInstance()->runDispatchLoopUntil(23);
                hostBlock(nullptr);
            }
            out.elapsedMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();

            if (sink == Sink::Lua) {
                out.fromLua = true;
                out.luaCount = (int)luaNumber("rx_count");
                out.luaSize = (int)luaNumber("rx_size");
                out.luaFirstMismatch = (int)luaNumber("rx_first_mismatch");
                out.luaOther = (int)luaNumber("rx_other");
                return out;
            }

            for (const auto &m : sinkMessages(sink)) {
                if (m == end)
                    continue;
                if (m == message) {
                    out.exactCopies++;
                    continue;
                }
                if (out.badMessages++ == 0) {
                    out.firstBadSize = m.getRawDataSize();
                    out.firstBadMismatch = firstMismatch(m, message);
                }
            }
            return out;
        }

        /** The pass rule for `path` (see the file comment). */
        static bool arrivedIntact(LsxPath path, int n, const LsxOutcome &o)
        {
            if (!o.sentinelArrived)
                return false;
            switch (sinkOf(path)) {
            case Sink::Lua:
                return o.luaCount == 1 && o.luaSize == n && o.luaFirstMismatch == -1 && o.luaOther == 0;
            case Sink::Device:
                return o.exactCopies == 1 && o.badMessages == 0;
            case Sink::Host:
                return o.exactCopies >= 1 && o.badMessages == 0;
            }
            return false;
        }

        // Send one SysEx of `n` bytes over `path` and check that it arrived intact.
        void run(LsxPath path, int n)
        {
            if (needsDevice(path) && !midi_mock.hasSubsystemMock())
                GTEST_SKIP() << "no MIDI subsystem mock on this platform";

            ASSERT_NE(loadLuaPanel("fixture_long_sysex.panel"), nullptr);
            ASSERT_FALSE(luaIsNil("lsx_send")) << "fixture Lua did not load";
            const LsxOutcome o = transfer(path, n);
            EXPECT_TRUE(arrivedIntact(path, n, o)) << pathName(path) << ", " << n << " bytes: " << o.describe();
        }

        static bool needsDevice(LsxPath path)
        {
            return sourceOf(path) != Source::Host || sinkOf(path) != Sink::Host;
        }
};

// ----------------------------------------------------------------------------------------------
// Measured limits before #350 (5.6.37, JUCE 6.0.8; the same on Linux, macOS and Windows CI):
//
//   path                                                last size OK   first broken
//   device -> Lua, host -> Lua, Lua -> host,
//   thru D2D, thru D2H                                  65529          65530: hangs
//   Lua -> device                                       65535          65536
//   thru H2D, thru H2H                                  65529 (*)      65530: hangs on Windows
//
// Cause: juce::MidiBuffer stores each event's size as a uint16, and computes an event's total size
// (data + 6 header bytes) as a uint16 too (juce_MidiBuffer.cpp; JUCE 8 is the same). At 65530 bytes
// that total wraps to 0, so a walk over the buffer (findEventAfter, getNumEvents, ...) stops
// advancing: e.g. the MIDI input thread spins in CtrlrPanelMIDIInputThread::handleMIDIFromDevice
// while it holds the panel lock. Larger messages make the walk read the payload as event headers;
// that mostly crashes.
// (*) Linux and macOS currently get through up to 65535 here only because GCC and Clang may assume
//     that the side-effect-free loop in MidiBuffer::getNumEvents() ends (undefined behaviour), while
//     MSVC spins. So the green size is 65529 on every platform.
//
// Since #350 + #352, messages over 65529 bytes bypass MidiBuffer on the device and Lua paths, so
// device -> Lua, Lua -> device and thru D2D pass at 65530 and at 128 KiB; the _65530 cases pin the
// start of the bypass. Host output over 65529 bytes is dropped on purpose (logged), and host input
// arrives in the host's MidiBuffer, so the host-side 128 KiB cases still fail.
// The bypass first sent the big message at once on the sending thread, while JUCE's MidiOutput
// thread sent the short ones; on macOS thru D2D then arrived broken. Since the ordered-send fix every
// message goes through JUCE's queue, in order: see LongSysexOrder below.
//
// Per path: a green case at the largest size that works, and a red case at 128 KiB: the acceptance
// criterion for a fix (the reported dump was 70k). The red cases are allowlisted in
// Tests/known_failures_{linux,macos,windows}.txt; some of them hang until the ctest TIMEOUT instead
// of crashing.
// ----------------------------------------------------------------------------------------------

#define LONG_SYSEX_CASE(path_name, path, size)                                                                         \
    TEST_F(LongSysex, path_name##_##size)                                                                              \
    {                                                                                                                  \
        run(path, size);                                                                                               \
    }

LONG_SYSEX_CASE(device_to_lua, LsxPath::DeviceToLua, 65529)
LONG_SYSEX_CASE(device_to_lua, LsxPath::DeviceToLua, 131072)
LONG_SYSEX_CASE(device_to_lua, LsxPath::DeviceToLua, 65530)
LONG_SYSEX_CASE(host_to_lua, LsxPath::HostToLua, 65529)
LONG_SYSEX_CASE(host_to_lua, LsxPath::HostToLua, 131072)
LONG_SYSEX_CASE(lua_to_device, LsxPath::LuaToDevice, 65535)
LONG_SYSEX_CASE(lua_to_device, LsxPath::LuaToDevice, 131072)
LONG_SYSEX_CASE(lua_to_host, LsxPath::LuaToHost, 65529)
LONG_SYSEX_CASE(lua_to_host, LsxPath::LuaToHost, 131072)
LONG_SYSEX_CASE(thru_d2d, LsxPath::ThruD2D, 65529)
LONG_SYSEX_CASE(thru_d2d, LsxPath::ThruD2D, 131072)
LONG_SYSEX_CASE(thru_d2d, LsxPath::ThruD2D, 65530)
LONG_SYSEX_CASE(thru_d2h, LsxPath::ThruD2H, 65529)
LONG_SYSEX_CASE(thru_d2h, LsxPath::ThruD2H, 131072)
LONG_SYSEX_CASE(thru_h2d, LsxPath::ThruH2D, 65529)
LONG_SYSEX_CASE(thru_h2d, LsxPath::ThruH2D, 131072)
LONG_SYSEX_CASE(thru_h2h, LsxPath::ThruH2H, 65529)
LONG_SYSEX_CASE(thru_h2h, LsxPath::ThruH2H, 131072)

// ----------------------------------------------------------------------------------------------
// Device output order with a long SysEx.
//
// A device has one MIDI output, so messages must leave it one at a time and in the order they
// were sent. The two cases with a global delay and thru D2D "long then marker" failed every time
// before 5.6.37 queued long messages with MidiOutput::sendMessageAt(); the other cases pin the order
// without a delay.
// #350 sent messages over 65529 bytes with MidiOutput::sendMessageNow() on the sending
// thread, while every other message goes through JUCE's MidiOutput thread (sendBlockOfMessages):
//  - the long message skipped panelMidiGlobalDelay and overtook short messages sent before it;
//  - thru D2D handled a batch's short messages before its long ones, whatever the input order;
//  - two threads sent to one port at once. On macOS 11+ JUCE sends a SysEx in many CoreMIDI calls,
//    so a short message could land inside the long one, and the receiver dropped it (upstream
//    run 43745732738: thru_d2d_131072 "exact copies=0").
// The marker F0 7D 01 F7 is the short message; the sentinel still comes last.
// ----------------------------------------------------------------------------------------------

namespace {
juce::MidiMessage marker()
{
    const juce::uint8 raw[] = {0xF0, 0x7D, 0x01, 0xF7};
    return juce::MidiMessage(raw, (int)sizeof(raw));
}
} // namespace

class LongSysexOrder : public LongSysex
{
    protected:
        enum class First
        {
            Marker,
            Long
        };

        /** Where each message landed in the device output. */
        struct Order
        {
                bool sentinelArrived = false;
                int markerAt = -1, longAt = -1, sentinelAt = -1; // index in the device output; -1 = absent
                int markers = 0, longs = 0, sentinels = 0, others = 0;

                std::string describe() const
                {
                    std::ostringstream s;
                    s << (sentinelArrived ? "sentinel arrived" : "SENTINEL NEVER ARRIVED") << "; at index: marker="
                      << markerAt << " long=" << longAt << " sentinel=" << sentinelAt << "; counts: marker=" << markers
                      << " long=" << longs << " sentinel=" << sentinels << " other=" << others;
                    return s.str();
                }
        };

        /**
         * Over `path` (LuaToDevice or ThruD2D), with panelMidiGlobalDelay = `delayMs`: send the marker
         * and an `n`-byte SysEx in the order `first`, then the sentinel. Wait for the sentinel, then a
         * while longer, so a message that comes late is told apart from one that is lost.
         */
        Order sendPair(LsxPath path, First first, int n, int delayMs)
        {
            isolatePath(path);
            panel->setProperty(Ids::panelMidiGlobalDelay, delayMs, false);
            pumpInputThreads(100);
            {
                std::lock_guard<std::mutex> l(sendsMutex);
                deviceSends.clear();
            }

            const juce::MidiMessage big = longSysex(n);
            if (path == LsxPath::ThruD2D) {
                auto *mock = MockMidi::getInstance();
                mock->injectMidiInput(first == First::Marker ? marker() : big);
                mock->injectMidiInput(first == First::Marker ? big : marker());
                mock->injectMidiInput(sentinel());
            } else {
                const juce::String sendMarker = "panel:sendMidiMessageNow(CtrlrMidiMessage(MemoryBlock({0xF0, 0x7D, 0x01, 0xF7}))) ";
                const juce::String sendLong = "panel:sendMidiMessageNow(lsx_message(" + juce::String(n) + ")) ";
                EXPECT_TRUE(runLua((first == First::Marker ? sendMarker + sendLong : sendLong + sendMarker) +
                                   "panel:sendMidiMessageNow(lsx_sentinel())"));
            }

            Order o;
            const auto start = std::chrono::steady_clock::now();
            while (!(o.sentinelArrived = sentinelSeen(Sink::Device))) {
                const auto waited = std::chrono::steady_clock::now() - start;
                if (std::chrono::duration_cast<std::chrono::milliseconds>(waited).count() > kSentinelCeilingMs)
                    break;
                juce::MessageManager::getInstance()->runDispatchLoopUntil(23);
            }
            pumpInputThreads(3 * delayMs + 200);

            const auto sends = sinkMessages(Sink::Device);
            for (int i = 0; i < (int)sends.size(); i++) {
                const auto &m = sends[(size_t)i];
                if (m == marker()) {
                    o.markerAt = i;
                    o.markers++;
                } else if (m == big) {
                    o.longAt = i;
                    o.longs++;
                } else if (m == sentinel()) {
                    o.sentinelAt = i;
                    o.sentinels++;
                } else {
                    o.others++;
                }
            }
            return o;
        }

        /** Each message exactly once, in the order sent. */
        static bool inOrder(First first, const Order &o)
        {
            if (!o.sentinelArrived || o.markers != 1 || o.longs != 1 || o.sentinels != 1 || o.others != 0)
                return false;
            const bool pairInOrder = first == First::Marker ? o.markerAt < o.longAt : o.longAt < o.markerAt;
            return pairInOrder && o.sentinelAt > o.markerAt && o.sentinelAt > o.longAt;
        }

        void runPair(LsxPath path, First first, int delayMs)
        {
            if (!midi_mock.hasSubsystemMock())
                GTEST_SKIP() << "no MIDI subsystem mock on this platform";

            ASSERT_NE(loadLuaPanel("fixture_long_sysex.panel"), nullptr);
            ASSERT_FALSE(luaIsNil("lsx_send")) << "fixture Lua did not load";
            const Order o = sendPair(path, first, 131072, delayMs);
            EXPECT_TRUE(inOrder(first, o)) << pathName(path) << ", " << (first == First::Marker ? "marker first" : "long first")
                                           << ", global delay " << delayMs << " ms: " << o.describe();
        }
};

// With a global MIDI delay every message waits its turn, the long one too. Deterministic: before
// the fix the long message left at once and the marker 100 ms later.
TEST_F(LongSysexOrder, lua_to_device_marker_then_long_with_global_delay)
{
    runPair(LsxPath::LuaToDevice, First::Marker, 100);
}

TEST_F(LongSysexOrder, thru_d2d_marker_then_long_with_global_delay)
{
    runPair(LsxPath::ThruD2D, First::Marker, 100);
}

// Without a delay: the input order holds when both messages land in one input batch, and when
// both are queued in the same millisecond.
TEST_F(LongSysexOrder, lua_to_device_marker_then_long)
{
    runPair(LsxPath::LuaToDevice, First::Marker, 0);
}

TEST_F(LongSysexOrder, thru_d2d_marker_then_long)
{
    runPair(LsxPath::ThruD2D, First::Marker, 0);
}

TEST_F(LongSysexOrder, thru_d2d_long_then_marker)
{
    runPair(LsxPath::ThruD2D, First::Long, 0);
}

TEST_F(LongSysexOrder, lua_to_device_long_then_marker)
{
    runPair(LsxPath::LuaToDevice, First::Long, 0);
}

// The upstream macOS failure, repeated: thru D2D at 128 KiB (long, then the sentinel) 20 times in
// one test. Reports how many transfers broke.
TEST_F(LongSysex, thru_d2d_131072_repeated)
{
    if (!midi_mock.hasSubsystemMock())
        GTEST_SKIP() << "no MIDI subsystem mock on this platform";

    ASSERT_NE(loadLuaPanel("fixture_long_sysex.panel"), nullptr);
    ASSERT_FALSE(luaIsNil("lsx_send")) << "fixture Lua did not load";
    int broken = 0;
    std::string first;
    for (int i = 0; i < 20; i++) {
        const LsxOutcome o = transfer(LsxPath::ThruD2D, 131072);
        if (!arrivedIntact(LsxPath::ThruD2D, 131072, o) && broken++ == 0)
            first = "transfer " + std::to_string(i) + ": " + o.describe();
    }
    EXPECT_EQ(broken, 0) << broken << " of 20 transfers broken; first: " << first;
}
