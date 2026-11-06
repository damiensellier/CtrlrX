#!/usr/bin/env python3
"""Eval 15: load a patch dump into knobs that send their own parameter changes, without
sending anything back to the synth; hand moves must still send."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, messages, mod_state

g = Grader()
P = "patch_sync.panel"
NAMES = ("osc1Level", "osc2Level", "filterCutoff")
DUMP1, DUMP2 = "F0 7D 02 11 22 33 F7", "F0 7D 02 7F 00 40 F7"
inject = lambda b: {"action": "injectDeviceMidi", "bytes": b}
wait = {"action": "wait", "ms": 300}  # value-change callbacks run asynchronously
g.require("patch_sync.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))
one = g.probe(P, [inject(DUMP1), wait])
two = g.probe(P, [inject(DUMP1), wait, inject(DUMP2), wait,
                  {"action": "setValue", "modulator": "osc2Level", "value": 99}])
g.check("the panel handles the dumps with no errors",
        lambda: (all(clean(r)[0] for r in (one, two)), [clean(r)[1] for r in (one, two)]))
values = lambda r: tuple((mod_state(r, n) or {}).get("value") for n in NAMES)
g.check("dump 11 22 33 sets osc1Level=17, osc2Level=34, filterCutoff=51",
        lambda: (values(one) == (17, 34, 51), values(one)))
# the dump's own bytes are judged separately (device->device thru), so drop them here
sent = lambda r, *i: [m for m in messages(r, *i, collapse=False) if not m.startswith("f0 7d 02")]
g.check("loading a dump sends no parameter change (or anything else) back to the synth",
        lambda: (sent(one, 0, 1) == [] and sent(two, 0, 1, 2, 3) == [], (sent(one, 0, 1), sent(two, 0, 1, 2, 3))))
echo = lambda r, *i: [m for m in messages(r, *i, collapse=False) if m.startswith("f0 7d 02")]
g.check("the dump itself is not echoed back to the synth",
        lambda: (echo(one, 0, 1) == [], echo(one, 0, 1)))
g.check("a second dump 7F 00 40 updates the knobs to 127, 0, 64 before osc2Level is moved",
        lambda: ((mod_state(two, "osc1Level") or {}).get("value") == 127
                 and (mod_state(two, "filterCutoff") or {}).get("value") == 64,
                 values(two)))
g.check("moving osc2Level by hand to 99 still sends exactly f0 7d 01 02 63 f7",
        lambda: (messages(two, 4, collapse=False) == ["f0 7d 01 02 63 f7"], messages(two, 4, collapse=False)))
g.finish()
