#!/usr/bin/env python3
"""Eval 1: value-change callback that updates a label, in a copy of the panel."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, console, lua_methods, messages, mod_state, modulator, num_text

g = Grader()
P = "cutoff_with_label.panel"
g.check("original synth.panel is unchanged", lambda: g.unchanged("synth.panel", "synth.panel"))
g.require("cutoff_with_label.panel was created", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
root = g.root(P)


def label():
    m = modulator(root, "cutoffLabel")
    t = m is not None and m.find("component").get("uiType")
    return t == "uiLabel", f"cutoffLabel uiType={t}"


def wired():
    name = modulator(root, "cutoff").get("luaModulatorValueChange")
    return name in lua_methods(root), f"luaModulatorValueChange={name!r}"


g.check("a uiLabel modulator named 'cutoffLabel' exists", label)
g.check("cutoff's luaModulatorValueChange names an existing Lua method", wired)
r = g.probe(P, [{"action": "setValue", "modulator": "cutoff", "value": 77}])
g.check("moving cutoff raises no errors in the probe", lambda: clean(r))
g.check("after moving cutoff to 77 the label shows 77",
        lambda: (num_text((mod_state(r, "cutoffLabel") or {}).get("text"), 77),
                 f"label text={(mod_state(r, 'cutoffLabel') or {}).get('text')!r}"))
g.check("the value is printed to the console",
        lambda: (any(num_text(l, 77) for l in console(r, 0)), console(r, 0)))
g.check("cutoff still sends its CC 74 (b0 4a 4d)",
        lambda: ("b0 4a 4d" in messages(r, 0), messages(r, 0)))
g.finish()
