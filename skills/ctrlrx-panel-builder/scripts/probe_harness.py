#!/usr/bin/env python3
"""Run CtrlrPanelProbe and read its reports. Standard library only (python3 >= 3.8).

Command line:

    python3 probe_harness.py where                         # print the probe binary it would use
    python3 probe_harness.py run PANEL SCRIPT.json         # run a script, print a digest
    python3 probe_harness.py run PANEL --steps '[{"action": "setValue", "modulator": "cutoff", "value": 100}]'
    python3 probe_harness.py batch PANEL SCRIPTS.jsonl     # one --stdin process, one digest per line

    Options for run/batch: --report (print the full JSON report(s) instead of the digest),
    --settle MS (script-wide settleMs when the script sets none), --edit-mode-as-saved,
    --timeout S (default 120), --ctrlrx DIR (a CtrlrX checkout to search for the probe),
    --allow-empty (accept a panel that loads with 0 modulators).

    Exit codes: 0 = no errors; 1 = summary.errorCount > 0 (or a disabled Lua method in batch);
    2 = fatal (probe not found, panel did not load, bad script, wrong reportVersion, timeout,
    or 0 modulators: older probe builds load some malformed XML as an empty panel; run
    panel_tool.py lint first).

The digest lists, per step: the action, deviceMidiOut / hostMidiOut, errors, returnValue,
dialogs; then warnings and summary.errorCount. It is what you assert on, minus finalState
(use --report and jq for that).

Locating the probe: $CTRLRX_PANEL_PROBE if set; otherwise the newest
build*/Tests/[<Config>/]CtrlrPanelProbe[.exe] under --ctrlrx DIR, or under the current directory,
any of its parents, or a "ctrlrx" directory inside any of them. Without a CtrlrX clone, download
the CtrlrPanelProbe-<OS> artifact of a CI run of https://github.com/damiensellier/CtrlrX and set
CTRLRX_PANEL_PROBE.

As a module (put this directory on sys.path):

    from probe_harness import run_script, set_value, click, call, evaluate, sent, assert_clean
    report = run_script("synth.panel", [set_value("cutoff", 100), click("send")])
    assert sent(report["steps"][0]) == [bytes.fromhex("b04a64")]
"""
from __future__ import annotations

import argparse
import glob
import json
import os
import subprocess
import sys
import tempfile

REPORT_VERSION = 1           # the report schema this file understands
DEFAULT_TIMEOUT_S = 120      # a probe run is seconds; minutes means a hung panel (e.g. a combo
                             # re-set from its own callback). Never wait for ever.


class ProbeError(RuntimeError):
    """The probe could not produce a usable report (not found, fatal load error, timeout...)."""


# --- locating the binary -------------------------------------------------------------------
def find_probe(ctrlrx: str | None = None) -> str:
    env = os.environ.get("CTRLRX_PANEL_PROBE")
    if env:
        if not os.path.isfile(env):
            raise ProbeError(f"CTRLRX_PANEL_PROBE={env} does not exist")
        return env
    roots = []
    if ctrlrx:
        roots.append(os.path.abspath(ctrlrx))
    here = os.getcwd()
    while True:
        roots += [here, os.path.join(here, "ctrlrx")]
        parent = os.path.dirname(here)
        if parent == here:
            break
        here = parent
    found = []
    for root in roots:
        for name in ("CtrlrPanelProbe", "CtrlrPanelProbe.exe"):
            found += glob.glob(os.path.join(root, "build*", "Tests", name))
            # multi-config generators (Visual Studio, Xcode): build/Tests/<Config>/
            found += glob.glob(os.path.join(root, "build*", "Tests", "*", name))
    found = [f for f in found if os.access(f, os.X_OK)]
    if not found:
        raise ProbeError(
            "CtrlrPanelProbe not found: set CTRLRX_PANEL_PROBE=/path/to/CtrlrPanelProbe (a CI "
            "artifact of github.com/damiensellier/CtrlrX), or pass --ctrlrx <CtrlrX clone> after "
            "building it there (cmake --build build --target CtrlrPanelProbe)")
    return max(found, key=os.path.getmtime)


