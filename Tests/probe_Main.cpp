// CtrlrPanelProbe -- drives a panel from a script and reports what it did. Headless one-shot:
// no hardware, no display.
//
// Loads an arbitrary .panel, rebinds its MIDI devices to the mocked test devices, (optionally)
// runs a JSON action script, and prints a JSON report of everything observed. See
// Doc/PanelProbe.md for the invocation / script / report contract.
//
// This reuses the GoogleTest mock MIDI layer (mock_MidiDevice*.cpp) but is NOT a gtest binary:
// it drives a real CtrlrProcessor the same way test_ProcessorFixture.h does, outside any TEST.
//
// Exit codes: 0 = clean; 1 = a Lua compile/runtime error or step error occurred (report still
// complete); 2 = panel load or script parse failure (minimal JSON error on stdout).

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <vector>
#include <fcntl.h>

// The report is written to a saved copy of fd 1 (see redirectStdoutToStderr). MSVC spells the
// POSIX fd calls with a leading underscore in <io.h>, and has no /dev/null or ssize_t.
#ifdef _WIN32
  #include <io.h>
#else
  #include <unistd.h>
#endif

namespace probe_fd
{
#ifdef _WIN32
    constexpr int stdoutFd = 1, stderrFd = 2;
    constexpr const char* nullDevice = "NUL";
    inline int dup (int fd)                                { return ::_dup (fd); }
    inline int dup2 (int from, int to)                     { return ::_dup2 (from, to); }
    inline int openWrite (const char* path)                { return ::_open (path, _O_WRONLY); }
    inline int close (int fd)                              { return ::_close (fd); }
    inline long write (int fd, const char* buf, size_t n)  { return ::_write (fd, buf, (unsigned int) n); }
    // Binary mode, so the report's "\n" does not become "\r\n" (one report per line in --stdin).
    inline void setBinary (int fd)                         { ::_setmode (fd, _O_BINARY); }
#else
    constexpr int stdoutFd = STDOUT_FILENO, stderrFd = STDERR_FILENO;
    constexpr const char* nullDevice = "/dev/null";
    inline int dup (int fd)                                { return ::dup (fd); }
    inline int dup2 (int from, int to)                     { return ::dup2 (from, to); }
    inline int openWrite (const char* path)                { return ::open (path, O_WRONLY); }
    inline int close (int fd)                              { return ::close (fd); }
    inline long write (int fd, const char* buf, size_t n)  { return (long) ::write (fd, buf, n); }
    inline void setBinary (int)                            {}
#endif
}

#include "gmock/gmock.h"
#include "JuceHeader.h"

#include <CtrlrProcessor.h>
#include <CtrlrManager.h>
#include <CtrlrPanel.h>
#include <CtrlrModulator.h>
#include <CtrlrModulatorProcessor.h>
#include "CtrlrComponents/CtrlrComponent.h"
#include "CtrlrComponents/CtrlrCustomComponent.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include <CtrlrMIDIDeviceManager.h>
#include "CtrlrLuaManager.h"
#include "Methods/CtrlrLuaMethodManager.h"
#include "Methods/CtrlrLuaMethod.h"
#include "CtrlrIDs.h"
#include "CtrlrLog.h"
#include "CtrlrMacros.h"

// Raw Lua 5.1 C API for callLuaMethod-with-args. Lua is vendored with C linkage
// (LUABIND_CPLUSPLUS_LUA is not defined), so wrap in extern "C". This is deliberately the
// stable C API (not luabind): it is exactly what a future LuaJIT switch honours, and it
// avoids dragging the luabind template layer into this C++17 translation unit.
extern "C"
{
    #include "lua.h"
    #include "lauxlib.h" // luaL_loadstring, for the `eval` / `waitFor` actions
}

#include "mock_MidiDevice.h"
#include "probe_Report.h"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

namespace
{
    constexpr int BLOCK_SIZE = 1024; // matches test_ProcessorFixture.h's audio block size

    // Stamped on every report (and on the fatal-error object) so a consumer can tell which
    // schema it is looking at. Bump on any breaking change to the report shape.
    constexpr int PROBE_REPORT_VERSION = 1;

    // Settle defaults. settleMs is a *ceiling*, not a floor: a step returns as soon as the panel
    // has been quiet (no device send, no host MIDI, no log line) for quietMs. See settle().
    constexpr int DEFAULT_SETTLE_MS = 300;
    constexpr int DEFAULT_QUIET_MS  = 30;
    constexpr int SETTLE_SLICE_MS   = 5; // granularity of one pump + processBlock iteration

    // CtrlrX pops a modal AlertWindow ("Callback error ...") on a runtime Lua error (see
    // CATCH_METHOD_EXCEPTION in CtrlrLuaMethodManagerCalls.cpp). That is a *blocking* modal loop on
    // the message thread -- fatal for a headless one-shot tool: it would hang waiting for a click
    // (or spin forever on a display-less CI). We can't touch that production code, so instead we run
    // a timer that dismisses any modal component the instant it appears. The timer fires from inside
    // the nested modal dispatch loop, so the blocked call returns immediately. The error is still
    // recorded (setValid(false)) and logged to stderr; only the dialog is suppressed.
    //
    // Panel Lua opens dialogs of its own too (utils.infoWindow / warnWindow / questionWindow,
    // AlertWindow.showOkCancelBox, ...), and dismissing one answers it with Cancel. So every dialog
    // the suppressor dismisses is recorded for the report (steps[i].dialogs).
    //
    // One CtrlrX dialog carries an error nothing else reports: a Lua timer callback that raised
    // (CtrlrLuaMultiTimer::timerCallback) is disabled and announced *only* by its dialog -- no log
    // line, no method marked invalid. CtrlrX has just read the Lua message off the top of the stack
    // and nothing pops it while the dialog runs, so it is read from there before dismissing.
    struct DismissedDialog
    {
        juce::String title;
        juce::String kind;               // "alert", "fileChooser" or "component"
        juce::StringArray buttons;
        bool isTimerError = false;       // CtrlrX's "Timer callback error" dialog
        juce::String timerErrorMessage;  // its Lua message, when it could be read
    };

    struct ModalDialogSuppressor : private juce::Timer
    {
        ModalDialogSuppressor() { startTimer(5); }
        ~ModalDialogSuppressor() override { stopTimer(); }

        lua_State* luaState = nullptr; // the loaded panel's, for a timer error's message

        // The dialogs dismissed since the last call.
        std::vector<DismissedDialog> take()
        {
            std::vector<DismissedDialog> out;
            out.swap(dismissed);
            return out;
        }

    private:
        std::vector<DismissedDialog> dismissed;

        void timerCallback() override
        {
            auto* m = juce::ModalComponentManager::getInstanceWithoutCreating();
            if (m == nullptr || m->getNumModalComponents() == 0)
                return;
            for (int i = 0; i < m->getNumModalComponents(); ++i)
                if (juce::Component* c = m->getModalComponent(i))
                    dismissed.push_back(describe(*c));
            m->cancelAllModalComponents();
        }

        DismissedDialog describe(juce::Component& c) const
        {
            DismissedDialog d;
            d.title = c.getName();
            d.kind = dynamic_cast<juce::AlertWindow*>(&c) != nullptr           ? "alert"
                   : dynamic_cast<juce::FileChooserDialogBox*>(&c) != nullptr ? "fileChooser"
                                                                                : "component";
            for (int i = 0; i < c.getNumChildComponents(); ++i)
                if (auto* b = dynamic_cast<juce::TextButton*>(c.getChildComponent(i)))
                    d.buttons.add(b->getButtonText());

            if (d.kind == "alert" && d.title.startsWith("Timer callback error"))
            {
                d.isTimerError = true;
                lua_State* L = luaState;
                if (L != nullptr && lua_gettop(L) > 0 && lua_type(L, -1) == LUA_TSTRING)
                    d.timerErrorMessage = juce::String::fromUTF8(lua_tostring(L, -1));
            }
            return d;
        }
    };

    ModalDialogSuppressor* g_dialogSuppressor = nullptr; // the one in main()

    // fd of the real stdout, saved before we redirect fd 1 to stderr. CtrlrX / JUCE / Lua write
    // plenty of noise to stdout during load; the report on stdout must be clean JSON, so all that
    // noise is sent to stderr and only the final report is written back to this saved fd.
    int g_realStdout = -1;

    // Redirect fd 1 (stdout) to fd 2 (stderr) so library chatter can't corrupt the JSON on stdout.
    void redirectStdoutToStderr()
    {
        std::cout.flush();
        std::fflush(stdout);
        g_realStdout = probe_fd::dup(probe_fd::stdoutFd);
        if (g_realStdout >= 0)
            probe_fd::setBinary(g_realStdout);
        probe_fd::dup2(probe_fd::stderrFd, probe_fd::stdoutFd);
    }

    // Quiet mode (default, no --verbose): send fd 1 and fd 2 to /dev/null so none of CtrlrX /
    // JUCE / Lua's stderr chatter (all of it lands on stderr via Logger::outputDebugString)
    // is shown. The report still goes to the saved g_realStdout fd, so stdout stays the clean
    // JSON contract and exit codes are unaffected.
    void silenceStdErr()
    {
        std::cout.flush();
        std::fflush(stdout);
        std::fflush(stderr);
        const int devnull = probe_fd::openWrite(probe_fd::nullDevice);
        if (devnull >= 0)
        {
            probe_fd::dup2(devnull, probe_fd::stdoutFd);
            probe_fd::dup2(devnull, probe_fd::stderrFd);
            probe_fd::close(devnull);
        }
    }

    // Captures CtrlrLog messages so console() output and real Lua error text land in the
    // report instead of only on stderr. messageLogged() is delivered on the message thread
    // during settle()'s dispatch-loop pumping, so it is read back single-threaded (no lock).
    struct LogCapture : public CtrlrLog::Listener
    {
        struct Entry { CtrlrLog::LogLevel level; juce::String message; };
        std::vector<Entry> entries;
        void messageLogged(CtrlrLog::CtrlrLogMessage m) override
        {
            entries.push_back({ m.level, m.message });
        }
    };

    // How deep pushVarToLua / luaToVar will descend before giving up. Guards against a
    // self-referential Lua table (belt and braces alongside the cycle set) and against a
    // pathologically nested JSON arg blowing the C stack.
    constexpr int MAX_CONVERSION_DEPTH = 8;

    // Push a JSON value onto the Lua stack as the matching Lua type. Scalars map directly;
    // JSON arrays become Lua tables keyed 1..n, JSON objects become string-keyed tables. Anything
    // deeper than MAX_CONVERSION_DEPTH, or that cannot be represented, becomes nil.
    void pushVarToLua(lua_State* L, const juce::var& v, int depth = 0)
    {
        if (depth > MAX_CONVERSION_DEPTH || ! lua_checkstack(L, 4))
        {
            lua_pushnil(L);
            return;
        }

        if (v.isBool())                                   lua_pushboolean(L, ((bool) v) ? 1 : 0);
        else if (v.isInt() || v.isInt64() || v.isDouble()) lua_pushnumber(L, (double) v);
        else if (v.isString())                            lua_pushstring(L, v.toString().toRawUTF8());
        else if (const juce::Array<juce::var>* a = v.getArray())
        {
            lua_createtable(L, a->size(), 0);
            for (int i = 0; i < a->size(); ++i)
            {
                pushVarToLua(L, (*a)[i], depth + 1);
                lua_rawseti(L, -2, i + 1); // Lua arrays are 1-based
            }
        }
        else if (juce::DynamicObject* o = v.getDynamicObject())
        {
            const juce::NamedValueSet& props = o->getProperties();
            lua_createtable(L, 0, props.size());
            for (int i = 0; i < props.size(); ++i)
            {
                lua_pushstring(L, props.getName(i).toString().toRawUTF8());
                pushVarToLua(L, props.getValueAt(i), depth + 1);
                lua_rawset(L, -3);
            }
        }
        else lua_pushnil(L);
    }

