#!/usr/bin/env python3
"""Self-test for CtrlrPanelProbe (see Doc/PanelProbe.md).

Runs the probe against known fixtures and asserts the JSON report structure, the exit codes
(0 clean / 1 lua-or-step error / 2 fatal), and a set of concrete observations. Invoked from
ctest as: python3 probe_selftest.py <path-to-CtrlrPanelProbe>

Most scenarios here exist to pin a specific defect the probe used to have, so that it cannot
come back silently: a nullary method whose return value was unreadable, non-scalar arguments
that became nil, failures split across two channels so asserting on one passed vacuously, a
control reported visible while it was hidden, and a per-step settle that cost ~450 ms whatever
the step did.

The fixtures and the scripts this drives are copied next to this file in the build dir, so we
chdir here first.
"""

import json
import os
import subprocess
import sys
import tempfile
import time
from typing import NoReturn

import png_sample  # next to this file, as in the build dir


def run(probe, *args):
    """Run the probe; return (exit_code, parsed_stdout_json_or_None, raw_stdout)."""
    proc = subprocess.run([probe, *args], capture_output=True, text=True, timeout=90)
    try:
        report = json.loads(proc.stdout)
    except json.JSONDecodeError:
        report = None
    return proc.returncode, report, proc.stdout


def run_script(probe, panel, script, *flags):
    """Run an inline script dict against a panel; return (exit_code, report, raw_stdout)."""
    with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False) as f:
        json.dump(script, f)
        path = f.name
    try:
        return run(probe, *flags, panel, path)
    finally:
        os.unlink(path)


def run_stdin(probe, panel, scripts):
    """Feed several script dicts to one --stdin process; return the list of reports."""
    payload = "".join(json.dumps(s) + "\n" for s in scripts)
    proc = subprocess.run([probe, "--stdin", panel], input=payload,
                          capture_output=True, text=True, timeout=90)
    reports = []
    for line in proc.stdout.splitlines():
        if line.strip():
            reports.append(json.loads(line))
    return proc.returncode, reports


def fail(msg) -> NoReturn:
    print("SELFTEST FAIL:", msg, file=sys.stderr)
    sys.exit(1)


def require(cond, msg):
    if not cond:
        fail(msg)


def find_mod(report, name):
    """Return the finalState snapshot of the named modulator, failing if it is absent."""
    for m in report["finalState"]["modulators"]:
        if m["name"] == name:
            return m
    fail(f"no modulator named '{name}' in finalState")


# A uiCustomComponent `surface` (200 x 100) whose paint callback counts into `paintCount` and whose
# mouse callbacks append what they received to `probeLog`, beside a button `btn`.
CUSTOM = "fixtures/fixture_probe_custom.panel"


def probe_log(rep, i):
    """The probeLog table returned by step i, as a list of dicts."""
    log = rep["steps"][i].get("returnValue")
    return log if isinstance(log, list) else []


def action_error_types(step):
    return [e["type"] for e in step["errors"]]


def scenario_17_callback_families(probe):
    # On a custom component only the uiCustomMouse* callbacks fire; componentLuaMouseDown/Up are
    # stored but never called (CtrlrCustomComponent shadows CtrlrComponent's callback members).
    # On a button it is the other way round. Characterization: pins what CtrlrX does today.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "click", "modulator": "surface"},
        {"action": "eval", "lua": "return probeKinds()"},
        {"action": "eval", "lua": "local l = probeLog probeLog = {} return l"},
        {"action": "click", "modulator": "btn"},
        {"action": "eval", "lua": "return probeKinds()"},
    ]})
    require(rep is not None, f"scenario 17 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 17 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    require(rep["steps"][1]["returnValue"] == ["down@100,50", "up@100,50"],
            f"scenario 17 a custom component should get only uiCustomMouse*: {rep['steps'][1]}")
    require(rep["steps"][4]["returnValue"] == ["baseDown@36,20", "baseUp@36,20"],
            f"scenario 17 a button should get componentLuaMouse*: {rep['steps'][4]}")
    # click's down and up carry the left button, as every real click does; nothing else is held.
    for r in probe_log(rep, 2):
        require(r["left"] is True and not (r["right"] or r["shift"] or r["ctrl"] or r["alt"]),
                f"scenario 17 click should hold the left button and no modifier: {r}")


def scenario_18_paint(probe):
    # The paint action paints one component synchronously, in memory, and counts it.
    shot = os.path.abspath("probe_selftest_paint.png")
    if os.path.exists(shot):
        os.unlink(shot)
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "paint", "modulator": "surface"},
        {"action": "eval", "lua": "return paintCount"},
        {"action": "paint", "modulator": "surface", "file": shot},
        {"action": "paint", "modulator": "no-such-modulator"},
    ]})
    require(rep is not None, f"scenario 18 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 1, f"scenario 18 expected exit 1 (the unknown modulator), got {code}")
    got = rep["steps"][0].get("paint")
    require(got is not None and got["modulator"] == "surface" and got["width"] == 200
            and got["height"] == 100, f"scenario 18 step should report the paint: {rep['steps'][0]}")
    assert got is not None
    require("file" not in got, f"scenario 18 no file unless asked: {got}")
    require(rep["steps"][1]["returnValue"] == 1,
            f"scenario 18 the paint callback should have run exactly once: {rep['steps'][1]}")
    require(os.path.exists(shot), "scenario 18 paint with 'file' should write a PNG")
    with open(shot, "rb") as f:
        head = f.read(24)
    os.unlink(shot)
    require(head[:8] == b"\x89PNG\r\n\x1a\n", "scenario 18 output is not a PNG")
    require(head[16:24] == (200).to_bytes(4, "big") + (100).to_bytes(4, "big"),
            "scenario 18 the PNG should be the component alone (200 x 100), not the canvas")
    require(action_error_types(rep["steps"][3]) == ["action"],
            f"scenario 18 an unknown modulator should be an action error: {rep['steps'][3]['errors']}")
    p = find_mod(rep, "surface").get("paint")
    require(p is not None and p["paints"] == 2 and p["pendingRepaint"] is False and p["tracked"] is True,
            f"scenario 18 finalState should count both paints: {p}")

    # paint renders a component whether or not a user could see it; the step says which.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "eval", "lua": "panel:getModulatorByName('surface'):getComponent():setVisible(false) return 1"},
        {"action": "paint", "modulator": "surface"},
        {"action": "eval", "lua": "return paintCount"},
    ]})
    assert rep is not None
    require(code == 0 and rep["steps"][1]["errors"] == [],
            f"scenario 18 painting a hidden component is not an error: {rep['steps'][1]}")
    require(rep["steps"][1].get("paint", {}).get("effectiveVisible") is False,
            f"scenario 18 the step should say the component was hidden: {rep['steps'][1].get('paint')}")
    require(rep["steps"][2]["returnValue"] == 1, f"scenario 18 a hidden component still paints: {rep['steps'][2]}")
    require(not [w for w in rep["warnings"] if w["type"] == "paint"],
            f"scenario 18 a hidden paint is not a warning: {rep['warnings']}")


