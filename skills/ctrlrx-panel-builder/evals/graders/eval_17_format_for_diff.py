#!/usr/bin/env python3
"""Eval 17: a diff-friendly formatter for a panel must not change what the panel does."""
import os
import re
import subprocess
import sys
import tempfile
sys.dont_write_bytecode = True  # keep __pycache__ out of the skill tree
from common import FILES, Grader, clean, copy_outputs, lua_methods, messages, sha256

g = Grader()
P, TOOL = "editor.panel", os.path.join("tools", "format_panel.py")
ORIG = os.path.join(FILES, P)
TRIGGERS = {"sendPatch": "f0 7d 01 10 f7", "requestDump": "f0 7d 01 11 f7",
            "storePatch": "f0 7d 01 12 f7", "initPatch": "f0 7d 01 13 f7"}
g.require("editor.panel is still there", lambda: g.exists(P))
g.require("tools/format_panel.py exists", lambda: g.exists(TOOL))
g.check("editor.panel was reformatted", lambda: (sha256(g.path(P)) != sha256(ORIG),
                                                 "changed" if sha256(g.path(P)) != sha256(ORIG)
                                                 else "byte-identical to the input"))
g.check("panel_tool lint passes", lambda: g.lint(P))


def one_attribute_per_line():
    text = open(g.path(P), encoding="utf-8", errors="replace").read()
    bad = [ln.strip()[:80] for ln in text.splitlines()
           if not ln.lstrip().startswith("<?xml") and len(re.findall(r'[\w:.-]+="', ln)) > 1]
    return not bad, (f"{len(bad)} lines with several attributes, e.g. {bad[:2]}" if bad
                     else "one attribute per line")


g.check("every attribute is on its own line", one_attribute_per_line)


def idempotent():
    with tempfile.TemporaryDirectory() as tmp:
        work = copy_outputs(g, os.path.join(tmp, "w"))
        before = sha256(os.path.join(work, P))
        r = subprocess.run([sys.executable, TOOL, P], cwd=work, capture_output=True, text=True,
                           timeout=120)
        after = sha256(os.path.join(work, P))
    return r.returncode == 0 and before == after, (
        f"exit {r.returncode}; " + ("unchanged" if before == after else "second run changed the file")
        + (f"; stderr: {r.stderr.strip()[-200:]}" if r.returncode else ""))


g.check("running the formatter again changes nothing", idempotent)


def same_lua():
    new, old = g.root(P), g.root(ORIG)
    a, b = lua_methods(new), lua_methods(old)
    diff = sorted(n for n in b if a.get(n) != b[n])
    return not diff and set(a) == set(b), (f"methods differ: {diff}" if diff else
                                           f"{len(b)} methods identical")


g.check("every Lua method's code is unchanged, byte for byte", same_lua)


def same_attributes():
    new, old = g.root(P), g.root(ORIG)
    a = [(e.tag, sorted(e.attrib.items())) for e in new.iter()]
    b = [(e.tag, sorted(e.attrib.items())) for e in old.iter()]
    if a == b:
        return True, f"{len(b)} elements, same attributes and values"
    first = next((i for i, (x, y) in enumerate(zip(a, b)) if x != y), min(len(a), len(b)))
    return False, f"{len(a)} vs {len(b)} elements; first difference at element {first}"


g.check("every element keeps its attributes and values", same_attributes)
steps = [{"action": "click", "modulator": n} for n in TRIGGERS]
r_new, r_old = g.probe(P, steps), g.probe(ORIG, steps)
g.check("the formatted panel runs with no errors", lambda: clean(r_new))


def same_state():
    keys = ("min", "max", "value", "visible", "componentType")
    a = {m["name"]: tuple(m.get(k) for k in keys) for m in r_new["finalState"]["modulators"]}
    b = {m["name"]: tuple(m.get(k) for k in keys) for m in r_old["finalState"]["modulators"]}
    diff = {n: (b[n], a.get(n)) for n in b if a.get(n) != b[n]}
    return not diff and set(a) == set(b), (f"differs from the original: {diff}" if diff
                                           else f"{len(b)} modulators identical ({', '.join(keys)})")


g.check("the panel loads to the same state as before (range, value, visibility of every control)",
        same_state)
g.check("each trigger button still sends its frame",
        lambda: ([messages(r_new, i, collapse=False) for i in range(len(TRIGGERS))]
                 == [[f] for f in TRIGGERS.values()],
                 [messages(r_new, i, collapse=False) for i in range(len(TRIGGERS))]))
g.finish()