    // Convert the Lua value at stack index idx into a juce::var for the report.
    //
    // Tables become a JSON array when their keys are exactly the contiguous integers 1..n (the
    // Lua list convention), and a JSON object with stringified keys otherwise. Values that have
    // no JSON equivalent are reported as a marker string rather than silently dropped, so a test
    // asserting on a returned table can tell "the field was a function" from "the field was nil".
    // `visited` holds the table pointers on the current path: revisiting one is a cycle, but a
    // table referenced twice in sibling positions still serialises fully (it is erased on exit).
    juce::var luaToVar(lua_State* L, int idx, int depth, std::set<const void*>& visited)
    {
        switch (lua_type(L, idx))
        {
            case LUA_TBOOLEAN: return juce::var((bool) lua_toboolean(L, idx));
            case LUA_TNUMBER:  return juce::var((double) lua_tonumber(L, idx));
            case LUA_TSTRING:  return juce::var(juce::String::fromUTF8(lua_tostring(L, idx)));
            case LUA_TFUNCTION: return juce::var("<function>");
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA: return juce::var("<userdata>");
            case LUA_TTHREAD:  return juce::var("<thread>");
            case LUA_TTABLE:   break; // handled below
            default:           return juce::var(); // nil / unsupported
        }

        if (depth >= MAX_CONVERSION_DEPTH)
            return juce::var("<max-depth>");
        if (! lua_checkstack(L, 4))
            return juce::var("<max-depth>");

        // lua_next below pushes onto the stack, so a relative index would drift; pin it to an
        // absolute one. (Lua 5.1 has no lua_absindex; pseudo-indices are already absolute.)
        const int t = (idx < 0 && idx > LUA_REGISTRYINDEX) ? lua_gettop(L) + idx + 1 : idx;
        const void* ptr = lua_topointer(L, t);
        if (! visited.insert(ptr).second)
            return juce::var("<cycle>");

        // First pass: count the keys, so a table whose keys are exactly 1..n can be emitted as a
        // JSON array. lua_objlen gives n for the array part; if that accounts for every key, it
        // is a list.
        const int arrayLen = (int) lua_objlen(L, t);
        int keyCount = 0;
        lua_pushnil(L);
        while (lua_next(L, t) != 0)
        {
            ++keyCount;
            lua_pop(L, 1); // drop value, keep key for the next iteration
        }
        const bool isList = (arrayLen > 0 && keyCount == arrayLen);

        juce::var out;
        if (isList)
        {
            juce::Array<juce::var> items;
            for (int i = 1; i <= arrayLen; ++i)
            {
                lua_rawgeti(L, t, i);
                items.add(luaToVar(L, -1, depth + 1, visited));
                lua_pop(L, 1);
            }
            out = juce::var(items);
        }
        else
        {
            out = probeReport::makeObject();
            lua_pushnil(L);
            while (lua_next(L, t) != 0)
            {
                // lua_tostring on the *key* would coerce a number key in place and confuse
                // lua_next, so copy the key before stringifying it.
                lua_pushvalue(L, -2);
                const char* rawKey = lua_tostring(L, -1);
                juce::String key = (rawKey != nullptr) ? juce::String::fromUTF8(rawKey)
                                                       : juce::String("<non-scalar-key>");
                lua_pop(L, 1);
                if (key.isEmpty())
                    key = "<empty-key>"; // juce::Identifier cannot be constructed from ""
                probeReport::setProp(out, key, luaToVar(L, -1, depth + 1, visited));
                lua_pop(L, 1); // drop value, keep key
            }
        }

        visited.erase(ptr);
        return out;
    }

    // Convenience overload: convert the value at idx with a fresh cycle set.
    juce::var luaToVar(lua_State* L, int idx)
    {
        std::set<const void*> visited;
        return luaToVar(L, idx, 0, visited);
    }

    // Lua truthiness applied to a converted value: only nil and false are false, so 0 and "" are
    // both truthy (unlike C or Python). luaToVar maps Lua nil to a void var.
    bool isLuaTruthy(const juce::var& v)
    {
        if (v.isVoid())
            return false;
        if (v.isBool())
            return (bool) v;
        return true;
    }

    // pcall a function already sitting on the stack with `nargs` arguments pushed above it,
    // collecting exactly one return value into resultOut. Leaves the stack as it found it either
    // way. Returns false and fills errorOut if the call raised.
    bool pcallForResult(lua_State* L, int nargs, juce::var& resultOut, juce::String& errorOut)
    {
        if (lua_pcall(L, nargs, 1, 0) != 0)
        {
            const char* msg = lua_tostring(L, -1);
            errorOut = (msg != nullptr) ? juce::String::fromUTF8(msg)
                                        : juce::String("(no error message)");
            lua_pop(L, 1);
            return false;
        }
        resultOut = luaToVar(L, -1);
        lua_pop(L, 1);
        return true;
    }

    // Run a chunk of Lua source in the panel's own lua_State and collect its single result.
    //
    // The chunk is tried as an *expression* first ("return <chunk>"), exactly as the stock Lua
    // REPL does, so `codec.get(img, 'a.b')` yields its return value instead of being executed as
    // a bare call statement that throws the result away. Only if that fails to compile is the
    // chunk run verbatim, which is what makes multi-statement chunks ("local x = 1 return x * 2")
    // work too.
    bool evalLuaChunk(lua_State* L, const juce::String& chunk,
                      juce::var& resultOut, juce::String& errorOut)
    {
        const juce::String asExpression = "return " + chunk;
        if (luaL_loadstring(L, asExpression.toRawUTF8()) != 0)
        {
            lua_pop(L, 1); // discard the expression-form error; the statement form has the say
            if (luaL_loadstring(L, chunk.toRawUTF8()) != 0)
            {
                const char* msg = lua_tostring(L, -1);
                errorOut = "compile error: "
                           + ((msg != nullptr) ? juce::String::fromUTF8(msg) : juce::String("?"));
                lua_pop(L, 1);
                return false;
            }
        }
        return pcallForResult(L, 0, resultOut, errorOut);
    }

    // Build one synthetic mouse event on `comp`, in component-local coordinates. The probe
    // dispatches these straight to the component's handlers, so default pressure/orientation/tilt
    // are fine.
    juce::MouseEvent makeMouseEvent(juce::Component* comp, juce::Point<float> pos, juce::ModifierKeys mods,
                                    juce::Time eventTime, juce::Point<float> downPos, juce::Time downTime,
                                    int clicks, bool wasDragged)
    {
        return juce::MouseEvent(juce::Desktop::getInstance().getMainMouseSource(),
                                pos, mods,
                                juce::MouseInputSource::invalidPressure,
                                juce::MouseInputSource::invalidOrientation,
                                juce::MouseInputSource::invalidRotation,
                                juce::MouseInputSource::invalidTiltX,
                                juce::MouseInputSource::invalidTiltY,
                                comp, comp,
                                eventTime, downPos, downTime,
                                clicks, wasDragged);
    }

    // The `click` action's event: the component's centre, the left button held (as on every real
    // click, so a handler gated on isLeftButtonDown() runs), no keyboard modifiers.
    juce::MouseEvent makeClickEvent(juce::Component* comp)
    {
        const juce::Point<float> centre((float) comp->getWidth() * 0.5f, (float) comp->getHeight() * 0.5f);
        const juce::Time now = juce::Time::getCurrentTime();
        return makeMouseEvent(comp, centre, juce::ModifierKeys(juce::ModifierKeys::leftButtonModifier),
                              now, centre, now, 1, false);
    }

    // Write a var as one JSON document to the real (saved) stdout. `compact` puts it on a single
    // line, which is what --stdin's one-report-per-line framing needs.
    void writeReport(const juce::var& report, bool compact)
    {
        const juce::String json = juce::JSON::toString(report, compact) + "\n";
        const int fd = g_realStdout >= 0 ? g_realStdout : probe_fd::stdoutFd;
        const char* bytes = json.toRawUTF8();
        size_t remaining = std::strlen(bytes);
        while (remaining > 0)
        {
            const long written = probe_fd::write(fd, bytes, remaining);
            if (written <= 0)
                break;
            bytes += written;
            remaining -= (size_t) written;
        }
    }

    // Print a var as JSON, then hard-exit. Clean teardown is deliberately skipped: the panel/Lua
    // object graph is known to leak once JUCE shuts down and the Debug leak detector would fire
    // after we have already produced the report. A one-shot tool just exits.
    [[noreturn]] void emitAndExit(const juce::var& report, int code)
    {
        writeReport(report, false);
        std::_Exit(code);
    }

    // Minimal error object for the load/parse failure path (exit 2).
    [[noreturn]] void failFatal(const juce::String& message, const juce::String& detail = {})
    {
        juce::var err = probeReport::makeObject();
        probeReport::setProp(err, "reportVersion", PROBE_REPORT_VERSION);
        probeReport::setProp(err, "error", message);
        if (detail.isNotEmpty())
            probeReport::setProp(err, "detail", detail);
        probeReport::setProp(err, "exitCode", 2);
        emitAndExit(err, 2);
    }

    // Pick the mock device to rebind to. The mock exposes one device with two ports, enumerated by
    // CtrlrX as "Device 00, port 00" / "Device 00, port 01". Fixture panels use port 00 for input
    // and port 01 for output, so mirror that when both are present; otherwise fall back to ends.
    juce::String pickMockDevice(const juce::StringArray& list, bool wantOutput)
    {
        if (list.isEmpty())
            return {};
        const juce::String suffix = wantOutput ? "port 01" : "port 00";
        for (const auto& s : list)
            if (s.endsWith(suffix))
                return s;
        return wantOutput ? list[list.size() - 1] : list[0];
    }

    // ------------------------------------------------------------------------------------------
    // Paint honesty. repaint() paints nothing here: with no desktop peer, JUCE's repaint walk ends
    // at the top-level component with nowhere to queue the area, and no amount of pumping paints
    // it. So the probe counts, per uiCustomComponent, the repaint requests it sees and the paints
    // that really happen, and warns about requests that were never followed by a paint.
    // ------------------------------------------------------------------------------------------

    // Per-component counters. They live in the session, not in the spy: the component owns the
    // spy, and panel Lua can replace or delete it (setCachedComponentImage, setBufferedToImage).
    struct PaintCounters
    {
        int repaintRequests = 0;    // invalidations: repaint(), move/resize, property change, focus
        int paints = 0;             // parent paints the spy saw + the `paint` action's own
        int requestsSincePaint = 0; // > 0 means a repaint is still pending
    };

