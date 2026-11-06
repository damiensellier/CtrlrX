#!/usr/bin/env python3
"""Eval 16: two trigger buttons, each sending its own SysEx frame exactly once on every click."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "synth.panel"
FRAMES = {"storePatch": "f0 7d 01 20 f7", "requestDump": "f0 7d 01 30 f7"}
g.require("synth.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
click = lambda n: {"action": "click", "modulator": n}
order = ["storePatch", "requestDump", "storePatch", "storePatch", "requestDump", "requestDump"]
r = g.probe(P, [click(n) for n in order] + [{"action": "setValue", "modulator": "cutoff", "value": 100}])
g.check("panel runs with no errors", lambda: clean(r))
for n in FRAMES:
    g.check(f"{n} is a button",
            lambda n=n: ("Button" in (mod_state(r, n) or {}).get("componentType", ""),
                         (mod_state(r, n) or {}).get("componentType")))
for n, want in FRAMES.items():
    steps = [i for i, s in enumerate(order) if s == n]
    g.check(f"every click on {n} sends exactly {want}, once",
            lambda steps=steps, want=want: (
                all(messages(r, i, collapse=False) == [want] for i in steps),
                {i: messages(r, i, collapse=False) for i in steps}))
g.check("cutoff still sends CC 74",
        lambda: (messages(r, len(order)) == ["b0 4a 64"], messages(r, len(order))))
g.finish()
