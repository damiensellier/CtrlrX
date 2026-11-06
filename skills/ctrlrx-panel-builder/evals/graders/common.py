"""Shared helpers for the ctrlrx-panel-builder eval graders (stdlib only).

A grader is run as  `python3 eval_NN_<name>.py <run-dir>`.  <run-dir>/outputs/ holds the
agent's working directory after the run; <run-dir>/final_message.txt (optional) holds the
agent's last message.  The grader writes <run-dir>/grading.json in the skill-creator schema
(expectations[].{text, passed, evidence} + summary).

Environment:
  CTRLRX_PANEL_PROBE  path to a built CtrlrPanelProbe (required for behaviour assertions)
  CTRLRX_PANEL_TOOL   panel_tool.py to lint with (default: this skill's scripts/panel_tool.py)
  EVAL_JUDGE_CMD      optional LLM-judge command; reads a prompt on stdin, answers YES/NO on
                      its first line (e.g. "claude -p --model haiku"). Unset -> keyword fallback.
"""
import hashlib
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
EVALS = os.path.dirname(HERE)
FILES = os.path.join(EVALS, "files")
PANEL_TOOL = os.environ.get("CTRLRX_PANEL_TOOL") or os.path.join(
    os.path.dirname(EVALS), "scripts", "panel_tool.py")
PROBE = os.environ.get("CTRLRX_PANEL_PROBE", "")


def _panel_tool_module():
    import importlib.util
    spec = importlib.util.spec_from_file_location("panel_tool", PANEL_TOOL)
    assert spec is not None and spec.loader is not None, PANEL_TOOL
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


class Grader:
    def __init__(self):
        if len(sys.argv) != 2:
            sys.exit(f"usage: {sys.argv[0]} <run-dir>")
        self.run_dir = os.path.abspath(sys.argv[1])
        self.out = os.path.join(self.run_dir, "outputs")
        self.expectations = []
        self._blocked = None

    # ---- recording -------------------------------------------------------------------
    def check(self, text, fn):
        """Record one expectation. fn() returns (passed, evidence); an exception fails it.
        After require() fails, every later check fails with the blocking reason."""
        if self._blocked:
            self.expectations.append({"text": text, "passed": False,
                                      "evidence": f"not checked: {self._blocked}"})
            return False
        try:
            passed, evidence = fn()
        except Exception as exc:  # a grader must never crash on odd output
            passed, evidence = False, f"grader exception: {type(exc).__name__}: {exc}"
        self.expectations.append({"text": text, "passed": bool(passed), "evidence": str(evidence)})
        return bool(passed)

    def require(self, text, fn):
        ok = self.check(text, fn)
        if not ok:
            self._blocked = f"prerequisite failed: {text}"
        return ok

    def finish(self):
        n = len(self.expectations)
        passed = sum(e["passed"] for e in self.expectations)
        result = {"expectations": self.expectations,
                  "summary": {"passed": passed, "failed": n - passed, "total": n,
                              "pass_rate": round(passed / n, 4) if n else 0.0}}
        with open(os.path.join(self.run_dir, "grading.json"), "w") as f:
            json.dump(result, f, indent=2)
        for e in self.expectations:
            print(("PASS " if e["passed"] else "FAIL ") + e["text"])
        print(f"{passed}/{n} passed")

    # ---- files -----------------------------------------------------------------------
    def path(self, rel):
        return os.path.join(self.out, rel)

    def exists(self, rel):
        p = self.path(rel)
        return os.path.isfile(p), (f"found {rel}" if os.path.isfile(p) else f"{rel} not found; "
                                   f"outputs contain: {sorted(os.listdir(self.out))[:30]}")

    def final_message(self):
        p = os.path.join(self.run_dir, "final_message.txt")
        return open(p, encoding="utf-8", errors="replace").read() if os.path.isfile(p) else ""

    def unchanged(self, rel, original):
        a, b = sha256(self.path(rel)), sha256(os.path.join(FILES, original))
        return a == b, ("unchanged" if a == b else "file was modified")

    # ---- panel tool ------------------------------------------------------------------
    def lint(self, rel):
        r = subprocess.run([sys.executable, PANEL_TOOL, "lint", self.path(rel)],
                           capture_output=True, text=True, timeout=60)
        tail = (r.stdout + r.stderr).strip().splitlines()[-3:]
        return r.returncode == 0, f"lint exit {r.returncode}: " + " | ".join(tail)

    def root(self, rel):
        """Parsed panel (any format), or None if it cannot be parsed; checks that use a
        None root fail with the exception as evidence."""
        try:
            return _panel_tool_module().parse(self.path(rel))
        except Exception:
            return None

    # ---- probe -----------------------------------------------------------------------
    def probe(self, panel, steps, **script):
        """Run the probe on a panel (path relative to outputs, or absolute) in a scratch cwd.
        Never raises: a failure comes back as {"error": ...}, which every check then fails on."""
        try:
            return self._probe(panel, steps, script)
        except Exception as exc:
            return {"error": f"{type(exc).__name__}: {exc}"}

    def _probe(self, panel, steps, script):
        if not PROBE or not os.access(PROBE, os.X_OK):
            raise RuntimeError("CTRLRX_PANEL_PROBE is not set to an executable probe")
        script = dict(script, steps=steps)
        if not os.path.isfile(os.path.join(self.out, panel)):
            raise RuntimeError(f"{panel} does not exist")
        with tempfile.TemporaryDirectory() as tmp:
            sp = os.path.join(tmp, "script.json")
            with open(sp, "w") as f:
                json.dump(script, f)
            r = subprocess.run([PROBE, os.path.join(self.out, panel), sp], cwd=tmp,
                               capture_output=True, text=True, timeout=120)
        try:
            return json.loads(r.stdout)
        except json.JSONDecodeError:
            raise RuntimeError(f"probe exit {r.returncode}, unparseable stdout: {r.stdout[:300]!r}")