    // A CachedComponentImage that paints straight through -- the canvas screenshot is byte-identical
    // with and without it -- but sees both sides: JUCE calls invalidate*() on every repaint request
    // before it looks for a peer, and paint() whenever a parent paints the component. It does not
    // see a direct paintEntireComponent() on the component, so the `paint` action counts its own.
    struct PaintSpy : public juce::CachedComponentImage
    {
        PaintSpy(juce::Component& c, PaintCounters& k, juce::uint32 t) : owner(c), counters(k), token(t) {}

        void paint(juce::Graphics& g) override
        {
            ++counters.paints;
            counters.requestsSincePaint = 0;
            owner.paintEntireComponent(g, false);
        }
        bool invalidateAll() override                          { noteRequest(); return true; }
        bool invalidate(const juce::Rectangle<int>&) override { noteRequest(); return true; }
        void releaseResources() override {}

        void noteRequest() { ++counters.repaintRequests; ++counters.requestsSincePaint; }

        juce::Component& owner;
        PaintCounters& counters;
        const juce::uint32 token; // tells our spy from a newer one, even at a recycled address
    };

    struct TrackedPaint
    {
        PaintCounters counters;
        juce::uint32 token = 0;
    };
    using PaintTracking = std::map<juce::String, TrackedPaint>; // keyed by modulator name

    // True while the component still carries the spy the probe attached. False once Lua replaced
    // or removed it, or the component was recreated (e.g. a uiType change): the counters are then
    // no longer fed, and the report says `tracked: false`.
    bool isOurSpy(juce::Component* comp, const TrackedPaint& t)
    {
        auto* spy = (comp != nullptr) ? dynamic_cast<PaintSpy*>(comp->getCachedComponentImage()) : nullptr;
        return spy != nullptr && spy->token == t.token;
    }

    // The method a component's uiCustomPaintCallback names, or nullptr when it names none.
    CtrlrLuaMethod* paintMethodOf(CtrlrPanel& panel, CtrlrModulator& mod)
    {
        const juce::String name = mod.getObjectTree().getChildWithName(Ids::component)
                                     .getProperty(Ids::uiCustomPaintCallback).toString();
        if (name.isEmpty())
            return nullptr;
        return panel.getCtrlrLuaManager().getMethodManager().getMethodByName(name);
    }

    // ------------------------------------------------------------------------------------------
    // Mouse. One press is held across steps (`down` ... `drag` ... `up`) so a test can assert
    // mid-gesture. The probe stamps every event of a press itself, on a virtual clock: +10 ms per
    // delivered event, plus any `holdMs`. Wall-clock age would be flaky, because the default settle
    // ceiling (300 ms) is exactly JUCE's long-press threshold.
    // ------------------------------------------------------------------------------------------
    constexpr int   MOUSE_EVENT_MS = 10;
    constexpr float DRAG_THRESHOLD_PX = 4.0f; // MouseInputSource's "moved significantly"
    constexpr int   LONG_PRESS_MS = 300;      // MouseInputSource::isLongPressOrDrag()

    struct HeldPress
    {
        bool active = false;
        juce::String modulator;
        juce::Component::SafePointer<juce::Component> comp;
        juce::Point<float> downPos, lastPos;
        juce::Time t0;              // real clock, stamped at the down
        juce::int64 vMs = 0;        // virtual age of the press
        int clicks = 1;
        juce::ModifierKeys mods;    // including the button flag
        bool movedSignificantly = false;

        void moveTo(juce::Point<float> pos)
        {
            lastPos = pos;
            movedSignificantly = movedSignificantly || downPos.getDistanceFrom(pos) >= DRAG_THRESHOLD_PX;
        }

        // The event this press delivers now, at `pos`. JUCE's rule for mouseWasDragged, on the
        // virtual age.
        juce::MouseEvent event(juce::Point<float> pos) const
        {
            return makeMouseEvent(comp.getComponent(), pos, mods,
                                  t0 + juce::RelativeTime::milliseconds((juce::int64) vMs), downPos, t0,
                                  clicks, movedSignificantly || vMs > LONG_PRESS_MS);
        }
    };

    // Everything a step needs to drive the loaded panel.
    struct RunContext
    {
        CtrlrProcessor& processor;
        CtrlrPanel& panel;
        MockMidi& midi;
        juce::AudioSampleBuffer& buffer;
        std::mutex& sendsMutex;
        std::vector<juce::MidiMessage>& deviceSends;
        std::set<juce::String>& knownInvalidMethods; // methods already invalid; used to detect new runtime errors
        LogCapture& log;
        PaintTracking& paintTracking;
        juce::Array<juce::var>& warnings; // the script's top-level warnings[]
        int settleMs = DEFAULT_SETTLE_MS; // ceiling: give up waiting for quiet after this long
        int quietMs  = DEFAULT_QUIET_MS;  // stop as soon as nothing has happened for this long
        HeldPress press;                  // the mouse press held across steps, if any
    };

    // Warnings are what the probe saw but could not turn into a pass or a fail: they are not
    // counted in errorCount and do not change the exit code. `step` is present when a step
    // raised it.
    void addWarning(RunContext& ctx, const char* type, const juce::String& modulator,
                    const juce::String& message, int stepIndex = -1)
    {
        juce::var w = probeReport::makeObject();
        probeReport::setProp(w, "type", type);
        probeReport::setProp(w, "modulator", modulator);
        probeReport::setProp(w, "message", message);
        if (stepIndex >= 0)
            probeReport::setProp(w, "step", stepIndex);
        ctx.warnings.add(w);
    }

    // Parse a whitespace/comma-separated hex byte string ("F0 7E 41 F7") into a MidiMessage.
    // Returns false on an empty/garbage string.
    bool parseHexBytes(const juce::String& hex, juce::MidiMessage& out)
    {
        juce::StringArray tokens;
        tokens.addTokens(hex, " \t\r\n,", "");
        tokens.removeEmptyStrings();
        juce::MemoryBlock bytes;
        for (const auto& tok : tokens)
        {
            const juce::String t = tok.startsWithIgnoreCase("0x") ? tok.substring(2) : tok;
            if (t.isEmpty() || ! t.containsOnly("0123456789abcdefABCDEF") || t.length() > 2)
                return false;
            const juce::uint8 b = (juce::uint8) t.getHexValue32();
            bytes.append(&b, 1);
        }
        if (bytes.getSize() == 0)
            return false;
        out = juce::MidiMessage(bytes.getData(), (int) bytes.getSize());
        return true;
    }

    // Every failure a step can produce goes into one array of {type, message} objects, so a
    // caller never has to know which of several channels a given failure would have landed in.
    // `type` is "action" (the probe could not carry out what was asked) or "lua" (the panel's
    // own Lua raised).
    void addError(juce::Array<juce::var>& errors, const char* type, const juce::String& message)
    {
        juce::var e = probeReport::makeObject();
        probeReport::setProp(e, "type", type);
        probeReport::setProp(e, "message", message);
        errors.add(e);
    }

    // The dialogs dismissed since the last call, as report entries. A timer callback's error dialog
    // also becomes a "lua" error in `errors` (when given): the dialog was CtrlrX's only report of it.
    juce::Array<juce::var> takeDialogs(juce::Array<juce::var>* errors)
    {
        juce::Array<juce::var> out;
        if (g_dialogSuppressor == nullptr)
            return out;
        for (const DismissedDialog& d : g_dialogSuppressor->take())
        {
            juce::var v = probeReport::makeObject();
            probeReport::setProp(v, "title", d.title);
            probeReport::setProp(v, "kind", d.kind);
            juce::Array<juce::var> buttons;
            for (const auto& b : d.buttons)
                buttons.add(b);
            probeReport::setProp(v, "buttons", buttons);
            probeReport::setProp(v, "result", 0); // dismissed: what Cancel / closing the window returns
            out.add(v);

            if (d.isTimerError && errors != nullptr)
                addError(*errors, "lua", d.title + ": "
                                         + (d.timerErrorMessage.isNotEmpty() ? d.timerErrorMessage
                                                                             : juce::String("(Lua message not available)"))
                                         + " -- CtrlrX disabled the callback");
        }
        return out;
    }

    // Count the entries of one type in a step's errors array.
    int countErrorsOfType(const juce::var& errors, const juce::String& type)
    {
        int n = 0;
        if (const juce::Array<juce::var>* a = errors.getArray())
            for (const auto& e : *a)
                if (e.getProperty("type", juce::var()).toString() == type)
                    ++n;
        return n;
    }

    // Append, to `errors`, any method that transitioned valid -> invalid since we last looked
    // (CATCH_METHOD_EXCEPTION marks a method invalid when its Lua raises at runtime). The actual
    // "Callback error: ..." text is only logged (level LuaError), so we fold in whatever error
    // text the LogCapture observed during this step instead of the old "see stderr" placeholder.
    void collectNewLuaErrors(RunContext& ctx, juce::Array<juce::var>& errors,
                             const juce::StringArray& capturedErrorText)
    {
        const juce::String detail = capturedErrorText.isEmpty()
            ? juce::String("see stderr log for the Lua message (run with --verbose)")
            : capturedErrorText.joinIntoString(" | ");

        CtrlrLuaMethodManager& mgr = ctx.panel.getCtrlrLuaManager().getMethodManager();
        for (int i = 0; i < mgr.getNumMethods(); ++i)
        {
            CtrlrLuaMethod* method = mgr.getMethodByIndex(i);
            if (method == nullptr || method->isValid())
                continue;
            const juce::String name = method->getName();
            if (ctx.knownInvalidMethods.insert(name).second) // newly invalid
                addError(errors, "lua",
                         "method '" + name + "' raised a runtime error and was disabled: " + detail);
        }
    }

    // Drain whatever the device-send recorder has captured so far into deviceMidiOut.
    // Returns how many messages were taken. Device sends arrive on JUCE's MidiOutput background
    // thread, hence the lock.
    int takeDeviceSends(RunContext& ctx, juce::Array<juce::var>& deviceMidiOut)
    {
        std::lock_guard<std::mutex> l(ctx.sendsMutex);
        const int n = (int) ctx.deviceSends.size();
        for (const auto& m : ctx.deviceSends)
            deviceMidiOut.add(probeReport::midiToHex(m));
        ctx.deviceSends.clear();
        return n;
    }

    // Implicit settle after every action, driven by a quiet window rather than a fixed sleep.
    //
    // Each ~5 ms iteration pumps the message loop (which is what delivers CtrlrLog's AsyncUpdater
    // and the async modulator callbacks) and runs one processBlock (which flushes host output and
    // the panel's MIDI collector), then drains the device-send recorder. Anything new -- a device
    // send, host MIDI, or a log line -- resets the quiet timer; the loop returns as soon as
    // quietMs has passed with nothing happening, and gives up at the ceilingMs ceiling.
    //
    // Consequences worth knowing:
    //  - ceilingMs is a *ceiling*. A typical pure-Lua step now costs ~quietMs instead of the full
    //    settle, which is where the bulk of the suite's runtime went.
    //  - ceilingMs == 0 still pumps once and runs one processBlock. A hard bypass would silently
    //    drop console() output and Lua error text, since CtrlrLog only delivers while the message
    //    loop runs -- the same class of trap this rework exists to remove.
    //  - A panel with a chatty repeating timer never settles quiet, so it degrades to the ceiling
    //    cost; it cannot spin here.
    void settle(RunContext& ctx, int ceilingMs, int quietMs,
                juce::Array<juce::var>& deviceMidiOut, juce::Array<juce::var>& hostMidiOut,
                const juce::MidiBuffer& firstBlockInput = {})
    {
        const auto start = std::chrono::steady_clock::now();
        auto lastActivity = start;
        auto elapsedMsSince = [](std::chrono::steady_clock::time_point t) {
            return (int) std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - t).count();
        };

