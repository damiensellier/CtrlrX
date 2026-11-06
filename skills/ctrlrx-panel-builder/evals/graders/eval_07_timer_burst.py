#!/usr/bin/env python3
"""Eval 7: a button sends 16 CCs spaced out by a timer; none dropped; re-triggerable."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "synth.panel"
WANT = [f"b0 {cc:02x} {(cc - 20) * 8:02x}" for cc in range(20, 36)]
g.require("synth.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
click = {"action": "click", "modulator": "sendAll", "settleMs": 40, "quietMs": 10}
r = g.probe(P, [click, {"action": "wait", "ms": 1500}, click, {"action": "wait", "ms": 1500}])
burst = lambda *s: [m for m in messages(r, *s, collapse=False)
                    if m.startswith("b0 ") and 20 <= int(m.split()[1], 16) <= 35]
g.check("panel runs with no errors", lambda: clean(r))
g.check("sendAll is a button",
        lambda: ("Button" in (mod_state(r, "sendAll") or {}).get("componentType", ""),
                 (mod_state(r, "sendAll") or {}).get("componentType")))
g.check("the click does not send all 16 at once (spaced by a timer)",
        lambda: (len(burst(0)) < 16, f"{len(burst(0))} of 16 within ~40 ms of the click"))
g.check("first click: all 16 CCs (20..35, value (n-20)*8) arrive in order, none dropped",
        lambda: (burst(0, 1) == WANT, burst(0, 1)))
g.check("second click: the full sequence again, nothing left over from the first",
        lambda: (burst(2, 3) == WANT, burst(2, 3)))
g.finish()