# --- reports -------------------------------------------------------------------------------
def parse_report(text: str, allow_empty: bool = False) -> dict:
    """One report, refusing anything that is not the schema we assert on."""
    try:
        report = json.loads(text)
    except json.JSONDecodeError as e:
        raise ProbeError(f"probe output is not JSON ({e}): {text[:200]!r}")
    if report.get("reportVersion") != REPORT_VERSION:
        raise ProbeError(f"report version {report.get('reportVersion')!r}, this harness reads "
                         f"{REPORT_VERSION}; update probe_harness.py or use a matching probe")
    if "summary" not in report:  # fatal load/script error: only error/detail are present
        raise ProbeError(f"probe returned no report: {report.get('error')}: "
                         f"{report.get('detail')}")
    if not allow_empty and not report["finalState"]["modulators"]:
        raise ProbeError(f"panel loaded with 0 modulators (as {report['panel'].get('name')!r}): "
                         "the file is probably malformed; run panel_tool.py lint")
    return report


def error_digest(report: dict) -> list:
    """Per-step errors (plus load-time compile errors), for an assertion message."""
    out = [{"load": e} for e in report["panel"].get("luaCompileErrors", [])]
    out += [{"step": s["index"], "action": s["action"].get("action"), "errors": s["errors"]}
            for s in report["steps"] if s["errors"]]
    return out


def assert_clean(report: dict) -> dict:
    """summary.errorCount is the single truth: compile, runtime and action errors in one number."""
    if report["summary"]["errorCount"] != 0:
        raise AssertionError(f"errorCount {report['summary']['errorCount']}: "
                             f"{json.dumps(error_digest(report))}")
    return report


# --- running -------------------------------------------------------------------------------
def _script(steps, settle_ms):
    if isinstance(steps, dict):
        script = dict(steps)
    else:
        script = {"steps": list(steps)}
    if settle_ms is not None:
        script.setdefault("settleMs", settle_ms)
    return script


def run_script(panel: str, steps, *, settle_ms: int | None = None, check: bool = True,
               flags: tuple = (), probe: str | None = None,
               timeout: float = DEFAULT_TIMEOUT_S, allow_empty: bool = False) -> dict:
    """Spawn one probe process for one script (a list of steps, or a whole script dict)."""
    probe = probe or find_probe()
    fd, path = tempfile.mkstemp(suffix=".json", prefix="probe-script-")
    try:
        with os.fdopen(fd, "w") as f:
            json.dump(_script(steps, settle_ms), f)
        try:
            proc = subprocess.run([probe, *flags, panel, path], capture_output=True,
                                  text=True, timeout=timeout)
        except subprocess.TimeoutExpired:
            raise ProbeError(f"probe timed out after {timeout:g} s (a hung panel?)")
    finally:
        os.unlink(path)
    report = parse_report(proc.stdout, allow_empty)
    return assert_clean(report) if check else report


class PersistentProbe:
    """One long-lived `--stdin` probe: startup (~1.5 s) is paid once. Every script shares the
    panel (values, Lua globals, a method disabled by an error stays disabled); there is no
    reload. Seed everything you assert on, or use run_script for a pristine panel."""

    def __init__(self, panel: str, *, flags: tuple = (), probe: str | None = None,
                 timeout: float = DEFAULT_TIMEOUT_S, allow_empty: bool = False):
        self.timeout = timeout
        self.allow_empty = allow_empty
        self._err = tempfile.TemporaryFile(mode="w+")  # kept: a mid-session crash needs it
        self._proc = subprocess.Popen([probe or find_probe(), "--stdin", *flags, panel],
                                      stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                      stderr=self._err, text=True, bufsize=1)

    def run(self, steps, *, settle_ms: int | None = None, check: bool = True) -> dict:
        assert self._proc.stdin is not None and self._proc.stdout is not None
        self._proc.stdin.write(json.dumps(_script(steps, settle_ms)) + "\n")
        self._proc.stdin.flush()
        line = self._proc.stdout.readline()
        if not line:
            self._err.seek(0)
            raise ProbeError(f"probe exited mid-session (code {self._proc.poll()}); stderr "
                             f"tail: {self._err.read()[-1000:]!r}")
        report = parse_report(line, self.allow_empty)
        return assert_clean(report) if check else report

    def close(self) -> None:
        if self._proc.stdin is not None:
            self._proc.stdin.close()
        try:
            self._proc.wait(timeout=30)
        except subprocess.TimeoutExpired:
            self._proc.kill()  # this exact process only, never a broad pkill
        self._err.close()

    def __enter__(self):
        return self

    def __exit__(self, *_exc):
        self.close()