def scenario_19_silent_repaint(probe):
    # The trap this work exists for. Under the old probe this script came back with errorCount 0
    # and nothing else, so a paint test written behind repaint() passed vacuously for ever.
    # repaint() still cannot paint headlessly -- the report must now say so.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "eval", "lua": "panel:getModulatorByName('surface'):getComponent():repaint() return 1"},
        {"action": "wait", "ms": 300},
        {"action": "eval", "lua": "return paintCount"},
    ]})
    require(rep is not None, f"scenario 19 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 19 a warning must not change the exit code, got {code}")
    require(rep["steps"][2]["returnValue"] == 0,
            f"scenario 19 repaint() is expected NOT to paint headlessly: {rep['steps'][2]}")
    p = find_mod(rep, "surface").get("paint")
    require(p is not None and p["repaintRequests"] >= 1 and p["paints"] == 0 and p["pendingRepaint"] is True,
            f"scenario 19 finalState should show an unpainted repaint: {p}")
    warnings = rep.get("warnings")
    require(isinstance(warnings, list)
            and any(w.get("type") == "paint" and w.get("modulator") == "surface" for w in warnings),
            f"scenario 19 the unpainted repaint should be a 'paint' warning: {warnings}")
    s = rep["summary"]
    require(s.get("warningCount", 0) >= 1 and s["errorCount"] == 0,
            f"scenario 19 a warning is counted apart from errors: {s}")

    # Painting after the repaint clears it.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "eval", "lua": "panel:getModulatorByName('surface'):getComponent():repaint() return 1"},
        {"action": "paint", "modulator": "surface"},
    ]})
    assert rep is not None
    p = find_mod(rep, "surface").get("paint")
    require(p is not None and p["pendingRepaint"] is False and p["paints"] == 1,
            f"scenario 19 a paint after the repaint should clear it: {p}")
    require(not [w for w in rep.get("warnings", []) if w.get("type") == "paint"],
            f"scenario 19 no paint warning once painted: {rep.get('warnings')}")

    # A script that does not test paint can turn the warning off; the counters still tell.
    code, rep, raw = run_script(probe, CUSTOM, {"paintWarnings": False, "steps": [
        {"action": "eval", "lua": "panel:getModulatorByName('surface'):getComponent():repaint() return 1"},
    ]})
    assert rep is not None
    require(not [w for w in rep["warnings"] if w.get("type") == "paint"],
            f"scenario 19 paintWarnings false should silence the warning: {rep['warnings']}")
    p = find_mod(rep, "surface").get("paint")
    require(p is not None and p["pendingRepaint"] is True and p["paints"] == 0,
            f"scenario 19 the counters are kept with paintWarnings false: {p}")


def scenario_20_mouse_click(probe):
    # A click lands where it is told, with the left button down, as a real one does.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "click", "x": 30, "y": 70},
        {"action": "eval", "lua": "return probeLog"},
    ]})
    require(rep is not None, f"scenario 20 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 20 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    log = probe_log(rep, 1)
    require([r["kind"] for r in log] == ["down", "up"], f"scenario 20 expected down, up: {log}")
    down = log[0]
    require((down["x"], down["y"], down["downX"], down["downY"]) == (30, 70, 30, 70),
            f"scenario 20 the click should land at (30, 70): {down}")
    require(down["left"] is True and down["right"] is False and down["clicks"] == 1,
            f"scenario 20 a click holds the left button: {down}")
    require(log[1]["wasClicked"] is True, f"scenario 20 an undragged press is a click: {log[1]}")
    require(rep["steps"][0].get("mouse", {}).get("events") == ["down@30,70", "up@30,70"],
            f"scenario 20 the step should list what it delivered: {rep['steps'][0].get('mouse')}")

    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "click", "x": 250, "y": 10},
        {"action": "mouse", "modulator": "surface", "gesture": "fling", "x": 5, "y": 5},
        {"action": "eval", "lua": "panel:getModulatorByName('surface'):getComponent():setVisible(false) return 1"},
        {"action": "mouse", "modulator": "surface", "gesture": "click", "x": 5, "y": 5},
        {"action": "eval", "lua": "return #probeLog"},
    ]})
    assert rep is not None
    for i, why in ((0, "outside the component"), (1, "an unknown gesture"), (3, "a hidden component")):
        require(action_error_types(rep["steps"][i]) == ["action"],
                f"scenario 20 {why} should be an action error: {rep['steps'][i]['errors']}")
    require(rep["steps"][4]["returnValue"] == 0,
            f"scenario 20 a refused gesture must deliver nothing: {rep['steps'][4]}")

    # mouse drives custom components only: on a button the wrapper is not what a user's mouse hits.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "btn", "gesture": "click", "x": 5, "y": 5},
        {"action": "eval", "lua": "return #probeLog"},
    ]})
    assert rep is not None
    require(action_error_types(rep["steps"][0]) == ["action"],
            f"scenario 20 mouse on a non-custom component should be an action error: {rep['steps'][0]['errors']}")
    require(rep["steps"][1]["returnValue"] == 0,
            f"scenario 20 mouse on a button must deliver nothing: {rep['steps'][1]}")


def scenario_21_mouse_drag(probe):
    # A drag threads the mouse-down position through every event, so a callback asking how far
    # the drag has gone gets the truth. Held across steps, so the middle can be asserted on.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "down", "x": 10, "y": 10},
        {"action": "mouse", "modulator": "surface", "gesture": "drag", "x": 110, "y": 50, "steps": 4},
        {"action": "eval", "lua": "return probeKinds()"},
        {"action": "mouse", "modulator": "surface", "gesture": "up"},
        {"action": "eval", "lua": "return probeLog"},
    ]})
    require(rep is not None, f"scenario 21 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 21 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    require(rep["steps"][2]["returnValue"] == ["down@10,10", "drag@35,20", "drag@60,30", "drag@85,40",
                                               "drag@110,50"],
            f"scenario 21 mid-gesture: a down and four interpolated drags: {rep['steps'][2]}")
    log = probe_log(rep, 4)
    drags = [r for r in log if r["kind"] == "drag"]
    require(all((r["downX"], r["downY"]) == (10, 10) and r["left"] is True for r in drags),
            f"scenario 21 every drag must carry the mouse-down position and the button: {drags}")
    last = drags[-1]
    require((last["dist"], last["distX"], last["distY"]) == (108, 100, 40),
            f"scenario 21 distance from drag start should be 108 (100, 40): {last}")
    up = log[-1]
    require(up["kind"] == "up" and (up["x"], up["y"]) == (110, 50) and up["wasClicked"] is False,
            f"scenario 21 the up lands where the drag ended and is not a click: {up}")

    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "drag", "x": 50, "y": 50},
        {"action": "mouse", "modulator": "surface", "gesture": "up"},
        {"action": "mouse", "modulator": "surface", "gesture": "down", "x": 5, "y": 5},
        {"action": "mouse", "modulator": "surface", "gesture": "down", "x": 6, "y": 6},
    ]})
    assert rep is not None
    for i, why in ((0, "drag with no press"), (1, "up with no press")):
        require(action_error_types(rep["steps"][i]) == ["action"],
                f"scenario 21 {why} should be an action error: {rep['steps'][i]['errors']}")
    # The last step carries its own error (down while pressed) and the end-of-script one.
    last = rep["steps"][3]["errors"]
    require(action_error_types(rep["steps"][3]) == ["action", "action"] and "held" in last[0]["message"]
            and "end of the script" in last[1]["message"],
            f"scenario 21 down while pressed, then the press left held, are action errors: {last}")
    require(rep["summary"]["actionErrorCount"] == 4,
            f"scenario 21 a press still held at script end is an action error too: {rep['summary']}")


