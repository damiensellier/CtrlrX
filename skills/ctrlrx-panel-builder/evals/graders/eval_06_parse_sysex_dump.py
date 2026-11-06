#!/usr/bin/env python3
"""Eval 6: an incoming SysEx dump updates three knobs; foreign SysEx is ignored."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, mod_state

g = Grader()
P = "dump_receiver.panel"
NAMES = ("osc1Level", "osc2Level", "filterCutoff")
DUMP1, DUMP2 = "F0 00 20 33 01 10 11 22 33 F7", "F0 00 20 33 01 10 7F 00 40 F7"
FOREIGN = "F0 00 20 34 01 10 01 02 03 F7"
g.require("dump_receiver.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))


def values(r):
    return tuple((mod_state(r, n) or {}).get("value") for n in NAMES)


inject = lambda b: {"action": "injectDeviceMidi", "bytes": b}
a = g.probe(P, [inject(DUMP1)])
b = g.probe(P, [inject(DUMP1), inject(FOREIGN)])
c = g.probe(P, [inject(DUMP1), inject(DUMP2)])
g.check("panel handles the dumps with no errors",
        lambda: (all(clean(x)[0] for x in (a, b, c)), [clean(x)[1] for x in (a, b, c)]))
g.check("dump 11 22 33 sets osc1Level=17, osc2Level=34, filterCutoff=51",
        lambda: (values(a) == (17, 34, 51), values(a)))
g.check("a SysEx with a different header leaves the knobs alone",
        lambda: (values(b) == (17, 34, 51), values(b)))
g.check("a second dump 7F 00 40 updates the knobs to 127, 0, 64",
        lambda: (values(c) == (127, 0, 64), values(c)))
g.finish()