# --- step builders -------------------------------------------------------------------------
def set_value(modulator: str, value: float) -> dict:
    return {"action": "setValue", "modulator": modulator, "value": value}


def click(modulator: str) -> dict:
    return {"action": "click", "modulator": modulator}


def call(method: str, *args) -> dict:
    step: dict = {"action": "callLuaMethod", "method": method}
    if args:
        step["args"] = list(args)
    return step


def evaluate(lua: str) -> dict:
    return {"action": "eval", "lua": lua}


def wait_for(lua: str, timeout_ms: int = 2000) -> dict:
    return {"action": "waitFor", "lua": lua, "timeoutMs": timeout_ms}


def inject(hex_bytes: str, host: bool = False) -> dict:
    return {"action": "injectHostMidi" if host else "injectDeviceMidi", "bytes": hex_bytes}


# --- report accessors ----------------------------------------------------------------------
def mods(report: dict) -> dict:
    return {m["name"]: m for m in report["finalState"]["modulators"]}


def values(report: dict) -> dict:
    return {n: m["value"] for n, m in mods(report).items()}


def visible(report: dict) -> dict:
    """effectiveVisible (the component and every ancestor), not the componentVisibility
    property. None when the modulator has no live component."""
    return {n: m.get("effectiveVisible") for n, m in mods(report).items()}


def sent(step: dict, host: bool = False) -> list:
    """The MIDI one step put on the device (or host) wire, one bytes object per message."""
    key = "hostMidiOut" if host else "deviceMidiOut"
    return [bytes.fromhex(m.replace(" ", "")) for m in step[key]]


def hexs(b: bytes) -> str:
    return " ".join(f"{x:02x}" for x in b)


def unterminated_sysex(messages) -> list:
    """SysEx messages that start with F0 but do not end with F7 (real MIDI drivers drop them)."""
    return [m for m in messages if m[:1] == b"\xf0" and m[-1:] != b"\xf7"]


def collapse_repeats(messages) -> list:
    out = []
    for m in messages:
        if not out or out[-1] != m:
            out.append(m)
    return out


def fold_double_send(messages) -> list:
    """Undo the uiCombo double send under the probe's setValue, in both arrangements it comes
    in: interleaved (a a b b c c) or sequential (a b c a b c). Two *different* writes survive."""
    msgs = list(messages)
    collapsed = collapse_repeats(msgs)
    if len(collapsed) * 2 == len(msgs):
        return collapsed
    half = len(msgs) // 2
    if half and msgs[:half] == msgs[half:]:
        return msgs[:half]
    return msgs


def decode_nrpn(messages, number_ccs=(99, 98), value_ccs=(6, 38)) -> list:
    """(parameter, 14-bit value) for each complete NRPN write in a CC stream; other traffic is
    ignored. Raises ValueError on a data CC with no parameter selected (a merged burst)."""
    writes, msb, lsb, vmsb = [], None, None, None
    for m in messages:
        if len(m) != 3 or m[0] & 0xF0 != 0xB0:
            continue
        cc, d = m[1], m[2]
        if cc == number_ccs[0]:
            msb, lsb, vmsb = d, None, None
        elif cc == number_ccs[1]:
            lsb, vmsb = d, None
        elif cc == value_ccs[0]:
            if msb is None or lsb is None:
                raise ValueError(f"CC{cc} {d} with no parameter selected")
            vmsb = d
        elif cc == value_ccs[1]:
            if vmsb is None or msb is None or lsb is None:
                raise ValueError(f"CC{cc} {d} with no preceding CC{value_ccs[0]}")
            writes.append(((msb << 7) | lsb, (vmsb << 7) | d))
            vmsb = None
    return writes


