#!/usr/bin/env python3
"""Eval 12: edit a .bpanelz (binary ValueTree) and deliver a loadable .panel."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
import xml.etree.ElementTree as ET
from common import Grader, clean, lua_methods, messages

g = Grader()
P = "vintage_edited.panel"
g.require("vintage_edited.panel was created", lambda: g.exists(P))
g.check("it is plain XML with a <panel> root",
        lambda: (ET.parse(g.path(P)).getroot().tag == "panel", "parsed as XML"))
g.check("panel_tool lint passes", lambda: g.lint(P))
r = g.probe(P, [{"action": "setValue", "modulator": "resonance", "value": 50},
                {"action": "setValue", "modulator": "cutoff", "value": 50}])
g.check("it loads and runs in the probe with no errors", lambda: clean(r))
g.check("resonance now sends CC 16 (b0 10 32)", lambda: (messages(r, 0) == ["b0 10 32"], messages(r, 0)))
g.check("cutoff still sends CC 74 (b0 4a 32)", lambda: (messages(r, 1) == ["b0 4a 32"], messages(r, 1)))


def same():
    root = g.root(P)
    mods = sorted(m.get("name") for m in root.iter("modulator"))
    n = len(lua_methods(root))
    return mods == ["cutoff", "resonance"] and n == 6, f"modulators={mods}, lua methods={n}"


g.check("nothing else was lost (2 modulators, 6 Lua methods)", same)
g.finish()
