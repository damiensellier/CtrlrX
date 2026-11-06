# Benchmark

Pass rates of this skill on the cases in `evals.json`: per case, the share of its expectations
met, averaged over the runs. Every expectation except eval 2's diagnosis is checked by a script
(lint, XML, behaviour in `CtrlrPanelProbe`).

- **Date**: 2026-10-30 (Haiku 5.5, Sonnet 5.5, Haiku 4.5), 2026-10-31 (Opus 5.5).
- **Skill**: content hash `223153d248173d52dd4ba43ef77563d9ee4335824f6853303f4f972f3bbb81b6`, from
  `find . -type f -not -path './evals/*' -not -path '*/__pycache__/*' | sort | xargs sha256sum | sha256sum`
  in this directory (files with LF line endings, as git stores them). A different hash means the
  skill changed after these runs.
- **Probe**: a Release build of `CtrlrPanelProbe` (Linux) from the commit that last changed this
  file (`git log -1 -- skills/ctrlrx-panel-builder/evals/benchmark.md`).
- **Runs per case**: Haiku 5.5 3, Sonnet 5.5 3, Opus 5.5 1, Haiku 4.5 1. Claude Code, models pinned
  by full id (`claude-haiku-5-5`, `claude-sonnet-5-5`, `claude-opus-5-5`,
  `claude-haiku-4-5-20251001`). Each run sandboxed as described in `README.md`; eval 2's diagnosis
  checked by keywords (no `EVAL_JUDGE_CMD`).

| # | case | Haiku 5.5 | Sonnet 5.5 | Opus 5.5 | Haiku 4.5 |
|---|---|---|---|---|---|
| 0 | new-panel-two-cc-knobs | 1.00 | 1.00 | 1.00 | 1.00 |
| 1 | callback-updates-label | 1.00 | 1.00 | 1.00 | 0.89 |
| 2 | debug-dead-control | 1.00 | 1.00 | 1.00 | 0.67 |
| 3 | nrpn-14bit | 1.00 | 1.00 | 1.00 | 0.71 |
| 4 | sysex-roland-checksum | 1.00 | 1.00 | 1.00 | 0.57 |
| 5 | combo-value-map | 1.00 | 1.00 | 1.00 | 1.00 |
| 6 | parse-sysex-dump | 1.00 | 1.00 | 1.00 | 1.00 |
| 7 | timer-throttled-burst | 1.00 | 1.00 | 1.00 | 1.00 |
| 8 | show-hide-by-type | 1.00 | 1.00 | 1.00 | 1.00 |
| 9 | custom-step-grid | 1.00 | 1.00 | 1.00 | 1.00 |
| 10 | write-probe-test | 1.00 | 1.00 | 1.00 | 1.00 |
| 11 | lua-files-inlined | 1.00 | 1.00 | 1.00 | 1.00 |
| 12 | edit-bpanelz | 1.00 | 1.00 | 1.00 | 1.00 |
| 13 | shared-callback-dispatch | 1.00 | 1.00 | 1.00 | 1.00 |
| 14 | second-device-channel | 1.00 | 1.00 | 1.00 | 1.00 |
| 15 | dump-without-echo | 1.00 | 1.00 | 1.00 | 0.88 |
| 16 | trigger-buttons | 1.00 | 1.00 | 1.00 | 1.00 |
| 17 | format-for-diff | 1.00 | 1.00 | 1.00 | 0.73 |
| | **overall** | **1.00** | **1.00** | **1.00** | **0.91** |

Cases 16 and 17 are held out (`README.md`): they were written after the skill's last change and
never used to change it.

**Haiku 4.5 is the weak-model floor**, reported but not used to accept or reject a change. It
mostly reads `SKILL.md` and rarely opens a reference that a checklist points to, so a fact that
lives only in a reference is often missing from its work. Three of its failures above are of that
kind: a short-form Multi list (eval 3), an invented checksum syntax (eval 4), and a formatter that
changed the Lua code and the buttons' range (eval 17), each where a reference spells out the right
form. This is a limit of the model, not a gap to close by copying references into `SKILL.md`
(`MAINTAINING.md`).

To reproduce a run, follow "Running a case" in `README.md`, then grade it:

```bash
CTRLRX_PANEL_PROBE=/path/to/CtrlrPanelProbe python3 graders/eval_03_nrpn_14bit.py <run-dir>
```
