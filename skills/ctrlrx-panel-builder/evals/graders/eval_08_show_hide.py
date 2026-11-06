#!/usr/bin/env python3
"""Eval 8: show only the knobs that belong to the selected effect type."""
import os
import re
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, mod_state

g = Grader()
P = "fx.panel"
GROUPS = {0: ("reverbTime", "reverbSize"), 1: ("delayTime", "delayFeedback"),
          2: ("chorusRate", "chorusDepth")}
ALL = [n for v in GROUPS.values() for n in v]
g.require("fx.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))


def run_mode_copy():
    """Probe a copy saved in run mode, so the verdict doesn't depend on the probe's edit-mode
    default. The input is saved in edit mode; probes that load it as saved only dim a control
    hidden via its componentVisibility property. Current probes run in run mode anyway, so the
    copy only matters for older probe builds."""
    with open(g.path(P), encoding="utf-8") as f:
        xml = re.sub(r'uiPanelEditMode="1"', 'uiPanelEditMode="0"', f.read(), count=1)
    path = os.path.join(g.run_dir, "fx.run-mode.panel")
    with open(path, "w", encoding="utf-8") as f:
        f.write(xml)
    return path


RUN = run_mode_copy() if os.path.isfile(g.path(P)) else P
sel = lambda v: {"action": "setValue", "modulator": "fxType", "value": v}
runs = {1: g.probe(RUN, [sel(1)]), 2: g.probe(RUN, [sel(2)]), 0: g.probe(RUN, [sel(1), sel(0)])}
g.check("selecting effect types raises no errors",
        lambda: (all(clean(r)[0] for r in runs.values()), [clean(r)[1] for r in runs.values()]))
for v, label in ((1, "Delay"), (2, "Chorus"), (0, "Reverb (after switching away and back)")):
    def vis(v=v):
        got = {n: (mod_state(runs[v], n) or {}).get("effectiveVisible") for n in ALL}
        return all(got[n] is (n in GROUPS[v]) for n in ALL), got
    g.check(f"{label}: exactly its two knobs are effectively visible", vis)
g.finish()