def scenario_22_mouse_mods(probe):
    # Right-click, modifiers, and a double-click in JUCE's own event order.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "click", "x": 20, "y": 20,
         "button": "right", "mods": ["shift"]},
        {"action": "mouse", "modulator": "surface", "gesture": "click", "x": 20, "y": 20,
         "mods": ["ctrl", "alt"]},
        {"action": "eval", "lua": "local l = probeLog probeLog = {} return l"},
        {"action": "mouse", "modulator": "surface", "gesture": "doubleClick", "x": 50, "y": 50},
        {"action": "eval", "lua": "return probeLog"},
    ]})
    require(rep is not None, f"scenario 22 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 22 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    log = probe_log(rep, 2)
    right = log[0]
    require(right["right"] is True and right["left"] is False and right["shift"] is True
            and right["popup"] is True, f"scenario 22 shift + right button: {right}")
    keyed = log[2]
    require(keyed["ctrl"] is True and keyed["alt"] is True and keyed["shift"] is False,
            f"scenario 22 ctrl + alt: {keyed}")
    dbl = probe_log(rep, 4)
    require([r["kind"] for r in dbl] == ["down", "up", "down", "up", "doubleClick"],
            f"scenario 22 a double-click is down up down up doubleClick: {dbl}")
    require([r["clicks"] for r in dbl] == [1, 1, 2, 2, 2],
            f"scenario 22 click counts should be 1,1,2,2,2: {[r['clicks'] for r in dbl]}")


def scenario_23_mouse_wheel_and_move(probe):
    # Wheel and move carry no button, as in JUCE.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "wheel", "x": 5, "y": 5, "dx": 0, "dy": -0.5},
        {"action": "mouse", "modulator": "surface", "gesture": "move", "x": 5, "y": 5},
        {"action": "eval", "lua": "return probeLog"},
    ]})
    require(rep is not None, f"scenario 23 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 23 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    log = probe_log(rep, 2)
    require(len(log) == 2, f"scenario 23 expected a wheel and a move: {log}")
    require(log[0]["kind"] == "wheel" and log[0]["dx"] == 0 and log[0]["dy"] == -0.5
            and log[0]["left"] is False, f"scenario 23 wheel deltas should reach the callback: {log[0]}")
    require(log[1]["kind"] == "move" and (log[1]["x"], log[1]["y"]) == (5, 5) and log[1]["left"] is False,
            f"scenario 23 move should reach uiCustomMouseMoveCallback with no button: {log[1]}")


def scenario_24_virtual_press_clock(probe):
    # The press clock is the probe's own, so a wait or a slow settle between down and up cannot
    # turn a click into a long press (JUCE's threshold is 300 ms, the default settle ceiling).
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "down", "x": 10, "y": 10},
        {"action": "wait", "ms": 400},
        {"action": "mouse", "modulator": "surface", "gesture": "up"},
        {"action": "mouse", "modulator": "surface", "gesture": "down", "x": 10, "y": 10},
        {"action": "mouse", "modulator": "surface", "gesture": "up", "holdMs": 500},
        {"action": "eval", "lua": "return probeLog"},
    ]})
    require(rep is not None, f"scenario 24 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 24 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    ups = [r for r in probe_log(rep, 5) if r["kind"] == "up"]
    require(len(ups) == 2, f"scenario 24 expected two ups: {probe_log(rep, 5)}")
    require(ups[0]["wasClicked"] is True and ups[0]["pressLen"] == 10,
            f"scenario 24 a 400 ms wall-clock wait must not make a long press: {ups[0]}")
    require(rep["steps"][2].get("mouse", {}).get("pressMs") == 10,
            f"scenario 24 pressMs should be the virtual age: {rep['steps'][2].get('mouse')}")
    require(ups[1]["wasClicked"] is False and ups[1]["pressLen"] == 510,
            f"scenario 24 holdMs is the way to a long press: {ups[1]}")
    require(rep["steps"][4].get("mouse", {}).get("pressMs") == 510,
            f"scenario 24 pressMs after holdMs 500: {rep['steps'][4].get('mouse')}")


def scenario_25_drag_and_drop_trap(probe):
    # A drag on a drag-and-drop container runs dadStart, then JUCE's startDragging silently does
    # nothing on synthetic events. The probe cannot simulate the drop, so it must say so.
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "dndSurface", "gesture": "down", "x": 10, "y": 10},
        {"action": "mouse", "modulator": "dndSurface", "gesture": "drag", "x": 60, "y": 30, "steps": 3},
        {"action": "mouse", "modulator": "dndSurface", "gesture": "up"},
        {"action": "eval", "lua": "return dadStarted"},
    ]})
    require(rep is not None, f"scenario 25 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 25 a warning must not change the exit code, got {code}: "
                       f"{[s['errors'] for s in rep['steps']]}")
    require(rep["steps"][3]["returnValue"] == 3, f"scenario 25 dadStart should run per drag: {rep['steps'][3]}")
    mouse_warnings = [w for w in rep["warnings"] if w["type"] == "mouse"]
    require(len(mouse_warnings) == 1 and mouse_warnings[0]["modulator"] == "dndSurface",
            f"scenario 25 one 'mouse' warning per drag step: {rep['warnings']}")
    require(rep["summary"]["errorCount"] == 0, f"scenario 25 the trap is a warning: {rep['summary']}")

    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "mouse", "modulator": "surface", "gesture": "down", "x": 10, "y": 10},
        {"action": "mouse", "modulator": "surface", "gesture": "drag", "x": 60, "y": 30},
        {"action": "mouse", "modulator": "surface", "gesture": "up"},
    ]})
    assert rep is not None
    require(not [w for w in rep["warnings"] if w["type"] == "mouse"],
            f"scenario 25 a drag on a plain surface is not a DnD trap: {rep['warnings']}")


def scenario_26_disabled_paint_callback(probe):
    # A runtime error disables a callback for good, and the error is reported once per process. In
    # a later --stdin script `paint` still "paints" -- without the callback. The report must show it.
    _, reports = run_stdin(probe, CUSTOM, [
        {"steps": [{"action": "eval", "lua": "probeFailPaint = true return 1"},
                   {"action": "paint", "modulator": "surface"}]},
        {"steps": [{"action": "eval", "lua": "probeFailPaint = false return paintCount"},
                   {"action": "paint", "modulator": "surface"},
                   {"action": "eval", "lua": "return paintCount"}]},
    ])
    require(len(reports) == 2, f"scenario 26 expected two reports, got {len(reports)}")
    first, second = reports
    require(first["summary"]["luaRuntimeErrorCount"] == 1,
            f"scenario 26 the failing paint should be one lua error: {first['summary']}")
    require(find_mod(first, "surface")["paint"]["paintMethodValid"] is False,
            f"scenario 26 paintMethodValid should be false: {find_mod(first, 'surface')['paint']}")
    require(second["summary"]["errorCount"] == 0, f"scenario 26 not counted again: {second['summary']}")
    p = find_mod(second, "surface")["paint"]
    require(p["paints"] == 1 and p["paintMethod"] == "surfacePaint" and p["paintMethodValid"] is False,
            f"scenario 26 painted, but without the callback: {p}")
    require(any(w["type"] == "paint" and w["modulator"] == "surface" and "surfacePaint" in w["message"]
                for w in second["warnings"]),
            f"scenario 26 a 'paint' warning should name the disabled callback: {second['warnings']}")
    require("surfacePaint" in second["summary"]["disabledMethods"],
            f"scenario 26 disabledMethods should list it: {second['summary']}")
    require(second["steps"][2]["returnValue"] == second["steps"][0]["returnValue"],
            f"scenario 26 the disabled callback must not have run: {second['steps']}")

    code, rep, raw = run_script(probe, CUSTOM, {"steps": [{"action": "paint", "modulator": "surface"}]})
    assert rep is not None
    require(rep["summary"]["disabledMethods"] == [] and find_mod(rep, "surface")["paint"]["paintMethodValid"] is True,
            f"scenario 26 a fresh process starts with nothing disabled: {rep['summary']}")


