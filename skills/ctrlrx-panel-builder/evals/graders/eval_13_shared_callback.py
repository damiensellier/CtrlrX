#!/usr/bin/env python3
"""Eval 13: one shared Lua method serves four knobs and must tell them apart."""
import sys
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import Grader, clean, lua_methods, messages, modulator

g = Grader()
P = "params.panel"
PARAMS = {"cutoff": (0x10, 100), "resonance": (0x11, 33), "attack": (0x12, 5), "release": (0x13, 127)}
g.require("params.panel is still there", lambda: g.exists(P))
g.check("panel_tool lint passes", lambda: g.lint(P))


def one_method():
    root = g.root(P)
    names = {n: (modulator(root, n).get("luaModulatorValueChange") if modulator(root, n) is not None
                 else None) for n in PARAMS}
    shared = set(names.values())
    ok = len(shared) == 1 and None not in shared and shared <= set(lua_methods(root))
    return ok, names


g.check("all four knobs run the same existing Lua method on a value change", one_method)
runs = {n: g.probe(P, [{"action": "setValue", "modulator": n, "value": v}])
        for n, (_, v) in PARAMS.items()}
g.check("moving the knobs raises no errors",
        lambda: (all(clean(r)[0] for r in runs.values()), {n: clean(r)[1] for n, r in runs.items()}))
for n, (pid, v) in PARAMS.items():
    want = f"f0 7d 10 {pid:02x} {v:02x} f7"
    g.check(f"{n} at {v} sends exactly {want}",
            lambda r=runs[n], want=want: (messages(r, 0) == [want], messages(r, 0)))
g.finish()
