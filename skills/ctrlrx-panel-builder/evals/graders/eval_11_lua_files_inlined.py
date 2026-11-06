#!/usr/bin/env python3
"""Eval 11: Lua kept in .lua files and inlined into the panel by the agent's script."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
import os
import shutil
import subprocess
import sys
import tempfile
from common import Grader, clean, console, lua_methods, num_text

g = Grader()
PROJ = "lua_project"
PANEL = f"{PROJ}/panel/synth.panel"
g.require("lua/onCutoff.lua and tools/inline_lua.py exist",
          lambda: (all(os.path.isfile(g.path(f"{PROJ}/{f}"))
                       for f in ("lua/onCutoff.lua", "tools/inline_lua.py")),
                   sorted(os.listdir(g.path(PROJ)))))
g.check("panel_tool lint passes", lambda: g.lint(PANEL))
norm = lambda s: "\n".join(l.rstrip() for l in s.replace("\r\n", "\n").strip().splitlines())


def inlined():
    code = lua_methods(g.root(PANEL)).get("onCutoff")
    src = open(g.path(f"{PROJ}/lua/onCutoff.lua"), encoding="utf-8").read()
    return code is not None and norm(code) == norm(src), \
        "identical" if code and norm(code) == norm(src) else f"method code={str(code)[:120]!r}"


g.check("the onCutoff method in the panel is exactly lua/onCutoff.lua", inlined)
r = g.probe(PANEL, [{"action": "setValue", "modulator": "cutoff", "value": 42}])
g.check("moving cutoff prints 'cutoff 42' with no errors",
        lambda: (clean(r)[0] and any("cutoff" in l and num_text(l, 42) for l in console(r, 0)),
                 f"{clean(r)[1]}; console={console(r, 0)}"))

# Re-run the agent's script after editing a file and adding one: replace, don't duplicate.
tmp = tempfile.mkdtemp()
rr = names = r2 = None
try:
    proj = os.path.join(tmp, PROJ)
    shutil.copytree(g.path(PROJ), proj, symlinks=True)
    with open(os.path.join(proj, "lua", "onCutoff.lua"), "w") as f:
        f.write('onCutoff = function(mod, value, source)\n\tconsole("CUT " .. value)\nend\n')
    with open(os.path.join(proj, "lua", "helper.lua"), "w") as f:
        f.write("helper = function()\n\treturn 7\nend\n")
    rr = subprocess.run([sys.executable, "tools/inline_lua.py"], cwd=proj, capture_output=True,
                        text=True, timeout=60)
    g.out = tmp
    root2 = g.root(PANEL)
    names = [m.get("luaMethodName") for m in root2.iter("luaMethod")] if root2 is not None else []
    r2 = g.probe(PANEL, [{"action": "setValue", "modulator": "cutoff", "value": 42},
                         {"action": "callLuaMethod", "method": "helper"}])
except Exception as exc:  # recorded by the checks below
    print(f"re-run setup failed: {exc}")
g.check("re-running tools/inline_lua.py after edits succeeds",
        lambda: (rr.returncode == 0, f"exit {rr.returncode}: {(rr.stdout + rr.stderr)[-200:]!r}"))
g.check("an edited file replaces its method (one onCutoff, prints 'CUT 42')",
        lambda: (names.count("onCutoff") == 1 and clean(r2)[0] and any("CUT" in l for l in console(r2, 0)),
                 f"onCutoff x{names.count('onCutoff')}; {clean(r2)[1]}; console={console(r2, 0)}"))
g.check("a new lua/helper.lua becomes a callable method returning 7",
        lambda: (r2["steps"][1].get("returnValue") == 7, r2["steps"][1]))
shutil.rmtree(tmp, ignore_errors=True)
g.finish()
