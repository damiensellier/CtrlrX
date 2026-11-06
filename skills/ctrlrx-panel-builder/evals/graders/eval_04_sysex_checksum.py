#!/usr/bin/env python3
"""Eval 4: SysEx with a Roland checksum, including the sum % 128 == 0 edge."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "synth.panel"
g.require("synth.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
vals = [16, 100, 15]
r = g.probe(P, [{"action": "setValue", "modulator": "detune", "value": v} for v in vals])
g.check("panel runs with no errors", lambda: clean(r))
g.check("detune exists with range 0..127",
        lambda: ((mod_state(r, "detune") or {}).get("max") == 127 and
                 (mod_state(r, "detune") or {}).get("min") == 0, mod_state(r, "detune")))
for i, (v, cs) in enumerate([(16, 0x7F), (100, 0x2B), (15, 0x00)]):
    want = f"f0 41 10 42 12 40 01 30 {v:02x} {cs:02x} f7"
    g.check(f"value {v} sends exactly {want}" + (" (checksum 128 -> 0)" if v == 15 else ""),
            lambda i=i, want=want: (messages(r, i) == [want], messages(r, i)))
g.finish()