# --- command line --------------------------------------------------------------------------
def digest(report: dict) -> str:
    lines = []
    for e in report["panel"].get("luaCompileErrors", []):
        lines.append(f"load: lua compile error: {e}")
    for d in report["panel"].get("dialogs", []):
        lines.append(f"load: dialog {d.get('title')!r} {d.get('buttons')}")
    for s in report["steps"]:
        a = dict(s["action"])
        name = a.pop("action", "?")
        lines.append(f"step {s['index']} {name} {json.dumps(a)}")
        for key in ("deviceMidiOut", "hostMidiOut"):
            if s.get(key):
                lines.append(f"  {key}: {', '.join(s[key])}")
        if "returnValue" in s:
            lines.append(f"  returnValue: {json.dumps(s['returnValue'])[:300]}")
        for e in s.get("errors", []):
            lines.append(f"  ERROR ({e.get('type')}): {e.get('message')}")
        for d in s.get("dialogs", []):
            lines.append(f"  dialog {d.get('title')!r} {d.get('buttons')} (answered as Cancel)")
        for c in s.get("console", [])[:20]:
            lines.append(f"  console: {c}")
    for w in report.get("warnings", []):
        lines.append(f"warning ({w.get('type')}) {w.get('modulator')}: {w.get('message')}")
    summ = report["summary"]
    lines.append(f"errorCount {summ['errorCount']}  warningCount {summ.get('warningCount', 0)}"
                 f"  disabledMethods {summ.get('disabledMethods', [])}")
    return "\n".join(lines)


def _load_script(args) -> dict:
    if args.steps is not None:
        return {"steps": json.loads(args.steps)}
    if not args.script:
        raise ProbeError("give a SCRIPT.json or --steps '[...]'")
    with open(args.script, encoding="utf-8") as f:
        return json.load(f)


def main(argv=None) -> int:
    doc = __doc__ or ""
    ap = argparse.ArgumentParser(description=doc.split("\n\n")[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter,
                                 epilog=doc.split("\n\n", 1)[1])
    sub = ap.add_subparsers(dest="cmd", required=True)
    w = sub.add_parser("where", help="print the probe binary that would be used")
    w.add_argument("--ctrlrx", help="a CtrlrX checkout to search")
    for name in ("run", "batch"):
        p = sub.add_parser(name, help="run one script" if name == "run" else
                           "run a .jsonl file (one script per line) in one --stdin process")
        p.add_argument("panel")
        p.add_argument("script", nargs="?",
                       help="JSON script" if name == "run" else "JSONL file, one script per line")
        if name == "run":
            p.add_argument("--steps", help="inline JSON array of steps instead of SCRIPT.json")
        p.add_argument("--report", action="store_true", help="print the full JSON report(s)")
        p.add_argument("--settle", type=int, help="settleMs when the script sets none")
        p.add_argument("--edit-mode-as-saved", action="store_true")
        p.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT_S)
        p.add_argument("--ctrlrx", help="a CtrlrX checkout to search for the probe")
        p.add_argument("--allow-empty", action="store_true",
                       help="accept a panel that loads with 0 modulators")
    args = ap.parse_args(argv)
    try:
        probe = find_probe(args.ctrlrx)
        if args.cmd == "where":
            print(probe)
            return 0
        if not os.path.isfile(args.panel):
            raise ProbeError(f"panel file not found: {args.panel}")
        flags = ("--edit-mode-as-saved",) if args.edit_mode_as_saved else ()
        if args.cmd == "run":
            reports = [run_script(args.panel, _load_script(args), settle_ms=args.settle,
                                  check=False, flags=flags, probe=probe, timeout=args.timeout,
                                  allow_empty=args.allow_empty)]
        else:
            if not args.script:
                raise ProbeError("batch needs a JSONL file")
            with open(args.script, encoding="utf-8") as f:
                scripts = [json.loads(l) for l in f if l.strip()]
            with PersistentProbe(args.panel, flags=flags, probe=probe, timeout=args.timeout,
                                 allow_empty=args.allow_empty) as pp:
                reports = [pp.run(s, settle_ms=args.settle, check=False) for s in scripts]
    except (ProbeError, OSError, json.JSONDecodeError) as e:
        print(f"error: {e}", file=sys.stderr)
        return 2
    bad = False
    for i, r in enumerate(reports):
        if args.report:
            print(json.dumps(r, indent=None if args.cmd == "batch" else 2))
        else:
            if args.cmd == "batch":
                print(f"=== script {i}")
            print(digest(r))
        bad |= r["summary"]["errorCount"] > 0 or bool(r["summary"].get("disabledMethods"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