# One modulator per component type a panel drives with setValue, a button whose Lua sends a bulk
# dump, a label, an inbound-SysEx handler that answers, one-shot timers, argument/return helpers.
# The panel's output channel override (4) puts every modulator's own message on channel 4 (bN = b3);
# messages Lua builds byte by byte keep their channel 1 (b0).
MIDI = "fixtures/fixture_probe_midi.panel"


def scenario_27_midi_out_per_component(probe):
    # deviceMidiOut is the probe's reason to exist, and both panels assert on it constantly. Each
    # step's MIDI must be that step's, complete and in wire order.
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "setValue", "modulator": "cc", "value": 85},
        {"action": "setValue", "modulator": "nrpn", "value": 1000, "settleMs": 500},
        {"action": "setValue", "modulator": "fixed", "value": 1},
        {"action": "setValue", "modulator": "onoff", "value": 1},
        {"action": "setValue", "modulator": "mode", "value": 2},
        {"action": "setValue", "modulator": "bipolar", "value": -12},
    ]})
    require(rep is not None, f"scenario 27 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 27 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    dev = [s["deviceMidiOut"] for s in rep["steps"]]
    require(dev[0] == ["b3 07 55"], f"scenario 27 a CC slider sends one CC: {dev[0]}")
    require(rep["steps"][0]["console"] == ["cc 85"],
            f"scenario 27 console() from the value-change callback lands on the step: {rep['steps'][0]['console']}")
    # NRPN 300 = 0x02 0x2c, value 1000 = 0x07 0x68: number MSB, LSB, then value MSB, LSB.
    require(dev[1] == ["b3 63 02", "b3 62 2c", "b3 06 07", "b3 26 68"],
            f"scenario 27 an NRPN burst arrives whole and in order: {dev[1]}")
    require(all(m in rep["steps"][1]["hostMidiOut"] for m in dev[1]),
            f"scenario 27 the panel echoes its output to the host: {rep['steps'][1]['hostMidiOut']}")
    require(dev[2] == ["b3 16 40"], f"scenario 27 a fixed slider sends its mapped value (mid=64): {dev[2]}")
    require(dev[3] == ["b3 15 01"], f"scenario 27 a toggle sends its value: {dev[3]}")
    # CtrlrCombo::setComponentValue re-enters itself when the value changes from outside, so it
    # may send twice (a CtrlrX property, not the probe's). Both copies must be the right message.
    require(1 <= len(dev[4]) <= 2 and set(dev[4]) == {"b3 14 02"},
            f"scenario 27 a combo sends CC 20 = 2 (possibly twice): {dev[4]}")
    m = {x["name"]: x for x in rep["finalState"]["modulators"]}
    require((m["fixed"]["value"], m["fixed"]["text"]) == (1.0, "mid") and m["mode"]["text"] == "Auto",
            f"scenario 27 a value-mapped control reports index and text: {m['fixed']}, {m['mode']}")
    # setValue takes the modulator's own units, negative ones included.
    require((m["bipolar"]["value"], m["bipolar"]["min"], m["bipolar"]["max"]) == (-12.0, -64.0, 63.0),
            f"scenario 27 a bipolar slider keeps its negative value and range: {m['bipolar']}")


def scenario_28_bulk_output(probe):
    # A dump: one long SysEx and a burst of 70 CCs from one click, all captured on that step, intact.
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "click", "modulator": "dumpBtn"},
        {"action": "callLuaMethod", "method": "sendDump", "args": [3]},
        {"action": "eval", "lua": "panel:sendMidiMessageNow(CtrlrMidiMessage(MemoryBlock({0xC0, 0x05}))) return 1"},
    ]})
    require(rep is not None, f"scenario 28 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 28 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    dev = rep["steps"][0]["deviceMidiOut"]
    sysex = [i % 128 for i in range(1, 298)]
    require(len(dev) == 71, f"scenario 28 expected 1 SysEx + 70 CCs, got {len(dev)}")
    require(dev[0] == " ".join(f"{b:02x}" for b in [0xF0, 0x7D, 0x10] + sysex + [0xF7]),
            f"scenario 28 the 301-byte SysEx must arrive whole: {dev[0][:60]}...")
    require(dev[1:] == [f"b0 30 {i % 128:02x}" for i in range(1, 71)],
            f"scenario 28 the CC burst must arrive complete and in order: {dev[1:6]}...")
    s = rep["steps"][1]
    require(s["returnValue"] == 4 and len(s["deviceMidiOut"]) == 4,
            f"scenario 28 callLuaMethod's MIDI and return value belong to its step: {s.get('returnValue')}, "
            f"{len(s['deviceMidiOut'])}")
    require(rep["steps"][2]["deviceMidiOut"] == ["c0 05"],
            f"scenario 28 a two-byte program change from eval: {rep['steps'][2]['deviceMidiOut']}")


def scenario_29_device_input(probe):
    # injectDeviceMidi drives the input path: a CC moves its modulator, a SysEx reaches
    # luaPanelMidiReceived, and the panel's answer is on the same step. Inbound messages match on
    # the panel's channel too (4), not the modulator's own.
    request = [0xF0, 0x7D, 0x01] + list(range(72)) + [0xF7]
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "injectDeviceMidi", "bytes": "B3 07 22"},
        {"action": "injectDeviceMidi", "bytes": " ".join(f"{b:02X}" for b in request)},
        {"action": "eval", "lua": "return {ccCount, midiInCount}"},
    ]})
    require(rep is not None, f"scenario 29 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 29 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    require(find_mod(rep, "cc")["value"] == 34.0, f"scenario 29 an inbound CC 7 should set 'cc' to 34: {find_mod(rep, 'cc')}")
    require("in 3" in rep["steps"][0]["console"],
            f"scenario 29 luaPanelMidiReceived should see the CC: {rep['steps'][0]['console']}")
    require(rep["steps"][1]["deviceMidiOut"] == [f"f0 7d 02 {len(request):02x} f7"],
            f"scenario 29 the answer to an inbound SysEx is on the inject's step: {rep['steps'][1]['deviceMidiOut']}")
    require(rep["steps"][2]["returnValue"] == [1, 2],
            f"scenario 29 one value change, two messages received: {rep['steps'][2]['returnValue']}")


def scenario_30_timers_and_dialogs(probe):
    # A panel timer's output lands on whichever step is pumping when it fires: wait for it.
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "callLuaMethod", "method": "startTick", "args": [50]},
        {"action": "wait", "ms": 500},
    ]})
    require(rep is not None, f"scenario 30 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    ticks = [(s["deviceMidiOut"], s["console"]) for s in rep["steps"]]
    require(code == 0 and ticks[0] == ([], []) and ticks[1] == (["b0 1e 63"], ["tick"]),
            f"scenario 30 the timer's MIDI and console belong to the wait it fired in: {ticks}")
    require(rep["panel"]["dialogs"] == [] and all(s["dialogs"] == [] for s in rep["steps"]),
            f"scenario 30 no dialog, no entry: {rep['panel']['dialogs']}")

    # A timer callback that raises: CtrlrX disables it and says so only in a modal dialog, which the
    # probe dismisses. It used to vanish -- errorCount 0, and the timer silently dead. The callback
    # stops its timer before raising, so waitFor on that is a condition, not a guessed duration.
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "callLuaMethod", "method": "startBadTick", "args": [50]},
        {"action": "waitFor", "lua": "return not timer:isTimerRunning(8)", "timeoutMs": 3000},
    ]})
    assert rep is not None
    require(code == 1, f"scenario 30 a raising timer callback must fail the run, got exit {code}")
    errs = rep["steps"][1]["errors"]
    require(len(errs) == 1 and errs[0]["type"] == "lua" and "tick boom" in errs[0]["message"],
            f"scenario 30 the timer error should be a 'lua' error with its message: {errs}")
    dlg = rep["steps"][1]["dialogs"]
    require(len(dlg) == 1 and dlg[0]["kind"] == "alert" and dlg[0]["title"].startswith("Timer callback error"),
            f"scenario 30 the dismissed dialog should be listed: {dlg}")

    # Panel Lua's own dialogs are dismissed as Cancel: the step says which, and what came back.
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "eval", "lua": "return utils.questionWindow('Erase?', 'Erase all programs?', 'Erase', 'Keep')"},
        {"action": "eval", "lua": "utils.infoWindow('Done', 'All sent') return 1"},
    ]})
    assert rep is not None
    require(code == 0, f"scenario 30 a dismissed dialog is not an error, got {code}: {[s['errors'] for s in rep['steps']]}")
    require(rep["steps"][0]["returnValue"] is False,
            f"scenario 30 a dismissed question answers false: {rep['steps'][0].get('returnValue')}")
    q = rep["steps"][0]["dialogs"]
    require(len(q) == 1 and q[0]["title"] == "Erase?" and q[0]["buttons"] == ["Erase", "Keep"] and q[0]["result"] == 0,
            f"scenario 30 the question dialog should be listed with its buttons: {q}")
    i = rep["steps"][1]["dialogs"]
    require(len(i) == 1 and i[0]["title"] == "Done" and i[0]["kind"] == "alert",
            f"scenario 30 the info dialog should be listed: {i}")


