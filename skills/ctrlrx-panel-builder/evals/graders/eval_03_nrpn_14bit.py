#!/usr/bin/env python3
"""Eval 3: a 14-bit NRPN knob with an exact byte order."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state, modulator

g = Grader()
P = "synth.panel"
N = "filterEnvAmount"
g.require("synth.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
r = g.probe(P, [{"action": "setValue", "modulator": N, "value": 1000},
                {"action": "setValue", "modulator": N, "value": 16383},
                {"action": "setValue", "modulator": "cutoff", "value": 64}])
g.check("panel runs with no errors", lambda: clean(r))


def rng():
    s = mod_state(r, N) or {}
    comp = modulator(g.root(P), N).find("component")
    smax = comp.get("uiSliderMax") if comp.get("uiType", "").startswith("ui") else None
    ok = s.get("min") == 0 and s.get("max") == 16383 and smax in (None, "16383", "16383.0")
    return ok, f"modulator {s.get('min')}..{s.get('max')}, uiSliderMax={smax}"


g.check("filterEnvAmount exists with a 0..16383 range (modulator and slider)", rng)
g.check("value 1000 sends NRPN 291 on ch 3 in order 99/98/6/38 (b2 63 02, b2 62 23, b2 06 07, b2 26 68)",
        lambda: (messages(r, 0) == ["b2 63 02", "b2 62 23", "b2 06 07", "b2 26 68"], messages(r, 0)))
g.check("value 16383 sends b2 63 02, b2 62 23, b2 06 7f, b2 26 7f",
        lambda: (messages(r, 1) == ["b2 63 02", "b2 62 23", "b2 06 7f", "b2 26 7f"], messages(r, 1)))
g.check("existing cutoff knob still sends b0 4a 40",
        lambda: (messages(r, 2) == ["b0 4a 40"], messages(r, 2)))
g.finish()
