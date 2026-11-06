#!/usr/bin/env python3
"""Eval 5: a combo whose entries map to non-index CC values."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "lfo.panel"
g.require("lfo.panel was created", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
order = [(1, 32, "Triangle"), (3, 96, "Square"), (0, 0, "Sine"), (4, 127, "S&H")]
r = g.probe(P, [{"action": "setValue", "modulator": "lfoShape", "value": i} for i, _, _ in order])
g.check("panel runs with no errors", lambda: clean(r))
g.check("lfoShape is a uiCombo",
        lambda: ((mod_state(r, "lfoShape") or {}).get("componentType") == "uiCombo",
                 (mod_state(r, "lfoShape") or {}).get("componentType")))


def mapped():
    got = {name: messages(r, k) for k, (_, _, name) in enumerate(order)}
    ok = all(got[name] == [f"b0 14 {v:02x}"] for _, v, name in order)
    return ok, got


g.check("each entry sends CC 20 on ch 1 with its mapped value (Triangle 32, Square 96, Sine 0, S&H 127)",
        mapped)
g.check("the last entry is displayed as 'S&H' (ampersand survived XML escaping)",
        lambda: ((mod_state(r, "lfoShape") or {}).get("text") == "S&H",
                 (mod_state(r, "lfoShape") or {}).get("text")))
g.finish()
