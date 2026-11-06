#!/usr/bin/env python3
"""Eval 14: a knob for a second device on its own MIDI channel, both directions."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "synth.panel"
g.require("synth.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
sv = lambda n, v: {"action": "setValue", "modulator": n, "value": v}
inject = lambda b: {"action": "injectDeviceMidi", "bytes": b}
value = lambda r, n: (mod_state(r, n) or {}).get("value")
out = g.probe(P, [sv("fxMix", 100), sv("cutoff", 64)])
ch5 = g.probe(P, [inject("B4 5B 33")])
ch1 = g.probe(P, [inject("B4 5B 33"), inject("B0 5B 22")])
g.check("the panel runs with no errors",
        lambda: (all(clean(r)[0] for r in (out, ch5, ch1)), [clean(r)[1] for r in (out, ch5, ch1)]))


def fx_range():
    m = mod_state(out, "fxMix")
    return m is not None and (m.get("min"), m.get("max")) == (0, 127), m and {k: m.get(k) for k in ("min", "max")}


g.check("fxMix exists with range 0..127", fx_range)
g.check("fxMix at 100 sends exactly CC 91 = 100 on channel 5 (b4 5b 64)",
        lambda: (messages(out, 0) == ["b4 5b 64"], messages(out, 0)))
g.check("cutoff still sends CC 74 on channel 1 (b0 4a 40)",
        lambda: (messages(out, 1) == ["b0 4a 40"], messages(out, 1)))
g.check("CC 91 = 51 arriving on channel 5 moves fxMix to 51",
        lambda: (value(ch5, "fxMix") == 51, value(ch5, "fxMix")))
g.check("CC 91 arriving on channel 1 leaves fxMix alone",
        lambda: (value(ch1, "fxMix") == 51, value(ch1, "fxMix")))
g.finish()
