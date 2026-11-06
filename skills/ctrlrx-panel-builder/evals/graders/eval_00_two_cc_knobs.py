#!/usr/bin/env python3
"""Eval 0: a new panel with two CC knobs."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "filter_section.panel"
g.require("filter_section.panel was created", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
r = g.probe(P, [{"action": "setValue", "modulator": "cutoff", "value": 100},
                {"action": "setValue", "modulator": "resonance", "value": 33}])
g.check("panel loads and runs in the probe with no errors", lambda: clean(r))


def knobs():
    states = {n: mod_state(r, n) for n in ("cutoff", "resonance")}
    ok = all(s and s["componentType"] == "uiSlider" and s["min"] == 0 and s["max"] == 127
             for s in states.values())
    return ok, {n: s and {k: s[k] for k in ("componentType", "min", "max")} for n, s in states.items()}


g.check("modulators 'cutoff' and 'resonance' are uiSliders with range 0..127", knobs)
g.check("cutoff at 100 sends exactly CC 74 = 100 on channel 1 (b0 4a 64)",
        lambda: (messages(r, 0) == ["b0 4a 64"], messages(r, 0)))
g.check("resonance at 33 sends exactly CC 71 = 33 on channel 1 (b0 47 21)",
        lambda: (messages(r, 1) == ["b0 47 21"], messages(r, 1)))
g.finish()