# ---- report helpers ----------------------------------------------------------------------
def errors(report):
    if "error" in report:
        return f"fatal: {report.get('error')} {report.get('detail', '')}"
    errs = list(report["panel"].get("luaCompileErrors", []))
    for s in report["steps"]:
        errs += [f"step {s['index']}: {e['type']}: {e['message']}" for e in s["errors"]]
    return errs


def clean(report):
    e = errors(report)
    if isinstance(e, str):
        return False, e
    return (not e), ("errorCount 0" if not e else f"errors: {e[:5]}")


def messages(report, *step_indexes, collapse=True):
    """Device MIDI out of the given steps as a list of normalised message strings. The byte
    stream is re-split on status bytes (SysEx kept whole), and runs of byte-identical
    consecutive messages are collapsed (undoes the uiCombo double send, see
    references/verify-with-probe.md)."""
    raw = []
    for i in step_indexes:
        for m in report["steps"][i]["deviceMidiOut"]:
            raw += [int(b, 16) for b in m.split()]
    msgs, cur, in_sysex = [], [], False
    for b in raw:
        if in_sysex:
            cur.append(b)
            if b == 0xF7:
                msgs.append(cur)
                cur, in_sysex = [], False
        elif b >= 0x80:
            if cur:
                msgs.append(cur)
            cur, in_sysex = [b], b == 0xF0
        else:
            cur.append(b)
    if cur:
        msgs.append(cur)
    out = [" ".join(f"{b:02x}" for b in m) for m in msgs]
    if collapse:
        out = [m for k, m in enumerate(out) if k == 0 or m != out[k - 1]]
    return out


def mod_state(report, name):
    for m in report.get("finalState", {}).get("modulators", []):
        if m["name"] == name:
            return m
    return None


def console(report, *step_indexes):
    return [line for i in step_indexes for line in report["steps"][i]["console"]]


def modulator(root, name):
    for m in root.iter("modulator"):
        if m.get("name") == name:
            return m
    return None


def lua_methods(root):
    return {m.get("luaMethodName"): m.get("luaMethodCode", "")
            for m in root.iter("luaMethod")}


def sha256(path):
    with open(path, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def judge(question, text, keyword_fallback):
    """LLM-judged yes/no. Falls back to keyword_fallback(text) -> bool when no judge is set."""
    cmd = os.environ.get("EVAL_JUDGE_CMD")
    if cmd and text.strip():
        prompt = (f"{question}\n\nAnswer YES or NO on the first line, then one sentence why.\n\n"
                  f"--- TEXT TO JUDGE ---\n{text[-6000:]}")
        try:
            r = subprocess.run(shlex.split(cmd), input=prompt, capture_output=True, text=True,
                               timeout=180)
            first = r.stdout.strip().splitlines()[0] if r.stdout.strip() else ""
            if first.upper().startswith(("YES", "NO")):
                return first.upper().startswith("YES"), "judge: " + r.stdout.strip()[:300]
        except (subprocess.TimeoutExpired, OSError):
            pass
    ok = keyword_fallback(text)
    return ok, f"keyword fallback ({'match' if ok else 'no match'}); final message: {text[-300:]!r}"


def copy_outputs(grader, dest):
    shutil.copytree(grader.out, dest, symlinks=True, dirs_exist_ok=True)
    return dest


def num_text(text, value):
    return re.search(rf"(?<![\d.]){value}(\.0+)?(?![\d.])", text or "") is not None