        juce::MidiBuffer midi;
        size_t lastLogSize = ctx.log.entries.size();
        bool first = true;

        for (;;)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil(SETTLE_SLICE_MS);

            midi.clear();
            if (first)
                midi.addEvents(firstBlockInput, 0, -1, 0); // carry host-injected input in block 0
            ctx.processor.processBlock(ctx.buffer, midi);

            bool sawActivity = false;
            for (const auto meta : midi)
            {
                hostMidiOut.add(probeReport::midiToHex(meta.getMessage()));
                sawActivity = true;
            }
            if (takeDeviceSends(ctx, deviceMidiOut) > 0)
                sawActivity = true;
            if (ctx.log.entries.size() != lastLogSize)
            {
                lastLogSize = ctx.log.entries.size();
                sawActivity = true;
            }

            const auto now = std::chrono::steady_clock::now();
            if (sawActivity)
                lastActivity = now;

            first = false;
            if (elapsedMsSince(start) >= ceilingMs)
                break;
            if (elapsedMsSince(lastActivity) >= quietMs)
                break;
        }
    }

    // Pump for exactly `ms`, with no early exit: the floor semantics `wait` promises. Still
    // collects everything settle() would, so a late device send is not dropped.
    void waitFixed(RunContext& ctx, int ms,
                   juce::Array<juce::var>& deviceMidiOut, juce::Array<juce::var>& hostMidiOut,
                   const juce::MidiBuffer& firstBlockInput = {})
    {
        // A quiet window as long as the wait itself can never elapse first, so this runs the full
        // duration while reusing the same pump/processBlock/drain loop.
        settle(ctx, ms, ms + 1, deviceMidiOut, hostMidiOut, firstBlockInput);
    }

    // Discard anything the recorder captured so a step's capture starts clean.
    void clearRecorder(RunContext& ctx)
    {
        std::lock_guard<std::mutex> l(ctx.sendsMutex);
        ctx.deviceSends.clear();
    }

    // Recursively find the first juce::Button under a component (its internal button, for click).
    juce::Button* findButtonChild(juce::Component* c)
    {
        for (int i = 0; i < c->getNumChildComponents(); ++i)
        {
            juce::Component* child = c->getChildComponent(i);
            if (auto* b = dynamic_cast<juce::Button*>(child))
                return b;
            if (auto* b = findButtonChild(child))
                return b;
        }
        return nullptr;
    }

    // Write an image as PNG to `path` (relative to the working directory). Returns false on
    // failure; `fullPathOut` is the absolute path either way.
    bool writePng(const juce::Image& image, const juce::String& path, juce::String& fullPathOut)
    {
        const juce::File out = juce::File::getCurrentWorkingDirectory().getChildFile(path);
        fullPathOut = out.getFullPathName();
        out.deleteFile();
        std::unique_ptr<juce::FileOutputStream> stream(out.createOutputStream());
        juce::PNGImageFormat png;
        if (stream == nullptr || ! png.writeImageToStream(image, *stream))
            return false;
        stream->flush();
        return true;
    }

    // An event label coordinate: integers without a decimal point, anything else with up to 3
    // decimals and the trailing zeros trimmed ("35", "22.5").
    juce::String formatCoord(float v)
    {
        if (v == std::floor(v) && std::abs(v) < 1.0e9f)
            return juce::String((juce::int64) v);
        juce::String s((double) v, 3);
        while (s.endsWithChar('0'))
            s = s.dropLastCharacters(1);
        if (s.endsWithChar('.'))
            s = s.dropLastCharacters(1);
        return s;
    }

    juce::String eventLabel(const juce::String& kind, juce::Point<float> p)
    {
        return kind + "@" + formatCoord(p.x) + "," + formatCoord(p.y);
    }

    // A drag on a drag-and-drop container with a live dadStart runs that callback, then JUCE's
    // startDragging() finds the real mouse source not dragging and silently returns (a jassert in
    // Debug). Nothing is dropped anywhere, yet the step looks fine -- so it is worth a warning.
    bool isDragAndDropTrap(CtrlrPanel& panel, CtrlrModulator& mod)
    {
        const juce::ValueTree tree = mod.getObjectTree().getChildWithName(Ids::component);
        if (! (bool) tree.getProperty(Ids::uiCustomDragAndDropContainer, false))
            return false;
        const juce::String name = tree.getProperty(Ids::uiCustomStartDraggingCallback).toString();
        CtrlrLuaMethod* m = name.isEmpty() ? nullptr
                                           : panel.getCtrlrLuaManager().getMethodManager().getMethodByName(name);
        return m != nullptr && m->isValid();
    }

    // The `mouse` action. Dispatches straight to the named uiCustomComponent's own handlers (no
    // hit-testing, no MouseListeners), like `click`, but at the requested position, with the
    // requested buttons and modifiers, and with every event of a press threading the down state.
    void runMouseAction(RunContext& ctx, int stepIndex, const juce::var& action, juce::var& step,
                        juce::Array<juce::var>& errors)
    {
        const juce::String gesture = action.getProperty("gesture", juce::var()).toString();
        const juce::String modName = action.getProperty("modulator", juce::var()).toString();
        HeldPress& press = ctx.press;

        static const juce::StringArray gestures { "down", "drag", "up", "click", "doubleClick", "move", "wheel" };
        if (! gestures.contains(gesture))
        {
            addError(errors, "action", "mouse: unknown gesture '" + gesture
                                       + "' (expected down, drag, up, click, doubleClick, move or wheel)");
            return;
        }
        const bool continuesPress = (gesture == "drag" || gesture == "up");
        const bool startsPress    = (gesture == "down" || gesture == "click" || gesture == "doubleClick");

        // Keyboard modifiers. On Linux and Windows JUCE's `cmd` is the same flag as `ctrl`.
        const bool hasMods = action.hasProperty("mods");
        int keyFlags = 0;
        if (hasMods)
        {
            const juce::Array<juce::var>* mods = action.getProperty("mods", juce::var()).getArray();
            if (mods == nullptr)
            {
                addError(errors, "action", "mouse: 'mods' must be an array");
                return;
            }
            for (const auto& m : *mods)
            {
                const juce::String k = m.toString();
                if      (k == "shift") keyFlags |= juce::ModifierKeys::shiftModifier;
                else if (k == "ctrl")  keyFlags |= juce::ModifierKeys::ctrlModifier;
                else if (k == "alt")   keyFlags |= juce::ModifierKeys::altModifier;
                else if (k == "cmd")   keyFlags |= juce::ModifierKeys::commandModifier;
                else
                {
                    addError(errors, "action", "mouse: unknown modifier '" + k + "' (expected shift, ctrl, alt or cmd)");
                    return;
                }
            }
        }

        int buttonFlag = juce::ModifierKeys::leftButtonModifier;
        if (action.hasProperty("button"))
        {
            const juce::String b = action.getProperty("button", juce::var()).toString();
            if (! startsPress)
            {
                addError(errors, "action", "mouse: 'button' applies to down, click and doubleClick; "
                                           "drag and up keep the pressed button");
                return;
            }
            if      (b == "left")   buttonFlag = juce::ModifierKeys::leftButtonModifier;
            else if (b == "right")  buttonFlag = juce::ModifierKeys::rightButtonModifier;
            else if (b == "middle") buttonFlag = juce::ModifierKeys::middleButtonModifier;
            else
            {
                addError(errors, "action", "mouse: unknown button '" + b + "' (expected left, right or middle)");
                return;
            }
        }

        if (action.hasProperty("holdMs") && ! continuesPress)
        {
            addError(errors, "action", "mouse: 'holdMs' applies to drag and up");
            return;
        }

        const bool hasXY = action.hasProperty("x") && action.hasProperty("y");
        const juce::Point<float> pos((float) (double) action.getProperty("x", juce::var(0)),
                                     (float) (double) action.getProperty("y", juce::var(0)));

        juce::Array<juce::var> events;
        juce::int64 pressMs = -1; // the press's virtual age at the step's last event; -1 = no press
        juce::String target = modName;

        if (continuesPress)
        {
            if (! press.active)
            {
                addError(errors, "action", "mouse: '" + gesture + "' needs a held press; send a 'down' first");
                return;
            }
            if (modName.isNotEmpty() && modName != press.modulator)
            {
                addError(errors, "action", "mouse: the held press is on '" + press.modulator + "', not '" + modName + "'");
                return;
            }
            // drag/up go to the pressed component even if Lua hid it mid-press, as on screen.
            juce::Component* comp = press.comp.getComponent();
            if (comp == nullptr)
            {
                addError(errors, "action", "mouse: the pressed component '" + press.modulator + "' was deleted");
                press = HeldPress(); // cleared, so the end-of-script check doesn't report it again
                return;
            }
            if (gesture == "drag" && ! hasXY)
            {
                addError(errors, "action", "mouse: drag needs 'x' and 'y'");
                return;
            }
            target = press.modulator;

            if (hasMods) // keyboard modifiers may change mid-press; the button stays
                press.mods = juce::ModifierKeys(keyFlags | (press.mods.getRawFlags()
                                                            & juce::ModifierKeys::allMouseButtonModifiers));
            press.vMs += juce::jmax(0, (int) action.getProperty("holdMs", juce::var(0)));

            if (gesture == "drag")
            {
                const int n = juce::jmax(1, (int) action.getProperty("steps", juce::var(1)));
                const juce::Point<float> from = press.lastPos;
                for (int k = 1; k <= n && press.comp != nullptr; ++k)
                {
                    const juce::Point<float> p = (k == n) ? pos : from + (pos - from) * ((float) k / (float) n);
                    press.vMs += MOUSE_EVENT_MS;
                    press.moveTo(p);
                    comp->mouseDrag(press.event(p));
                    events.add(eventLabel("drag", p));
                }
                CtrlrModulator* mod = ctx.panel.getModulator(press.modulator);
                if (mod != nullptr && isDragAndDropTrap(ctx.panel, *mod))
                    addWarning(ctx, "mouse", press.modulator,
                               "drag-and-drop is not simulated: dadStart ran, startDragging was a no-op", stepIndex);
                pressMs = press.vMs;
            }
            else // up, with the pre-release mods, as MouseInputSource passes them
            {
                const juce::Point<float> p = hasXY ? pos : press.lastPos;
                press.vMs += MOUSE_EVENT_MS;
                press.moveTo(p);
                comp->mouseUp(press.event(p));
                events.add(eventLabel("up", p));
                pressMs = press.vMs;
                press = HeldPress();
            }
        }
        else
        {
            if (press.active)
            {
                addError(errors, "action", "mouse: a press on '" + press.modulator
                                           + "' is held; only drag and up can follow a down");
                return;
            }

            // The checks a gesture start makes: a uiCustomComponent a user could reach, at a point
            // inside it. Anything else delivers nothing.
            CtrlrModulator* mod = ctx.panel.getModulator(modName);
            CtrlrComponent* comp = (mod != nullptr) ? mod->getComponent() : nullptr;
            if (mod == nullptr)
            {
                addError(errors, "action", "no modulator named '" + modName + "'");
                return;
            }
            if (dynamic_cast<CtrlrCustomComponent*>(comp) == nullptr)
            {
                addError(errors, "action", "mouse drives uiCustomComponent only; use click for buttons, "
                                           "setValue for value controls ('" + modName + "')");
                return;
            }
            if (! probeReport::isEffectivelyVisible(comp, ctx.panel.getCanvas()))
            {
                addError(errors, "action", "mouse: '" + modName + "' is not visible (effectiveVisible is false)");
                return;
            }
            if (! hasXY)
            {
                addError(errors, "action", "mouse: " + gesture + " needs 'x' and 'y'");
                return;
            }
            if (pos.x < 0 || pos.y < 0 || pos.x >= (float) comp->getWidth() || pos.y >= (float) comp->getHeight())
            {
                addError(errors, "action", "mouse: (" + formatCoord(pos.x) + ", " + formatCoord(pos.y)
                                           + ") is outside '" + modName + "' (" + juce::String(comp->getWidth())
                                           + " x " + juce::String(comp->getHeight()) + ")");
                return;
            }

            const juce::Time now = juce::Time::getCurrentTime();
            if (startsPress)
            {
                HeldPress p;
                p.active = true;
                p.modulator = modName;
                p.comp = comp;
                p.downPos = p.lastPos = pos;
                p.t0 = now;
                p.mods = juce::ModifierKeys(keyFlags | buttonFlag);

                // One press: down at its own time 0, then (unless it stays held) up 10 ms later.
                auto pressOnce = [&](bool release) {
                    if (p.comp == nullptr)
                        return;
                    p.comp->mouseDown(p.event(pos));
                    events.add(eventLabel("down", pos));
                    if (! release || p.comp == nullptr)
                        return;
                    p.vMs += MOUSE_EVENT_MS;
                    const juce::MouseEvent up = p.event(pos);
                    p.comp->mouseUp(up);
                    events.add(eventLabel("up", pos));
                    // JUCE's Component::internalMouseUp: the double-click follows the second up,
                    // with the same event.
                    if (p.clicks >= 2 && p.comp != nullptr)
                    {
                        p.comp->mouseDoubleClick(up);
                        events.add(eventLabel("doubleClick", pos));
                    }
                };

                if (gesture == "down")
                {
                    pressOnce(false);
                    press = p;
                }
                else
                {
                    pressOnce(true);
                    if (gesture == "doubleClick")
                    {
                        // The second press starts 10 ms after the first up and keeps its own down time.
                        p.t0 = p.t0 + juce::RelativeTime::milliseconds((juce::int64) (p.vMs + MOUSE_EVENT_MS));
                        p.vMs = 0;
                        p.clicks = 2;
                        pressOnce(true);
                    }
                }
                pressMs = p.vMs;
            }
            else if (gesture == "move")
            {
                comp->mouseMove(makeMouseEvent(comp, pos, juce::ModifierKeys(keyFlags), now, pos, now, 1, false));
                events.add(eventLabel("move", pos));
            }
            else // wheel
            {
                juce::MouseWheelDetails wheel;
                wheel.deltaX = (float) (double) action.getProperty("dx", juce::var(0));
                wheel.deltaY = (float) (double) action.getProperty("dy", juce::var(0));
                wheel.isReversed = false;
                wheel.isSmooth = false;
                wheel.isInertial = false;
                comp->mouseWheelMove(makeMouseEvent(comp, pos, juce::ModifierKeys(keyFlags), now, pos, now, 1, false),
                                     wheel);
                events.add(eventLabel("wheel", pos));
            }
        }

        juce::var m = probeReport::makeObject();
        probeReport::setProp(m, "modulator", target);
        probeReport::setProp(m, "gesture", gesture);
        probeReport::setProp(m, "events", events);
        if (pressMs >= 0)
            probeReport::setProp(m, "pressMs", (int) pressMs);
        probeReport::setProp(step, "mouse", m);
    }

    // Dispatch one action, run the settle, and return the completed step var.
    juce::var executeStep(RunContext& ctx, int index, const juce::var& action)
    {
        juce::var step = probeReport::makeObject();
        probeReport::setProp(step, "index", index);
        probeReport::setProp(step, "action", action);

        juce::Array<juce::var> errors; // every failure, action- or Lua-sourced, ends up here
        const juce::String actionName = action.getProperty("action", juce::var()).toString();

        // settleMs / quietMs are read from *every* action, not just `wait`, so a step known to be
        // slow (or known to need no settle at all) can be tuned without changing the whole script.
        const int settleMs = (int) action.getProperty("settleMs", juce::var(ctx.settleMs));
        const int quietMs  = (int) action.getProperty("quietMs",  juce::var(ctx.quietMs));

        bool isFixedWait = false;   // `wait` keeps floor semantics: run the full duration
        int  fixedWaitMs = 0;
        bool settleDone  = false;   // `waitFor` does its own pumping; don't settle again after it
        juce::MidiBuffer hostInput; // only injectHostMidi fills this
        juce::var stepReturn;       // an action that produces a Lua value records it here
        bool hasReturn = false;
        juce::Array<juce::var> deviceMidiOut, hostMidiOut;

        // Console/Lua-error lines captured during this step start after everything logged so far.
        const size_t logStart = ctx.log.entries.size();

        clearRecorder(ctx);

        if (actionName == "wait")
        {
            // Deliberately a floor, not a ceiling: `wait` is how a script says "this panel does
            // something on a timer, give it this long" -- an early quiet-window exit would defeat
            // the point.
            isFixedWait = true;
            fixedWaitMs = (int) action.getProperty("ms", juce::var(settleMs));
        }
        else if (actionName == "setValue")
        {
            const juce::String modName = action.getProperty("modulator", juce::var()).toString();
            CtrlrModulator* mod = ctx.panel.getModulator(modName);
            if (mod == nullptr)
            {
                addError(errors, "action","no modulator named '" + modName + "'");
            }
            else
            {
                const double value = (double) action.getProperty("value", juce::var(0));
                // Prefer the GUI path (setComponentValue -> setValueGeneric(changedByGUI)) so the
                // full knob-move wiring fires; fall back to the processor directly if this headless
                // modulator has no component.
                if (CtrlrComponent* comp = mod->getComponent())
                    comp->setComponentValue(value, true);
                else
                    mod->getProcessor().setValueGeneric(
                        CtrlrModulatorValue(value, CtrlrModulatorValue::changedByGUI), true);
            }
        }
        else if (actionName == "click")
        {
            const juce::String modName = action.getProperty("modulator", juce::var()).toString();
            CtrlrModulator* mod = ctx.panel.getModulator(modName);
            if (mod == nullptr)
                addError(errors, "action","no modulator named '" + modName + "'");
            else if (CtrlrComponent* comp = mod->getComponent())
            {
                // Drive the full component mouse-up path, not just the internal juce::Button:
                //   mouseDown -> (triggerClick) -> mouseUp
                // CtrlrComponent::mouseDown/mouseUp fire componentLuaMouseDown/componentLuaMouseUp
                // (the nav-button wiring) -- except on a uiCustomComponent, whose own handlers fire
                // only its uiCustomMouse* callbacks. triggerClick() drives buttonClicked -> the
                // value-change / MIDI path. A trigger-on-mouse-down button self-triggers inside
                // CtrlrButton::mouseDown, so we skip the explicit triggerClick() then to avoid a
                // double click.
                const juce::MouseEvent e = makeClickEvent(comp);
                juce::Button* button = findButtonChild(comp);
                const bool triggerOnDown = (bool) mod->getObjectTree()
                    .getChildWithName(Ids::component).getProperty(Ids::uiButtonTriggerOnMouseDown);

                comp->mouseDown(e);
                if (button != nullptr && ! triggerOnDown)
                    button->triggerClick();
                comp->mouseUp(e);
            }
            else
            {
                addError(errors, "action","modulator '" + modName + "' has no component to click");
            }
        }
        else if (actionName == "injectDeviceMidi")
        {
            juce::MidiMessage m;
            if (! parseHexBytes(action.getProperty("bytes", juce::var()).toString(), m))
                addError(errors, "action","injectDeviceMidi: could not parse 'bytes'");
            else
                ctx.midi.injectMidiInput(m); // arrives as if from the (mock) hardware input
        }
        else if (actionName == "injectHostMidi")
        {
            juce::MidiMessage m;
            if (! parseHexBytes(action.getProperty("bytes", juce::var()).toString(), m))
                addError(errors, "action","injectHostMidi: could not parse 'bytes'");
            else
                hostInput.addEvent(m, 0); // carried into the first settle processBlock
        }
        else if (actionName == "callLuaMethod")
        {
            const juce::String methodName = action.getProperty("method", juce::var()).toString();
            const juce::var args = action.getProperty("args", juce::var());
            CtrlrLuaMethod* method = ctx.panel.getCtrlrLuaManager().getMethodManager().getMethodByName(methodName);
            if (method == nullptr)
                addError(errors, "action", "no Lua method named '" + methodName + "'");
            else if (! method->isValid())
                addError(errors, "action", "Lua method '" + methodName + "' is invalid");
            else
            {
                // Always the raw-Lua path, arguments or not. Methods are compiled as globals by
                // name, so we fetch the global, push the args as their Lua types and pcall for one
                // return value. Going through CtrlrLuaMethodManager::call() instead (as this used
                // to for the nullary case) would discard the return value -- which is why callers
                // used to pass a dummy argument just to be able to read one -- and would route a
                // runtime error through CATCH_METHOD_EXCEPTION's modal alert. With pcall the error
                // comes back to us and lands in errors[] as type "lua".
                lua_State* L = ctx.panel.getCtrlrLuaManager().getLuaState();
                lua_getglobal(L, methodName.toRawUTF8());
                if (! lua_isfunction(L, -1))
                {
                    lua_pop(L, 1);
                    addError(errors, "action", "Lua global '" + methodName + "' is not a callable function");
                }
                else
                {
                    const juce::Array<juce::var>* a = args.getArray();
                    const int numArgs = (a != nullptr) ? a->size() : 0;
                    for (int i = 0; i < numArgs; ++i)
                        pushVarToLua(L, (*a)[i]);

                    juce::String err;
                    if (pcallForResult(L, numArgs, stepReturn, err))
                        hasReturn = true;
                    else
                        addError(errors, "lua", "callLuaMethod '" + methodName + "' raised: " + err);
                }
            }
        }
        else if (actionName == "eval")
        {
            // Run an arbitrary Lua chunk in the panel's own lua_State, so panel internals (module
            // tables, upvalue-free globals, anything a method can see) are directly assertable
            // without injecting a throwaway <luaMethod> into a scratch copy of the panel.
            const juce::String chunk = action.getProperty("lua", juce::var()).toString();
            if (chunk.trim().isEmpty())
                addError(errors, "action", "eval: 'lua' is missing or empty");
            else
            {
                juce::String err;
                if (evalLuaChunk(ctx.panel.getCtrlrLuaManager().getLuaState(), chunk, stepReturn, err))
                    hasReturn = true;
                else
                    addError(errors, "lua", "eval failed: " + err);
            }
        }
        else if (actionName == "waitFor")
        {
            // Poll a Lua condition until it goes truthy, pumping the same way settle() does in
            // between. This is how a script waits on something genuinely asynchronous (a dump
            // arriving, a timer firing) without guessing at a `wait` duration.
            const juce::String chunk = action.getProperty("lua", juce::var()).toString();
            const int timeoutMs = (int) action.getProperty("timeoutMs", juce::var(2000));
            const int pollMs = juce::jmax(1, (int) action.getProperty("pollMs", juce::var(25)));

            if (chunk.trim().isEmpty())
            {
                addError(errors, "action", "waitFor: 'lua' is missing or empty");
            }
            else
            {
                lua_State* L = ctx.panel.getCtrlrLuaManager().getLuaState();
                const auto start = std::chrono::steady_clock::now();
                bool satisfied = false, chunkFailed = false;

                for (;;)
                {
                    waitFixed(ctx, pollMs, deviceMidiOut, hostMidiOut, hostInput);
                    hostInput.clear(); // injected input rides only the first block

                    juce::String err;
                    juce::var result;
                    if (! evalLuaChunk(L, chunk, result, err))
                    {
                        addError(errors, "lua", "waitFor condition failed: " + err);
                        chunkFailed = true;
                        break;
                    }
                    stepReturn = result;
                    hasReturn = true;
                    if (isLuaTruthy(result))
                    {
                        satisfied = true;
                        break;
                    }
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - start).count() >= timeoutMs)
                        break;
                }

                if (! satisfied && ! chunkFailed)
                    addError(errors, "action", "waitFor timed out after " + juce::String(timeoutMs)
                                               + " ms waiting for: " + chunk);
            }
            settleDone = true; // the poll loop already pumped and collected
        }
        else if (actionName == "screenshot")
        {
            // Render the live canvas to a PNG. The canvas never had a desktop peer, but it is
            // fully built and laid out, so createComponentSnapshot paints it the same way the
            // editor window would.
            const juce::String path = action.getProperty("file", juce::var()).toString();
            CtrlrPanelCanvas* canvas = ctx.panel.getCanvas();
            if (path.isEmpty())
                addError(errors, "action", "screenshot: 'file' is missing");
            else if (canvas == nullptr)
                addError(errors, "action", "screenshot: the panel has no canvas");
            else
            {
                // The canvas adds an always-on-top ResizableBorderComponent child; leaving it
                // visible would draw a resize frame around every screenshot.
                juce::Component* border = canvas->getResizableBorder();
                const bool borderWasVisible = (border != nullptr) && border->isVisible();
                if (borderWasVisible)
                    border->setVisible(false);

                const juce::Image image = canvas->createComponentSnapshot(canvas->getLocalBounds(), true);

                if (borderWasVisible)
                    border->setVisible(true);

                juce::String fullPath;
                if (! writePng(image, path, fullPath))
                {
                    addError(errors, "action", "screenshot: could not write " + fullPath);
                }
                else
                {
                    juce::var shot = probeReport::makeObject();
                    probeReport::setProp(shot, "file", fullPath);
                    probeReport::setProp(shot, "width", image.getWidth());
                    probeReport::setProp(shot, "height", image.getHeight());
                    probeReport::setProp(step, "screenshot", shot);
                }
            }
        }
        else if (actionName == "paint")
        {
            // Paint one component, synchronously, into an in-memory image -- the way to test paint
            // code headlessly, since repaint() paints nothing here. It renders the component whether
            // or not a user could see it (an off-page editor is legitimately painted before it is
            // shown), and says which in `effectiveVisible`.
            const juce::String modName = action.getProperty("modulator", juce::var()).toString();
            CtrlrModulator* mod = ctx.panel.getModulator(modName);
            CtrlrComponent* comp = (mod != nullptr) ? mod->getComponent() : nullptr;
            if (mod == nullptr)
                addError(errors, "action", "no modulator named '" + modName + "'");
            else if (comp == nullptr)
                addError(errors, "action", "modulator '" + modName + "' has no component to paint");
            else if (comp->getWidth() <= 0 || comp->getHeight() <= 0)
                addError(errors, "action", "paint: '" + modName + "' has zero size");
            else
            {
                // CtrlrCustomComponent::paint silently skips a callback an earlier runtime error
                // disabled, and that error was reported once, in the script that raised it.
                if (dynamic_cast<CtrlrCustomComponent*>(comp) != nullptr)
                    if (CtrlrLuaMethod* pm = paintMethodOf(ctx.panel, *mod))
                        if (! pm->isValid())
                            addWarning(ctx, "paint", modName, "paint callback '" + pm->getName()
                                       + "' is disabled by an earlier error; the component painted without it", index);

                const juce::Image image = comp->createComponentSnapshot(comp->getLocalBounds(), true);

                // The spy does not see a direct paint, so count it here.
                auto tracked = ctx.paintTracking.find(modName);
                if (tracked != ctx.paintTracking.end() && isOurSpy(comp, tracked->second))
                {
                    ++tracked->second.counters.paints;
                    tracked->second.counters.requestsSincePaint = 0;
                }

                juce::var painted = probeReport::makeObject();
                probeReport::setProp(painted, "modulator", modName);
                probeReport::setProp(painted, "width", image.getWidth());
                probeReport::setProp(painted, "height", image.getHeight());
                probeReport::setProp(painted, "effectiveVisible",
                                     probeReport::isEffectivelyVisible(comp, ctx.panel.getCanvas()));

                const juce::String path = action.getProperty("file", juce::var()).toString();
                juce::String fullPath;
                if (path.isNotEmpty() && ! writePng(image, path, fullPath))
                    addError(errors, "action", "paint: could not write " + fullPath);
                else if (path.isNotEmpty())
                    probeReport::setProp(painted, "file", fullPath);
                probeReport::setProp(step, "paint", painted);
            }
        }
        else if (actionName == "mouse")
        {
            runMouseAction(ctx, index, action, step, errors);
        }
        else if (actionName == "setLayerVisible")
        {
            // Set a canvas layer's visibility (layers are pages). Headless there is no live layer
            // Component, so this sets the uiPanelCanvasLayerVisibility tree property directly, which
            // is what the report reads back. Match by uid first, then by name.
            const juce::String layerId = action.getProperty("layer", juce::var()).toString();
            const bool visible = (bool) action.getProperty("visible", juce::var(true));
            const juce::ValueTree editor = ctx.panel.getObjectTree().getChildWithName(Ids::uiPanelEditor);

            juce::ValueTree target;
            for (int pass = 0; pass < 2 && ! target.isValid(); ++pass)
            {
                const juce::Identifier key = (pass == 0) ? Ids::uiPanelCanvasLayerUid : Ids::uiPanelCanvasLayerName;
                for (int i = 0; i < editor.getNumChildren(); ++i)
                {
                    const juce::ValueTree layer = editor.getChild(i);
                    if (layer.hasType(Ids::uiPanelCanvasLayer)
                        && layer.getProperty(key).toString() == layerId)
                    {
                        target = layer;
                        break;
                    }
                }
            }

            if (target.isValid())
                target.setProperty(Ids::uiPanelCanvasLayerVisibility, visible, nullptr);
            else
                addError(errors, "action","no canvas layer with uid or name '" + layerId + "'");
        }
        else
        {
            addError(errors, "action","unknown action '" + actionName + "'");
        }

        if (! settleDone)
        {
            if (isFixedWait)
                waitFixed(ctx, fixedWaitMs, deviceMidiOut, hostMidiOut, hostInput);
            else
                settle(ctx, settleMs, quietMs, deviceMidiOut, hostMidiOut, hostInput);
        }

        // Slice the log lines captured during this step (delivered on the message thread inside
        // settle): console() output (level Lua) and the real Lua error text (level LuaError).
        juce::Array<juce::var> console;
        juce::StringArray capturedErrorText;
        for (size_t i = logStart; i < ctx.log.entries.size(); ++i)
        {
            const LogCapture::Entry& e = ctx.log.entries[i];
            if (e.level == CtrlrLog::Lua)
                console.add(e.message);
            else if (e.level == CtrlrLog::LuaError || e.level == CtrlrLog::LuaRuntimeError)
                capturedErrorText.add(e.message);
        }

        const int errorsBeforeCollect = errors.size();
        collectNewLuaErrors(ctx, errors, capturedErrorText);
        // Surface captured error text even if no method flipped to invalid (e.g. an error raised
        // from a callback that did not disable a named method), so it is never silently dropped.
        if (errors.size() == errorsBeforeCollect)
            for (const auto& line : capturedErrorText)
                addError(errors, "lua", line);

        const juce::Array<juce::var> dialogs = takeDialogs(&errors);

        probeReport::setProp(step, "deviceMidiOut", deviceMidiOut);
        probeReport::setProp(step, "hostMidiOut", hostMidiOut);
        probeReport::setProp(step, "errors", errors);
        probeReport::setProp(step, "console", console);
        probeReport::setProp(step, "dialogs", dialogs);
        if (hasReturn)
            probeReport::setProp(step, "returnValue", stepReturn);
        probeReport::setProp(step, "ok", errors.isEmpty());
        return step;
    }

    // ------------------------------------------------------------------------------------------
    // Session: the long-lived process state, so one process can run several scripts (--stdin).
    // ------------------------------------------------------------------------------------------
    struct Session
    {
        CtrlrProcessor& processor;
        MockMidi& midi;
        juce::AudioSampleBuffer& buffer;
        std::mutex& sendsMutex;
        std::vector<juce::MidiMessage>& deviceSends;
        LogCapture& log;
        juce::File panelFile;
        bool editModeAsSaved = false; // --edit-mode-as-saved

        // Rebuilt on every (re)load:
        CtrlrPanel* panel = nullptr;
        bool savedEditMode = false; // the mode the file opens in, before the probe's patch
        juce::String origIn, origOut;
        juce::var compileErrors;
        juce::Array<juce::var> loadDialogs; // dismissed while the panel loaded
        std::set<juce::String> knownInvalidMethods;
        PaintTracking paintTracking; // every uiCustomComponent's paint counters
    };

    // Attach a PaintSpy to every uiCustomComponent. Each attach gets a fresh token, so a spy can
    // never be mistaken for an earlier one.
    void attachPaintSpies(CtrlrPanel& panel, PaintTracking& tracking)
    {
        static juce::uint32 nextToken = 0;
        for (int i = 0; i < panel.getNumModulators(); ++i)
        {
            CtrlrModulator* mod = panel.getModulatorByIndex(i);
            auto* custom = (mod != nullptr) ? dynamic_cast<CtrlrCustomComponent*>(mod->getComponent()) : nullptr;
            if (custom == nullptr)
                continue;
            TrackedPaint& t = tracking[mod->getName()];
            t.token = ++nextToken;
            custom->setCachedComponentImage(new PaintSpy(*custom, t.counters, t.token));
            t.counters = PaintCounters(); // setCachedComponentImage itself invalidates once
        }
    }

    // finalState.modulators[i].paint, for every component that ever had a spy.
    void addPaintState(juce::var& finalState, CtrlrPanel& panel, const PaintTracking& tracking)
    {
        const juce::var modulators = finalState.getProperty("modulators", juce::var());
        const juce::Array<juce::var>* mods = modulators.getArray();
        if (mods == nullptr)
            return;
        for (juce::var m : *mods)
        {
            const juce::String name = m.getProperty("name", juce::var()).toString();
            const auto t = tracking.find(name);
            CtrlrModulator* mod = panel.getModulator(name);
            if (t == tracking.end() || mod == nullptr)
                continue;

            const PaintCounters& c = t->second.counters;
            CtrlrLuaMethod* pm = paintMethodOf(panel, *mod);
            juce::var p = probeReport::makeObject();
            probeReport::setProp(p, "repaintRequests", c.repaintRequests);
            probeReport::setProp(p, "paints", c.paints);
            probeReport::setProp(p, "pendingRepaint", c.requestsSincePaint > 0);
            probeReport::setProp(p, "tracked", isOurSpy(mod->getComponent(), t->second));
            probeReport::setProp(p, "paintMethod", pm != nullptr ? juce::var(pm->getName()) : juce::var());
            probeReport::setProp(p, "paintMethodValid", pm != nullptr ? juce::var(pm->isValid()) : juce::var());
            probeReport::setProp(m, "paint", p);
        }
    }

    // Take the panel the manager just loaded, rebind its MIDI to the mock devices and capture the
    // Lua baselines. Returns false (with a reason) if no panel is there.
    //
    // The compile-error baseline has to be taken *before* any step runs: a runtime Lua error marks
    // its method invalid, so reading it afterwards would misreport runtime failures as compile
    // errors. knownInvalidMethods is seeded from the same baseline so per-step detection only
    // reports methods that go invalid during a step.
    bool bindLoadedPanel(Session& s, juce::String& errorOut)
    {
        CtrlrManager& manager = s.processor.getManager();
        if (manager.getNumPanels() <= 0)
        {
            errorOut = "no panel was loaded";
            return false;
        }
        s.panel = manager.getPanel(manager.getNumPanels() - 1);
        if (s.panel == nullptr)
        {
            errorOut = "the loaded panel was null";
            return false;
        }

        // Record the panel's original device bindings, then rebind to the mock devices so the
        // panel's MIDI I/O is captured. The file on disk is never touched.
        s.origIn  = s.panel->getProperty(Ids::panelMidiInputDevice).toString();
        s.origOut = s.panel->getProperty(Ids::panelMidiOutputDevice).toString();

        CtrlrMIDIDeviceManager& devMgr = manager.getCtrlrMIDIDeviceManager();
        const juce::String mockIn  = pickMockDevice(devMgr.getManagedDevices(inputDevice),  false);
        const juce::String mockOut = pickMockDevice(devMgr.getManagedDevices(outputDevice), true);
        if (mockIn.isNotEmpty())
            s.panel->setProperty(Ids::panelMidiInputDevice, mockIn, false);
        if (mockOut.isNotEmpty())
            s.panel->setProperty(Ids::panelMidiOutputDevice, mockOut, false);

        // Let the rebind settle (opens the mock devices / starts the input thread).
        s.processor.prepareToPlay(44100, BLOCK_SIZE);
        for (int i = 0; i < 12; ++i)
            juce::MessageManager::getInstance()->runDispatchLoopUntil(25);

        s.compileErrors = probeReport::luaCompileErrors(*s.panel);
        s.loadDialogs = takeDialogs(nullptr);
        if (g_dialogSuppressor != nullptr)
            g_dialogSuppressor->luaState = s.panel->getCtrlrLuaManager().getLuaState();
        s.knownInvalidMethods.clear();
        CtrlrLuaMethodManager& mgr = s.panel->getCtrlrLuaManager().getMethodManager();
        for (int i = 0; i < mgr.getNumMethods(); ++i)
            if (CtrlrLuaMethod* m = mgr.getMethodByIndex(i))
                if (! m->isValid())
                    s.knownInvalidMethods.insert(m->getName());

        attachPaintSpies(*s.panel, s.paintTracking);
        return true;
    }

    // Open the panel file and bind it. Returns false (with a reason) on failure.
    //
    // This is CtrlrManager::openPanelInternal(const File&), plus one patch before addPanel: a panel
    // saved in edit mode is switched to run mode in its tree (unless --edit-mode-as-saved). That
    // makes it load exactly like a file saved with uiPanelEditMode="0", so luaPanelLoaded runs in
    // run mode too. CtrlrX itself opens such a file half in edit mode -- CtrlrPanel::bootstrapPanel
    // re-applies the mode read from the panel root, not from <uiPanelEditor> -- and in that state a
    // control hidden after load only dims. Tests mean the panel as played.
    // CtrlrPanel::openXmlPanel drops JUCE's parse error, so a broken file only shows up as "no
    // panel was loaded" -- and a file that ends inside its root's start tag parses with no error
    // at all and loads as an empty panel that runs clean. Parse the file once more, the way
    // openXmlPanel reads it, and return what is wrong with it (empty if nothing).
    juce::String xmlParseError(const juce::File& file)
    {
        juce::String text;
        if (file.hasFileExtension("panelz"))
        {
            juce::FileInputStream in(file);
            if (! in.openedOk())
                return "cannot read the file";
            juce::GZIPDecompressorInputStream gz(&in, false);
            text = gz.readEntireStreamAsString();
        }
        else
        {
            text = file.loadFileAsString();
        }
        juce::XmlDocument doc(text);
        std::unique_ptr<juce::XmlElement> xml = doc.getDocumentElement();
        if (doc.getLastParseError().isNotEmpty())
            return doc.getLastParseError();
        if (xml == nullptr)
            return "no XML element in the file";
        // XmlDocument reports nothing when the input ends inside a start tag; every complete
        // document ends with '>'.
        if (! text.trimEnd().endsWithChar('>'))
            return "the file ends inside a tag (truncated?)";
        return {};
    }

    bool openPanel(Session& s, juce::String& errorOut)
    {
        try
        {
            CtrlrManager& manager = s.processor.getManager();
            manager.setProperty(Ids::panelLastSaveDir, s.panelFile.getParentDirectory().getFullPathName());
            const bool binary = s.panelFile.getFileExtension().startsWith(".b");
            if (! binary)
            {
                const juce::String parseError = xmlParseError(s.panelFile);
                if (parseError.isNotEmpty())
                {
                    errorOut = "the XML does not parse: " + parseError;
                    return false;
                }
            }
            juce::ValueTree tree = binary ? CtrlrPanel::openBinPanel(s.panelFile)
                                          : CtrlrPanel::openXmlPanel(s.panelFile);
            if (! tree.isValid())
            {
                errorOut = "not a readable panel file";
                return false;
            }
            if (! tree.hasType(Ids::panel))
            {
                errorOut = "the root element is <" + tree.getType().toString() + ">, not <panel>";
                return false;
            }

            tree.setProperty(Ids::panelFilePath, s.panelFile.getFullPathName(), nullptr);

            // No attribute opens in edit mode: CtrlrPanelEditor's constructor sets it, and its
            // restoreState copies only the attributes that are there. So set it in every case.
            juce::ValueTree editor = tree.getChildWithName(Ids::uiPanelEditor);
            s.savedEditMode = editor.isValid() && (bool) editor.getProperty(Ids::uiPanelEditMode, true);
            if (editor.isValid())
                editor.setProperty(Ids::uiPanelEditMode, (s.savedEditMode && s.editModeAsSaved) ? "1" : "0", nullptr);

            const int panelsBefore = manager.getNumPanels();
            manager.addPanel(tree, true);
            if (manager.getNumPanels() <= panelsBefore)
            {
                errorOut = "CtrlrX did not add the panel";
                return false;
            }
        }
        catch (...)
        {
            errorOut = "opening the panel threw";
            return false;
        }
        return bindLoadedPanel(s, errorOut);
    }

    // Run one script object against the bound panel and return the finished report.
    // `logStart` is where this script's console output begins in the shared log capture.
    juce::var runScript(Session& s, const juce::var& script, bool globalConsole, size_t logStart)
    {
        juce::Array<juce::var> warnings;
        RunContext ctx{ s.processor, *s.panel, s.midi, s.buffer, s.sendsMutex, s.deviceSends,
                        s.knownInvalidMethods, s.log, s.paintTracking, warnings,
                        (int) script.getProperty("settleMs", juce::var(DEFAULT_SETTLE_MS)),
                        (int) script.getProperty("quietMs",  juce::var(DEFAULT_QUIET_MS)) };

        // Paint counters are per script: --stdin scripts share a process.
        for (auto& t : s.paintTracking)
            t.second.counters = PaintCounters();

        // The mode this script runs in, and a load-time warning that every script repeats (each
        // --stdin report stands alone).
        CtrlrPanelEditor* editor = s.panel->getEditor(false);
        const bool editMode = editor != nullptr && (bool) editor->getProperty(Ids::uiPanelEditMode);
        if (s.savedEditMode)
            addWarning(ctx, "editMode", "", s.editModeAsSaved
                ? "saved in edit mode and probed as saved (--edit-mode-as-saved): componentVisibility=0 "
                  "set after load only dims the control, and CtrlrX ignores slider drags; the probe's "
                  "actions still act"
                : "saved in edit mode (uiPanelEditMode=1); probed in run mode, as played. CtrlrX opens "
                  "this file half in edit mode; save it with uiPanelEditMode=0, or run with "
                  "--edit-mode-as-saved to probe it as CtrlrX opens it");

        juce::Array<juce::var> stepResults;
        int actionErrorCount = 0;     // the probe could not carry out what a step asked for
        int luaRuntimeErrorCount = 0; // the panel's Lua raised during a step
        const juce::var steps = script.getProperty("steps", juce::var());
        if (const juce::Array<juce::var>* stepArray = steps.getArray())
        {
            for (int i = 0; i < stepArray->size(); ++i)
            {
                juce::var stepResult = executeStep(ctx, i, (*stepArray)[i]);

                // A press nobody released would leak into the next --stdin script, and its
                // handlers never saw an up: the script is wrong, so say so on its last step.
                if (i == stepArray->size() - 1 && ctx.press.active)
                {
                    if (juce::Array<juce::var>* errs = stepResult.getProperty("errors", juce::var()).getArray())
                        addError(*errs, "action", "mouse: the press on '" + ctx.press.modulator
                                                  + "' is still held at the end of the script; finish it with 'up'");
                    probeReport::setProp(stepResult, "ok", false);
                    ctx.press = HeldPress();
                }

                const juce::var stepErrors = stepResult.getProperty("errors", juce::var());
                actionErrorCount     += countErrorsOfType(stepErrors, "action");
                luaRuntimeErrorCount += countErrorsOfType(stepErrors, "lua");
                stepResults.add(stepResult);
            }
        }

        // A repaint request never followed by a paint. "Invalidated", not "repaint() had no
        // effect": the spy cannot tell Lua's repaint() from a property change, a resize or focus,
        // and on screen each of those would have painted. A script that never tests paint can
        // opt out with "paintWarnings": false; the counters in finalState still say it.
        const bool paintWarnings = (bool) script.getProperty("paintWarnings", juce::var(true));
        for (const auto& t : s.paintTracking)
        {
            if (! paintWarnings)
                break;
            CtrlrModulator* mod = s.panel->getModulator(t.first);
            const int pending = t.second.counters.requestsSincePaint;
            if (mod != nullptr && pending > 0 && isOurSpy(mod->getComponent(), t.second))
                addWarning(ctx, "paint", t.first, "invalidated " + juce::String(pending)
                                                  + (pending == 1 ? " time" : " times") + ", painted 0 times since");
        }

        // Every method invalid right now, compile or runtime. A method disabled by a runtime error
        // stays silently dead for the rest of the process, and its error was counted once, in
        // the script that raised it -- later --stdin scripts would otherwise never hear of it.
        juce::Array<juce::var> disabledMethods;
        CtrlrLuaMethodManager& methods = s.panel->getCtrlrLuaManager().getMethodManager();
        for (int i = 0; i < methods.getNumMethods(); ++i)
            if (CtrlrLuaMethod* m = methods.getMethodByIndex(i))
                if (! m->isValid())
                    disabledMethods.add(m->getName());

        juce::var report = probeReport::makeObject();
        probeReport::setProp(report, "reportVersion", PROBE_REPORT_VERSION);

        juce::var panelInfo = probeReport::makeObject();
        probeReport::setProp(panelInfo, "file", s.panelFile.getFullPathName());
        probeReport::setProp(panelInfo, "name", s.panel->getName());
        juce::var origDevices = probeReport::makeObject();
        probeReport::setProp(origDevices, "input", s.origIn);
        probeReport::setProp(origDevices, "output", s.origOut);
        probeReport::setProp(panelInfo, "originalDevices", origDevices);
        probeReport::setProp(panelInfo, "luaCompileErrors", s.compileErrors);
        probeReport::setProp(panelInfo, "dialogs", s.loadDialogs);
        probeReport::setProp(panelInfo, "savedEditMode", s.savedEditMode);
        probeReport::setProp(panelInfo, "editMode", editMode);
        probeReport::setProp(report, "panel", panelInfo);

        probeReport::setProp(report, "steps", stepResults);
        juce::var finalState = probeReport::finalState(*s.panel);
        addPaintState(finalState, *s.panel, s.paintTracking);
        probeReport::setProp(report, "finalState", finalState);
        probeReport::setProp(report, "warnings", warnings);

        // Global console aggregate (opt-in): every console() line this script produced. Gated
        // behind a flag because a chatty panel can emit a lot; the per-step console arrays are
        // always present.
        if (globalConsole)
        {
            juce::Array<juce::var> allConsole;
            for (size_t i = logStart; i < s.log.entries.size(); ++i)
                if (s.log.entries[i].level == CtrlrLog::Lua)
                    allConsole.add(s.log.entries[i].message);
            probeReport::setProp(report, "console", allConsole);
        }

        const int compileErrorCount = s.compileErrors.isArray() ? s.compileErrors.size() : 0;
        // errorCount is the total of all three channels, so a caller that asserts on a single
        // number cannot get a green run while something failed in a channel it forgot to check.
        const int errorCount = compileErrorCount + luaRuntimeErrorCount + actionErrorCount;

        juce::var summary = probeReport::makeObject();
        probeReport::setProp(summary, "errorCount", errorCount); // compile + runtime + action
        probeReport::setProp(summary, "actionErrorCount", actionErrorCount);
        probeReport::setProp(summary, "luaRuntimeErrorCount", luaRuntimeErrorCount);
        probeReport::setProp(summary, "luaCompileErrorCount", compileErrorCount);
        probeReport::setProp(summary, "exitCode", (errorCount > 0) ? 1 : 0);
        probeReport::setProp(summary, "warningCount", warnings.size()); // not in errorCount
        probeReport::setProp(summary, "disabledMethods", disabledMethods); // not in errorCount either
        probeReport::setProp(report, "summary", summary);

        return report;
    }
}

