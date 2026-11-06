#!/usr/bin/env python3
"""Eval 2: diagnose and fix a dead control (callback points at a misspelled method)."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
import re
from common import Grader, clean, judge, messages

g = Grader()
P = "broken_panel.panel"
g.require("broken_panel.panel is still there", lambda: g.exists(P))
g.check("the fixed panel passes panel_tool lint", lambda: g.lint(P))
r = g.probe(P, [{"action": "setValue", "modulator": "cutoff", "value": 90},
                {"action": "setValue", "modulator": "resonance", "value": 33}])
g.check("fixed panel runs with no errors", lambda: clean(r))
g.check("cutoff at 90 now sends CC 74 = 90 exactly once (b0 4a 5a)",
        lambda: (messages(r, 0, collapse=False) == ["b0 4a 5a"], messages(r, 0, collapse=False)))
g.check("resonance still sends CC 71 (b0 47 21)",
        lambda: (messages(r, 1) == ["b0 47 21"], messages(r, 1)))


def keywords(t):
    return "onCutoffChange" in t and re.search(
        r"onCutoffChanged|does ?n[o']t exist|not exist|missing|typo|misspel|not found|dangling|"
        r"no such|undefined|mismatch", t, re.I) is not None


g.check("[judge] the explanation identifies the callback pointing at a non-existent method "
        "('onCutoffChange' vs the defined 'onCutoffChanged')",
        lambda: judge("Does this text explain that the knob was dead because its value-change "
                      "callback referred to a Lua method name ('onCutoffChange') that does not "
                      "exist, the real method being 'onCutoffChanged'?",
                      g.final_message(), keywords))
g.finish()