def scenario_31_value_shapes(probe):
    # Argument and return shapes the panels rely on, through the typed C-API path.
    code, rep, raw = run_script(probe, MIDI, {"steps": [
        {"action": "callLuaMethod", "method": "argTypes", "args": [1, None, "x"]},
        {"action": "callLuaMethod", "method": "identity", "args": [7]},
        {"action": "callLuaMethod", "method": "identity", "args": [-5]},
        {"action": "callLuaMethod", "method": "identity", "args": [0.25]},
        {"action": "callLuaMethod", "method": "identity", "args": [True]},
        {"action": "callLuaMethod", "method": "identity", "args": [""]},
        {"action": "eval", "lua": "return false"},
        {"action": "eval", "lua": "return -2.5"},
        {"action": "eval", "lua": "return string.rep('x', 200000)"},
        {"action": "eval", "lua": "local t = {} for i = 1, 1000 do t[i] = i end return t"},
        {"action": "eval", "lua": "return {[0] = 'a', [1] = 'b'}"},
        {"action": "callLuaMethod", "method": "setName", "args": ["hello"]},
    ]})
    require(rep is not None, f"scenario 31 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 31 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    rv = [s.get("returnValue") for s in rep["steps"]]
    require(rv[0] == "number,nil,string", f"scenario 31 a null argument keeps its place as nil: {rv[0]}")
    require(rv[1:6] == [7, -5, 0.25, True, ""], f"scenario 31 scalar round trips: {rv[1:6]}")
    require(rv[6] is False and rv[7] == -2.5, f"scenario 31 false is not nil; numbers keep sign and fraction: {rv[6:8]}")
    require(isinstance(rv[8], str) and len(rv[8]) == 200000, "scenario 31 a 200 KB string must come back whole")
    require(rv[9] == list(range(1, 1001)), "scenario 31 a 1000-element array must come back whole")
    require(rv[10] == {"0": "a", "1": "b"}, f"scenario 31 keys not exactly 1..n make an object: {rv[10]}")
    name = find_mod(rep, "name")
    require(name["text"] == "hello" and name["componentType"] == "uiLabel",
            f"scenario 31 finalState text follows Lua setText: {name}")
    bip = find_mod(rep, "bipolar")
    require(bip["componentRectangle"] == "96 16 72 96" and bip["vstIndex"] == 2,
            f"scenario 31 geometry and vstIndex as in the panel: {bip}")


def scenario_32_png_pixels(probe):
    # The stdlib PNG sampler against the probe's own PNGs: a solid Lua fill and the panel background,
    # sampled away from edges and text (D7: no whole-image comparisons).
    paint_png = os.path.abspath("probe_selftest_pixels_paint.png")
    shot_png = os.path.abspath("probe_selftest_pixels_shot.png")
    code, rep, raw = run_script(probe, CUSTOM, {"steps": [
        {"action": "paint", "modulator": "surface", "file": paint_png},
        {"action": "screenshot", "file": shot_png},
    ]})
    require(rep is not None, f"scenario 32 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 32 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    try:
        fill = (0x20, 0x60, 0xA0, 0xFF)  # surfacePaint: g:fillAll(Colour(0xff2060a0))
        comp = png_sample.load(paint_png)
        require((comp.width, comp.height) == (200, 100), f"scenario 32 component PNG size: {comp.width}x{comp.height}")
        got = png_sample.pixel(comp, 100, 50)
        require(png_sample.close(got, fill), f"scenario 32 the Lua fill should read {fill}, got {got}")
        shot = png_sample.load(shot_png)
        got = png_sample.pixel(shot, 16 + 100, 16 + 50)  # surface sits at (16, 16) on the canvas
        require(png_sample.close(got, fill), f"scenario 32 the screenshot shows the fill: {got}")
        got = png_sample.pixel(shot, 500, 300)  # empty canvas: uiPanelBackgroundColour ffefefef
        require(png_sample.close(got, (0xEF, 0xEF, 0xEF, 0xFF)), f"scenario 32 the panel background: {got}")
        require(not png_sample.close(fill, (0xEF, 0xEF, 0xEF)), "scenario 32 close() must tell them apart")
    finally:
        for p in (paint_png, shot_png):
            if os.path.exists(p):
                os.unlink(p)


# Saved with uiPanelEditMode="1". knobA is hidden from luaPanelLoaded, knobB by `sel`'s value-change
# callback when it goes to 1 ("Hide"); `cc` is a plain CC 7 slider.
EDITMODE = "fixtures/fixture_probe_editmode.panel"


def editmode_warnings(rep):
    return [w for w in rep.get("warnings", []) if w.get("type") == "editMode"]


def scenario_33_edit_mode(probe):
    # A panel saved in edit mode is probed as played (run mode), whatever the file says. CtrlrX
    # itself opens such a file half in edit mode, where a control hidden at runtime only dims and
    # stays visible; a test of what the user sees would pass or fail on that by accident.
    shots = {mode: os.path.abspath(f"probe_selftest_editmode_{mode}.png") for mode in ("run", "saved")}
    reps = {}
    try:
        for mode, flags in (("run", ()), ("saved", ("--edit-mode-as-saved",))):
            code, rep, raw = run_script(probe, EDITMODE, {"steps": [
                {"action": "setValue", "modulator": "sel", "value": 1},
                {"action": "setValue", "modulator": "cc", "value": 42},
                {"action": "screenshot", "file": shots[mode]},
            ]}, *flags)
            require(rep is not None, f"scenario 33 ({mode}) stdout was not valid JSON:\n{raw}")
            assert rep is not None
            require(code == 0, f"scenario 33 ({mode}) expected exit 0, got {code}: "
                               f"{[s['errors'] for s in rep['steps']]}")
            # Actions are the same in both modes: setValue drives the component directly.
            require("b3 07 2a" in rep["steps"][1]["deviceMidiOut"],
                    f"scenario 33 ({mode}) setValue on cc emits its CC: {rep['steps'][1]['deviceMidiOut']}")
            w = editmode_warnings(rep)
            require(len(w) == 1 and w[0]["modulator"] == "" and "step" not in w[0],
                    f"scenario 33 ({mode}) one load-time editMode warning: {rep.get('warnings')}")
            reps[mode] = rep

        run, saved = reps["run"], reps["saved"]
        require(run["panel"].get("savedEditMode") is True and run["panel"].get("editMode") is False,
                f"scenario 33 saved in edit mode, probed in run mode: {run['panel']}")
        require("--edit-mode-as-saved" in editmode_warnings(run)[0]["message"],
                f"scenario 33 the default warning names the opt-out: {editmode_warnings(run)}")
        for name in ("knobA", "knobB"):  # hidden at load, hidden at runtime: both really hidden
            require(find_mod(run, name)["effectiveVisible"] is False,
                    f"scenario 33 {name} should be hidden in run mode: {find_mod(run, name)}")

        require(saved["panel"].get("savedEditMode") is True and saved["panel"].get("editMode") is True,
                f"scenario 33 --edit-mode-as-saved keeps edit mode: {saved['panel']}")
        require("probed as saved" in editmode_warnings(saved)[0]["message"],
                f"scenario 33 the flag's warning: {editmode_warnings(saved)}")
        # Pins CtrlrX's own split state on purpose (CtrlrPanel::bootstrapPanel re-applies the mode
        # from the panel root, not from <uiPanelEditor>): hidden at load is hidden, hidden at runtime
        # only dims. A CtrlrX fix shows up here as a deliberate change.
        require(find_mod(saved, "knobA")["effectiveVisible"] is False
                and find_mod(saved, "knobB")["effectiveVisible"] is True,
                f"scenario 33 as saved: knobA hidden, knobB dimmed but visible: "
                f"{find_mod(saved, 'knobA')} {find_mod(saved, 'knobB')}")

        # Pixels: the 8 px snap grid and the dimmed knobB are drawn only in edit mode.
        bg = (0xEF, 0xEF, 0xEF, 0xFF)  # uiPanelBackgroundColour1
        img = {mode: png_sample.load(shots[mode]) for mode in shots}
        grid = (400, 304)  # on a grid line, in an empty canvas area
        require(png_sample.close(png_sample.pixel(img["run"], *grid), bg)
                and not png_sample.close(png_sample.pixel(img["saved"], *grid), bg),
                f"scenario 33 grid line: run {png_sample.pixel(img['run'], *grid)}, "
                f"saved {png_sample.pixel(img['saved'], *grid)}")
        thumb = (119, 83)  # knobB's thumb at value 0, off the grid lines
        require(png_sample.close(png_sample.pixel(img["run"], *thumb), bg)
                and not png_sample.close(png_sample.pixel(img["saved"], *thumb), bg),
                f"scenario 33 knobB: run {png_sample.pixel(img['run'], *thumb)}, "
                f"saved {png_sample.pixel(img['saved'], *thumb)}")
    finally:
        for p in shots.values():
            if os.path.exists(p):
                os.unlink(p)

    # A file saved in run mode says so, and gets no warning.
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_visibility.panel", {"steps": []})
    assert rep is not None
    require(rep["panel"].get("savedEditMode") is False and rep["panel"].get("editMode") is False
            and not editmode_warnings(rep),
            f"scenario 33 a run-mode file: {rep['panel']} {rep.get('warnings')}")


def scenario_34_unreadable_panel(probe):
    # A file that is not a panel must exit 2 with a JSON error that says why. A file that ends
    # inside its root's start tag used to parse without error and load as an empty panel with a
    # clean run; other XML errors exited 2 as "no panel was loaded", without the parser's reason.
    with open("fixtures/fixture_probe_midi.panel", encoding="utf-8") as f:
        good = f.read()
    cut = good.index('<modulator', len(good) // 2)
    stray_quote = good[:cut] + '<modulator name="a"b"' + good[cut + len('<modulator'):]
    for name, text in (("truncated", '<?xml version="1.0"?>\n<panel name="x"'),
                       ("cut-in-half", good[:len(good) // 2]),
                       ("stray-quote", stray_quote),
                       ("not-a-panel", '<?xml version="1.0"?>\n<notAPanel name="x"/>\n')):
        with tempfile.NamedTemporaryFile("w", suffix=".panel", delete=False) as f:
            f.write(text)
            path = f.name
        try:
            code, rep, raw = run(probe, path)
            require(code == 2 and rep is not None and rep.get("error") == "panel failed to load",
                    f"scenario 34 {name}: expected exit 2 with 'panel failed to load', got {code}: {raw[:300]}")
            assert rep is not None
            require("summary" not in rep, f"scenario 34 {name}: a fatal report has no summary: {rep}")
            expect = "<notAPanel>" if name == "not-a-panel" else "does not parse"
            require(expect in rep.get("detail", ""), f"scenario 34 {name}: detail should say '{expect}': {rep}")
            if name == "truncated":
                proc = subprocess.run([probe, "--stdin", path], input='{"steps": []}\n',
                                      capture_output=True, text=True, timeout=90)
                require(proc.returncode == 2 and '"panel failed to load"' in proc.stdout,
                        f"scenario 34 --stdin: expected exit 2, got {proc.returncode}: {proc.stdout[:300]}")
        finally:
            os.unlink(path)


def main():
    if len(sys.argv) < 2:
        fail("usage: probe_selftest.py <path-to-CtrlrPanelProbe>")
    probe = sys.argv[1]

    # Fixtures / scripts are copied next to this script in the build dir.
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # --- Scenario 1: clean run with a setValue driving an onChange callback (exit 0) ---
    code, rep, raw = run(probe, "fixtures/fixture_lua_valuechange.panel", "probe_selftest.json")
    require(rep is not None, f"scenario 1 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 1 expected exit 0, got {code}")

    # Structure
    for key in ("panel", "steps", "finalState", "summary"):
        require(key in rep, f"scenario 1 report missing top-level key '{key}'")
    require(rep["panel"]["luaCompileErrors"] == [], "scenario 1 expected no compile errors")
    require(len(rep["steps"]) == 1, "scenario 1 expected exactly one step")

    require(rep["reportVersion"] == 1, "scenario 1 report should carry reportVersion 1")

    step = rep["steps"][0]
    for key in ("index", "action", "deviceMidiOut", "hostMidiOut", "errors", "ok"):
        require(key in step, f"scenario 1 step missing key '{key}'")
    require(step["ok"] is True, f"scenario 1 step not ok: {step['errors']}")
    require(step["errors"] == [], f"scenario 1 unexpected errors: {step['errors']}")

    # Observations: the onChange callback fired once with the driven value, via the GUI source (4).
    require(find_mod(rep, "src")["value"] == 40.0, "scenario 1 'src' should read 40")
    require(find_mod(rep, "change-count")["value"] == 1.0,
            "scenario 1 onChange should fire exactly once (change-count == 1)")
    require(find_mod(rep, "last-value")["value"] == 40.0,
            "scenario 1 onChange should receive value 40")
    require(find_mod(rep, "last-source")["value"] == 4.0,
            "scenario 1 onChange source should be changedByGUI (4)")
    require(rep["summary"]["exitCode"] == 0, "scenario 1 summary.exitCode should be 0")

    # --- Scenario 2: panel with an invalid Lua method -> exit 1, compile error reported ---
    code, rep, raw = run(probe, "fixtures/fixture_lua_invalid.panel")
    require(rep is not None, f"scenario 2 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 1, f"scenario 2 expected exit 1, got {code}")
    require(len(rep["panel"]["luaCompileErrors"]) >= 1,
            "scenario 2 should report at least one luaCompileError")
    require(rep["summary"]["exitCode"] == 1, "scenario 2 summary.exitCode should be 1")
    # errorCount is the single number a caller asserts on: it must include compile errors.
    require(rep["summary"]["errorCount"] == rep["summary"]["luaCompileErrorCount"] >= 1,
            f"scenario 2 errorCount should total the compile errors: {rep['summary']}")

    # --- Scenario 3: nonexistent panel -> fatal exit 2 with a JSON error object ---
    code, rep, raw = run(probe, "this_panel_does_not_exist.panel")
    require(rep is not None, f"scenario 3 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 2, f"scenario 3 expected exit 2, got {code}")
    require("error" in rep, "scenario 3 should report an 'error' field")

    # --- Scenario 4: click drives the component mouse-up path -> componentLuaMouseUp fires ---
    code, rep, raw = run(probe, "fixtures/fixture_probe_click.panel", "probe_click.json")
    require(rep is not None, f"scenario 4 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 4 expected exit 0, got {code}")
    require("clicked" in rep["steps"][0]["console"],
            f"scenario 4 click should log 'clicked' via componentLuaMouseUp: {rep['steps'][0]['console']}")

    # --- Scenario 5: sub-type value flips dependent controls' reported visibility ---
    code, rep, raw = run(probe, "fixtures/fixture_probe_visibility.panel", "probe_visibility.json")
    require(rep is not None, f"scenario 5 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 5 expected exit 0, got {code}")
    # value 100 (>= 64): change-count hidden, last-value shown.
    require(find_mod(rep, "change-count")["visible"] is False,
            "scenario 5 'change-count' should be hidden after setValue 100")
    require(find_mod(rep, "last-value")["visible"] is True,
            "scenario 5 'last-value' should be visible after setValue 100")
    require(len(rep["finalState"]["layers"]) >= 1, "scenario 5 report should carry layers")
    require("uid" in rep["finalState"]["layers"][0], "scenario 5 layers should carry a uid")

    # --- Scenario 6: console() capture + callLuaMethod with args + return value ---
    code, rep, raw = run(probe, "fixtures/fixture_probe_console.panel", "probe_console.json")
    require(rep is not None, f"scenario 6 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 6 expected exit 0, got {code}")
    require("hello 42" in rep["steps"][0]["console"],
            f"scenario 6 step 0 should capture console('hello 42'): {rep['steps'][0]['console']}")
    require(rep["steps"][1].get("returnValue") == "7:x:true",
            f"scenario 6 echo(7,'x',true) return should be '7:x:true': {rep['steps'][1].get('returnValue')}")

    # --- Scenario 7: setLayerVisible flips a layer's reported visibility ---
    code, rep, raw = run(probe, "fixtures/fixture_probe_layers.panel", "probe_layers.json")
    require(rep is not None, f"scenario 7 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 7 expected exit 0, got {code}")
    page2 = [l for l in rep["finalState"]["layers"] if l["name"] == "Page 2"]
    require(len(page2) == 1, "scenario 7 should have a 'Page 2' layer")
    require(page2[0]["visible"] is False, "scenario 7 'Page 2' should be hidden after setLayerVisible")

    # --- Scenario 8: a nullary callLuaMethod reports its return value ---
    # Used to go through CtrlrLuaMethodManager::call(), which discarded the return -- so callers
    # passed a dummy argument purely to reach the raw-Lua path and read a value back.
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_console.panel", {"steps": [
        {"action": "callLuaMethod", "method": "echo"},
    ]})
    require(rep is not None, f"scenario 8 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require("returnValue" in rep["steps"][0],
            "scenario 8 a nullary callLuaMethod should still report returnValue")
    require(rep["steps"][0]["returnValue"] == "nil:nil:nil",
            f"scenario 8 echo() should return 'nil:nil:nil': {rep['steps'][0].get('returnValue')}")

    # --- Scenario 9: eval reaches panel internals; tables convert both ways ---
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_console.panel", {"steps": [
        {"action": "eval", "lua": "return {1, 2, {a = 1, b = 'two'}}"},
        {"action": "eval", "lua": "string.rep('ab', 2)"},                  # REPL-style expression
        {"action": "eval", "lua": "local t = {} t.self = t return t"},     # cycle
        {"action": "callLuaMethod", "method": "echo", "args": [[10, 20], {"k": "v"}, 1]},
    ]})
    require(rep is not None, f"scenario 9 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 9 expected exit 0, got {code}: {[s['errors'] for s in rep['steps']]}")
    require(rep["steps"][0]["returnValue"] == [1.0, 2.0, {"a": 1.0, "b": "two"}],
            f"scenario 9 nested table should convert to JSON: {rep['steps'][0].get('returnValue')}")
    require(rep["steps"][1]["returnValue"] == "abab",
            f"scenario 9 a bare expression should yield its value: {rep['steps'][1].get('returnValue')}")
    require(rep["steps"][2]["returnValue"] == {"self": "<cycle>"},
            f"scenario 9 a self-referential table should report <cycle>: {rep['steps'][2].get('returnValue')}")
    # Non-scalar args used to silently arrive as nil; now they are real Lua tables.
    require(rep["steps"][3]["returnValue"].startswith("table:"),
            f"scenario 9 an array arg should reach Lua as a table: {rep['steps'][3].get('returnValue')}")

    # --- Scenario 10: a throwing method lands in errors[] as type 'lua' AND in errorCount ---
    # The old split (step.error vs step.luaErrors) let a test assert on luaErrorCount and pass
    # green while the method it called threw on every invocation.
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_console.panel", {"steps": [
        {"action": "eval", "lua": "error('boom')"},
        {"action": "setValue", "modulator": "no-such-modulator", "value": 1},
    ]})
    require(rep is not None, f"scenario 10 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 1, f"scenario 10 expected exit 1, got {code}")
    require([e["type"] for e in rep["steps"][0]["errors"]] == ["lua"],
            f"scenario 10 a raising chunk should be a 'lua' error: {rep['steps'][0]['errors']}")
    require("boom" in rep["steps"][0]["errors"][0]["message"],
            f"scenario 10 the Lua message should survive: {rep['steps'][0]['errors']}")
    require([e["type"] for e in rep["steps"][1]["errors"]] == ["action"],
            f"scenario 10 a missing modulator should be an 'action' error: {rep['steps'][1]['errors']}")
    require(rep["steps"][0]["ok"] is False and rep["steps"][1]["ok"] is False,
            "scenario 10 both steps should be not-ok")
    s = rep["summary"]
    require(s["errorCount"] == 2 and s["luaRuntimeErrorCount"] == 1 and s["actionErrorCount"] == 1,
            f"scenario 10 errorCount must total every channel: {s}")

    # --- Scenario 11: a Lua setVisible(false) shows up in liveVisible/effectiveVisible ---
    # componentVisibility (what `visible` reports) is never written by comp:setVisible(), so the
    # tree-only report claimed visible: true for a control that was plainly hidden.
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_setvisible.panel", {"steps": [
        {"action": "callLuaMethod", "method": "hideTarget"},
    ]})
    require(rep is not None, f"scenario 11 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 11 expected exit 0, got {code}: {rep['steps'][0]['errors']}")
    hidden = find_mod(rep, "last-value")
    require(hidden["visible"] is True,
            "scenario 11 the tree property should still say visible (that is the point)")
    require(hidden["liveVisible"] is False,
            f"scenario 11 liveVisible should be False after setVisible(false): {hidden}")
    require(hidden["effectiveVisible"] is False,
            f"scenario 11 effectiveVisible should be False after setVisible(false): {hidden}")
    shown = find_mod(rep, "change-count")
    require(shown["liveVisible"] is True and shown["effectiveVisible"] is True,
            f"scenario 11 an untouched control should stay visible: {shown}")
    require("canvasRectangle" in shown,
            f"scenario 11 a placed control should report absolute bounds: {shown}")

    # --- Scenario 12: two overlapping controls are reported in finalState.overlaps ---
    code, rep, raw = run(probe, "fixtures/fixture_probe_overlap.panel")
    require(rep is not None, f"scenario 12 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 12 expected exit 0, got {code}")
    laps = rep["finalState"]["overlaps"]
    require(len(laps) == 1, f"scenario 12 expected exactly one overlap: {laps}")
    require({laps[0]["a"], laps[0]["b"]} == {"change-count", "last-value"},
            f"scenario 12 wrong pair reported: {laps}")
    require(laps[0]["rect"] == "150 40 38 72", f"scenario 12 wrong intersection: {laps}")
    # The same panel's non-overlapping controls must not be reported.
    code, rep2, _ = run(probe, "fixtures/fixture_probe_setvisible.panel")
    assert rep2 is not None
    require(rep2["finalState"]["overlaps"] == [],
            f"scenario 12 a clean layout should report no overlaps: {rep2['finalState']['overlaps']}")

    # --- Scenario 13: screenshot writes a real PNG of the laid-out canvas ---
    shot = os.path.abspath("probe_selftest_shot.png")
    if os.path.exists(shot):
        os.unlink(shot)
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_setvisible.panel", {"steps": [
        {"action": "screenshot", "file": shot},
    ]})
    require(rep is not None, f"scenario 13 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 13 expected exit 0, got {code}: {rep['steps'][0]['errors']}")
    got = rep["steps"][0].get("screenshot")
    require(got is not None, f"scenario 13 step should report a screenshot: {rep['steps'][0]}")
    require(got["width"] > 0 and got["height"] > 0, f"scenario 13 empty image: {got}")
    require(os.path.exists(shot) and os.path.getsize(shot) > 0,
            "scenario 13 should have written a non-empty PNG")
    with open(shot, "rb") as f:
        require(f.read(8) == b"\x89PNG\r\n\x1a\n", "scenario 13 output is not a PNG")
    os.unlink(shot)

    # --- Scenario 14: settle is a ceiling, not a floor (regression guard on the speed fix) ---
    # 30 no-op steps used to cost ~450 ms each (~13.5 s). They must now finish well inside 3 s of
    # step time. Console output must still be captured -- CtrlrLog is an AsyncUpdater, so a settle
    # that skipped pumping entirely would silently drop it.
    started = time.monotonic()
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_console.panel", {
        "steps": [{"action": "callLuaMethod", "method": "sayHello"} for _ in range(30)]})
    elapsed = time.monotonic() - started
    require(rep is not None, f"scenario 14 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(code == 0, f"scenario 14 expected exit 0, got {code}")
    require(len(rep["steps"]) == 30, "scenario 14 expected 30 steps")
    require(elapsed < 8.0, f"scenario 14 30 no-op steps took {elapsed:.1f}s -- settle has regressed")
    require(all("hello 42" in s["console"] for s in rep["steps"]),
            "scenario 14 every step must still capture its console() output")

    # --- Scenario 15: waitFor polls until truthy, and reports a timeout as an action error ---
    code, rep, raw = run_script(probe, "fixtures/fixture_probe_console.panel", {"steps": [
        {"action": "eval", "lua": "probe_t0 = os.time()"},
        {"action": "waitFor", "lua": "return os.time() > probe_t0", "timeoutMs": 5000, "pollMs": 20},
        {"action": "waitFor", "lua": "return false", "timeoutMs": 100, "pollMs": 20},
    ]})
    require(rep is not None, f"scenario 15 stdout was not valid JSON:\n{raw}")
    assert rep is not None
    require(rep["steps"][1]["returnValue"] is True and rep["steps"][1]["errors"] == [],
            f"scenario 15 waitFor should succeed once the condition goes true: {rep['steps'][1]}")
    require([e["type"] for e in rep["steps"][2]["errors"]] == ["action"],
            f"scenario 15 a waitFor timeout should be an action error: {rep['steps'][2]['errors']}")

    # --- Scenario 16: --stdin runs several scripts in one process, one report per line ---
    code, reports = run_stdin(probe, "fixtures/fixture_probe_console.panel", [
        {"steps": [{"action": "eval", "lua": "probe_marker = 'first'"}]},
        {"steps": [{"action": "eval", "lua": "return probe_marker"}]},
        {"steps": [{"action": "callLuaMethod", "method": "sayHello"}]},
    ])
    require(code == 0, f"scenario 16 --stdin should exit 0 at EOF, got {code}")
    require(len(reports) == 3, f"scenario 16 expected one report per script, got {len(reports)}")
    require(all(r["reportVersion"] == 1 for r in reports), "scenario 16 every report needs a version")
    # Documented contract: scripts in one process SHARE state. Isolation means respawning.
    require(reports[1]["steps"][0]["returnValue"] == "first",
            f"scenario 16 state should persist across scripts: {reports[1]['steps'][0]}")
    require("hello 42" in reports[2]["steps"][0]["console"],
            f"scenario 16 later scripts still capture console: {reports[2]['steps'][0]}")

    # --- Scenarios 17-26: custom components -- paint honesty and real mouse gestures ---
    scenario_17_callback_families(probe)
    scenario_18_paint(probe)
    scenario_19_silent_repaint(probe)
    scenario_20_mouse_click(probe)
    scenario_21_mouse_drag(probe)
    scenario_22_mouse_mods(probe)
    scenario_23_mouse_wheel_and_move(probe)
    scenario_24_virtual_press_clock(probe)
    scenario_25_drag_and_drop_trap(probe)
    scenario_26_disabled_paint_callback(probe)

    # --- Scenarios 27-32: what the panel suites lean on hardest -- MIDI in and out, timers,
    # dialogs, value shapes -- and pixels ---
    scenario_27_midi_out_per_component(probe)
    scenario_28_bulk_output(probe)
    scenario_29_device_input(probe)
    scenario_30_timers_and_dialogs(probe)
    scenario_31_value_shapes(probe)
    scenario_32_png_pixels(probe)

    # --- Scenario 33: a panel saved in edit mode is probed as played ---
    scenario_33_edit_mode(probe)

    # --- Scenario 34: an unreadable panel file is fatal, not an empty clean run ---
    scenario_34_unreadable_panel(probe)

    print("SELFTEST PASS: all 34 scenarios ok")
    sys.exit(0)


if __name__ == "__main__":
    main()
