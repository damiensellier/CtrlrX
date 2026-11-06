# Evals for `ctrlrx-panel-builder`

Eighteen realistic panel-authoring tasks, each graded by a script that checks the result the way
CtrlrX would see it: `panel_tool.py lint`, the panel's XML, and **behaviour** observed through
`CtrlrPanelProbe` (the exact MIDI bytes sent, label text, visibility, Lua state). Only one
expectation (eval 2's diagnosis) is judged from the agent's prose.

| id | name | what it exercises |
|---|---|---|
| 0 | new-panel-two-cc-knobs | a new panel from scratch; CC on the right channel |
| 1 | callback-updates-label | `luaModulatorValueChange` + label, in a copy of the panel |
| 2 | debug-dead-control | diagnose a callback that names a non-existent method |
| 3 | nrpn-14bit | 14-bit NRPN, exact CC 99/98/6/38 order |
| 4 | sysex-roland-checksum | SysEx formula with a Roland checksum, incl. the `sum % 128 == 0` edge |
| 5 | combo-value-map | combo entries mapped to non-index CC values; `&` in an entry name |
| 6 | parse-sysex-dump | an incoming dump updates knobs; other SysEx ignored |
| 7 | timer-throttled-burst | queue + timer: 16 CCs spaced out, none dropped, re-triggerable |
| 8 | show-hide-by-type | visibility driven by a selector (`effectiveVisible`, judged in run mode) |
| 9 | custom-step-grid | `uiCustomComponent` paint + mouse handler, state in Lua |
| 10 | write-probe-test | the agent's own probe check must pass the panel and fail mutants |
| 11 | lua-files-inlined | Lua in `.lua` files, inlined into the panel by an idempotent script |
| 12 | edit-bpanelz | edit a binary `.bpanelz`, deliver a loadable `.panel` |
| 13 | shared-callback-dispatch | one Lua method for many knobs must tell them apart (`getName()` is a userdata) |
| 14 | second-device-channel | a knob on its own MIDI channel, out and in (the panel channel wins unless overridden) |
| 15 | dump-without-echo | load a dump without echoing parameter changes or the dump back (async callbacks, MIDI thru) |
| 16 | trigger-buttons | two buttons that each send a SysEx frame on every click (held out) |
| 17 | format-for-diff | a diff-friendly formatter that must not change what the panel does (held out) |

Cases marked `"held_out": true` were added after the skill's last round of changes and were
never used to change it. They check that a gain on the other cases carries over to tasks the
skill was not tuned on. Don't edit the skill to pass them; when they have been used that way,
drop the flag and write new held-out cases.

## Layout

- `evals.json` — the cases (skill-creator schema: `id`, `prompt`, `expected_output`, `files`,
  `expectations`) plus `grader` per case and an `environment` block describing the run setup.
- `files/` — the input panels. They are small and purpose-built from `assets/template-panel.panel`.
- `graders/` — one stdlib Python grader per case, plus `common.py`.

## Running a case

1. Make a fresh, empty working directory and copy the case's `files` into it by basename.
2. Symlink a CtrlrX source checkout into it as `ctrlrx` (read-only is best), and set
   `CTRLRX_PANEL_PROBE` to a built `CtrlrPanelProbe` (`cmake --build build --target CtrlrPanelProbe`).
3. Give the agent `environment.preamble` followed by the case's `prompt`. To test the skill, copy
   this skill directory to `./skill/ctrlrx-panel-builder/` and prefix the prompt with
   `Instructions for this kind of task are in skill/ctrlrx-panel-builder/SKILL.md - read it first.`
   Any agent that can read files and run a shell can take part.
   - **Isolate the run.** The agent must see only its working directory, the `ctrlrx` checkout
     and the probe. An agent that searches the disk can find another copy of the skill, these
     graders or another run's outputs, and the comparison is then void. Hide the home directory
     and shared temp directories from it, e.g. with `bwrap` (Linux) mounting a tmpfs over them.
   - **Pin the model by its full id**, not an alias: an alias can move to a newer model between
     two runs you mean to compare.
4. Afterwards, put the working directory at `<run-dir>/outputs/` and the agent's last message at
   `<run-dir>/final_message.txt`. Then grade:

```bash
CTRLRX_PANEL_PROBE=/path/to/CtrlrPanelProbe python3 graders/eval_03_nrpn_14bit.py <run-dir>
```

The grader prints PASS/FAIL per expectation and writes `<run-dir>/grading.json`
(`expectations[].{text, passed, evidence}` + `summary`). skill-creator's `aggregate_benchmark`
and its viewer read that format directly.

Optional environment:

- `CTRLRX_PANEL_TOOL` — the `panel_tool.py` to lint with (default: this skill's own). Pin it when
  comparing two skill versions, so lint changes don't move the score.
- `EVAL_JUDGE_CMD` — a command that reads a prompt on stdin and answers YES/NO, e.g.
  `claude -p --model haiku`. Without it, eval 2's diagnosis is checked by keywords.

## Changing the skill

`../MAINTAINING.md` lists where changes go and the checks before a pull request, including the
old-vs-new eval run described above.

## Writing a new case

Ground it in something a real panel needed. Assert on observable behaviour (bytes, state), not
on how it was done. Before you trust the grader, make one correct and one wrong output by hand and
check that the grader passes the first and fails the second. A grader that cannot fail measures
nothing.
