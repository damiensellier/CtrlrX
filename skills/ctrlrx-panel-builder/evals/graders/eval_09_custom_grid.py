#!/usr/bin/env python3
"""Eval 9: a uiCustomComponent step grid with paint and click handlers."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, lua_methods, mod_state, modulator

g = Grader()
P = "synth.panel"
g.require("synth.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
root = g.root(P)


def shape():
    m = modulator(root, "stepGrid")
    c = m.find("component")
    w, h = c.get("componentRectangle").split()[2:4]
    return (c.get("uiType") == "uiCustomComponent" and (float(w), float(h)) == (400, 50),
            f"uiType={c.get('uiType')} size={w}x{h}")


def callbacks():
    c = modulator(root, "stepGrid").find("component")
    methods = lua_methods(root)
    got = {k: c.get(k) for k in ("uiCustomPaintCallback", "uiCustomMouseDownCallback",
                                 "uiCustomMouseUpCallback")}
    ok = got["uiCustomPaintCallback"] in methods and (
        got["uiCustomMouseDownCallback"] in methods or got["uiCustomMouseUpCallback"] in methods)
    return ok, got


g.check("stepGrid is a 400x50 uiCustomComponent", shape)
g.check("paint and mouse callbacks name existing Lua methods", callbacks)
state = {"action": "eval", "lua": "return stepState"}
r = g.probe(P, [state, {"action": "screenshot", "file": "grid.png"},
                {"action": "click", "modulator": "stepGrid"}, state,
                {"action": "click", "modulator": "stepGrid"}, state])
g.check("load, paint (screenshot), and clicks raise no errors", lambda: clean(r))
sv = lambda i: r["steps"][i].get("returnValue")
g.check("stepState starts as 8 false entries", lambda: (sv(0) == [False] * 8, sv(0)))
g.check("screenshot rendered the canvas",
        lambda: ("screenshot" in r["steps"][1], r["steps"][1].get("screenshot")))


def toggled():
    s = sv(3)
    ok = isinstance(s, list) and len(s) == 8 and s.count(True) == 1 and (s[3] or s[4])
    return ok, f"after centre click: {s} (centre x=200 is the cell 4/5 boundary)"


g.check("a centre click turns on exactly one cell (4 or 5)", toggled)
g.check("a second click turns it off again", lambda: (sv(5) == [False] * 8, sv(5)))
g.finish()