int main(int argc, char** argv)
{
    // Isolate stdout up front: from here on only JSON reports reach the real stdout.
    redirectStdoutToStderr();

   #if defined(__linux__)
    // Never use the user's display. The probe needs none (rendering is in memory), and with one,
    // every modal dialog the probe dismisses is put on screen first and flashes up on the desktop.
    // Without one, dialogs run and are recorded the same way, and screenshots are byte-identical.
    unsetenv("DISPLAY");
    unsetenv("WAYLAND_DISPLAY");
   #endif

    // InitGoogleMock initialises gmock (needed for NiceMock/ON_CALL) and strips its own flags,
    // leaving our positional arguments in place.
    ::testing::InitGoogleMock(&argc, argv);

    // Positional args: <panel-file> [<script-file>]. Flags: --verbose (show stderr chatter),
    // --global-console (add a per-script aggregate of console() output to the report),
    // --stdin (persistent mode: one script object per input line, one report object per output
    // line, so a suite pays the ~0.9 s process startup once instead of per test),
    // --edit-mode-as-saved (load a panel saved in edit mode as CtrlrX opens it, not in run mode).
    juce::StringArray positional;
    bool verbose = false;
    bool globalConsole = false;
    bool stdinMode = false;
    bool editModeAsSaved = false;
    for (int i = 1; i < argc; ++i)
    {
        const juce::String a(argv[i]);
        if (a == "--verbose")                  verbose = true;
        else if (a == "--global-console")      globalConsole = true;
        else if (a == "--stdin")               stdinMode = true;
        else if (a == "--edit-mode-as-saved")  editModeAsSaved = true;
        else if (! a.startsWith("-"))          positional.add(a);
    }
    if (positional.isEmpty())
        failFatal("usage: CtrlrPanelProbe [--verbose] [--global-console] [--stdin] [--edit-mode-as-saved] "
                  "<panel-file> [<script-file>]",
                  "CtrlrPanelProbe drives a CtrlrX panel from a JSON script and reports what it did "
                  "-- MIDI out, Lua errors, final state. No hardware, no display. See Doc/PanelProbe.md.");

    // Quiet by default: silence all stderr chatter so stdout stays the clean JSON contract. The
    // reports are written to the saved real-stdout fd, so exit codes and output are unaffected.
    if (! verbose)
        silenceStdErr();


    const juce::File panelFile = juce::File::getCurrentWorkingDirectory().getChildFile(positional[0]);
    if (! panelFile.existsAsFile())
        failFatal("panel file not found", panelFile.getFullPathName());

    // One-shot mode's optional action script: {"settleMs": N, "steps": [ {action...}, ... ]}. An
    // absent or empty script means "load, rebind, snapshot" only. Malformed JSON is a fatal exit-2.
    juce::var script;
    if (positional.size() >= 2)
    {
        const juce::File scriptFile = juce::File::getCurrentWorkingDirectory().getChildFile(positional[1]);
        if (! scriptFile.exists())
            failFatal("script file not found", scriptFile.getFullPathName());
        const juce::String scriptText = scriptFile.loadFileAsString();
        if (scriptText.trim().isNotEmpty())
        {
            const juce::Result parseResult = juce::JSON::parse(scriptText, script);
            if (parseResult.failed())
                failFatal("script parse error", parseResult.getErrorMessage());
        }
    }

    juce::ScopedJuceInitialiser_GUI juceGui;
    juce::MessageManager::getInstance()->setCurrentThreadAsMessageThread();

    // Suppress the modal "Callback error" dialogs CtrlrX raises on Lua runtime errors (see above).
    // Force JUCE's own (ModalComponentManager-tracked) alert windows so the suppressor can dismiss
    // them, rather than an untracked native/GTK box.
    juce::LookAndFeel::getDefaultLookAndFeel().setUsingNativeAlertWindows(false);
    ModalDialogSuppressor dialogSuppressor;
    g_dialogSuppressor = &dialogSuppressor;

    // Reuse the test mock MIDI subsystem. NiceMock (not StrictMock) because there is no gtest
    // context to verify EXPECT_CALLs against; ON_CALL supplies the behaviour the mock needs.
    NiceMock<MockMidi> midi;
    std::mutex sendsMutex;
    std::vector<juce::MidiMessage> deviceSends; // written from JUCE's MidiOutput background thread

    ON_CALL(midi, getNumDevices()).WillByDefault(Return(1));
    ON_CALL(midi, getNumPorts(_)).WillByDefault(Return(2));
    ON_CALL(midi, sendMidiEvent(_, _, _))
        .WillByDefault(Invoke([&](int, int, juce::MidiMessage m) {
            std::lock_guard<std::mutex> l(sendsMutex);
            deviceSends.push_back(m);
        }));

    auto processor = std::make_shared<CtrlrProcessor>();

    // Capture console() output and real Lua error text into the report. Registered before the
    // panel loads so load-time console() lines land in the aggregate too.
    LogCapture logCapture;
    processor->getManager().getCtrlrLog().addListener(&logCapture);

    // JUCE 8 enumerates MIDI devices asynchronously (see test_ProcessorFixture.h): pump the message
    // loop until the mock devices appear, then refresh CtrlrX's device list.
    if (midi.hasSubsystemMock())
    {
        for (int i = 0; i < 40 && juce::MidiOutput::getAvailableDevices().isEmpty(); ++i)
            juce::MessageManager::getInstance()->runDispatchLoopUntil(25);
        processor->getManager().getCtrlrMIDIDeviceManager().refreshDevices();
    }

    juce::AudioSampleBuffer buffer(2, BLOCK_SIZE);
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        buffer.setSample(0, i, 1.0f * i);
        buffer.setSample(1, i, 2.0f * i);
    }

    Session session{ *processor, midi, buffer, sendsMutex, deviceSends, logCapture, panelFile };
    session.editModeAsSaved = editModeAsSaved;

    juce::String loadError;
    if (! openPanel(session, loadError))
        failFatal("panel failed to load", panelFile.getFullPathName() + ": " + loadError);

    if (! stdinMode)
    {
        const juce::var report = runScript(session, script, globalConsole, 0);
        emitAndExit(report, (int) report.getProperty("summary", juce::var())
                                       .getProperty("exitCode", juce::var(0)));
    }

    // --- Persistent mode ----------------------------------------------------------------------
    // One JSON script object per input line -> one compact JSON report object per output line.
    // EOF exits 0. This exists to amortise the ~1.5 s of process startup (JUCE init, MIDI device
    // enumeration, panel parse) across a whole suite: measured on a large real-world panel, 50 scripts
    // in one process take 1.8 s total against ~75 s of respawning.
    //
    // IMPORTANT: scripts in one process SHARE PANEL STATE -- modulator values, Lua globals, the
    // lot. Isolation is the client's job, and the way to get it is to respawn the process.
    //
    // There is deliberately no in-process `reload` action. It was built and measured: tearing the
    // panel down via CtrlrManager::removePanel and re-opening costs ~1.74 s (0.6-1.1 s of teardown
    // plus ~1.0 s to re-open) and grows RSS by ~0.8 MB a time on the known-leaky teardown path --
    // so it is both slower than simply respawning the process (1.50 s) and leakier. If
    // CtrlrManager ever grows a non-UI panel-teardown path, this becomes worth revisiting.
    std::string line;
    while (std::getline(std::cin, line))
    {
        const juce::String text = juce::String::fromUTF8(line.c_str(), (int) line.size()).trim();
        if (text.isEmpty())
            continue;

        juce::var lineScript;
        const juce::Result parseResult = juce::JSON::parse(text, lineScript);
        if (parseResult.failed())
        {
            // A bad line is the client's bug, not a broken panel: report it and keep going.
            juce::var err = probeReport::makeObject();
            probeReport::setProp(err, "reportVersion", PROBE_REPORT_VERSION);
            probeReport::setProp(err, "error", "script parse error");
            probeReport::setProp(err, "detail", parseResult.getErrorMessage());
            writeReport(err, true);
            continue;
        }

        const size_t logStart = logCapture.entries.size();
        writeReport(runScript(session, lineScript, globalConsole, logStart), true);
    }

    // Same reasoning as emitAndExit: skip teardown rather than trip the leak detector on a
    // known-leaky shutdown path after the reports are already out.
    std::_Exit(0);
}
