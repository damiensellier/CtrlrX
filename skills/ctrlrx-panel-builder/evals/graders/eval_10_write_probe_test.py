#!/usr/bin/env python3
"""Eval 10: the agent's check script passes on the good panel and fails on mutants."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
import os
import subprocess
import tempfile
from common import FILES, PROBE, Grader

g = Grader()
S = "check_octave.sh"
g.check("octave.panel is unchanged", lambda: g.unchanged("octave.panel", "octave.panel"))
g.require("check_octave.sh was created", lambda: g.exists(S))
good = open(os.path.join(FILES, "octave.panel"), encoding="utf-8").read()
MUTANTS = {
    "CC number 52 instead of 51": good.replace('midiMessageCtrlrNumber="51"',
                                               'midiMessageCtrlrNumber="52"'),
    "value expression *31 (sends 62)": good.replace('modulatorValue * 32', 'modulatorValue * 31'),
    "label off by one ('+3 oct')": good.replace('string.format(&quot;+%d oct&quot;, value)',
                                                'string.format(&quot;+%d oct&quot;, value + 1)'),
}
assert all(m != good for m in MUTANTS.values()), "a mutant did not apply"


def run(panel_text):
    with tempfile.TemporaryDirectory() as tmp:
        work = os.path.join(tmp, "work")
        import shutil
        shutil.copytree(g.out, work, symlinks=True)
        panel = os.path.join(tmp, "under_test.panel")
        with open(panel, "w", encoding="utf-8") as f:
            f.write(panel_text)
        env = dict(os.environ, CTRLRX_PANEL_PROBE=PROBE)
        r = subprocess.run(["bash", S, panel], cwd=work, env=env, capture_output=True,
                           text=True, timeout=120)
        return r.returncode, (r.stdout + r.stderr).strip()[-200:]


g.check("the script exits 0 on the correct panel",
        lambda: (lambda rc_out: (rc_out[0] == 0, f"exit {rc_out[0]}: {rc_out[1]!r}"))(run(good)))
for name, text in MUTANTS.items():
    g.check(f"the script fails on a mutant: {name}",
            lambda text=text: (lambda rc_out: (rc_out[0] != 0, f"exit {rc_out[0]}: {rc_out[1]!r}"))(run(text)))
g.finish()
