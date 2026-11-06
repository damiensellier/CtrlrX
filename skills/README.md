# Agent skills for CtrlrX

This folder holds skills for AI coding agents (Claude Code, GitHub Copilot, opencode, pi, Hermes
and others). A skill is a folder of instructions, reference notes and scripts that an agent reads
when a task calls for it. The skills follow the open [Agent Skills](https://agentskills.io) format,
so the same folder works in every agent that supports it.

## `ctrlrx-panel-builder`

Teaches an agent to build and edit CtrlrX panels by editing the `.panel` file directly, and to check
the result with [`CtrlrPanelProbe`](../Doc/PanelProbe.md) instead of clicking through CtrlrX. With it,
an agent can:

- start a panel from a template, or add and change controls by copying working ones;
- wire controls to MIDI: CC, NRPN/RPN, 14-bit, Program Change, Pitch Bend, SysEx formulas with
  checksums, Multi message lists;
- write panel Lua: value-change and MIDI-received callbacks, timers, custom paint and mouse
  handlers, SysEx dump parsing;
- open a `.bpanelz` / `.panelz` as an editable `.panel`;
- test a panel without hardware: which MIDI bytes a control sends, what a dump from the synth does
  to the controls, Lua errors, visibility, pixels of a custom component;
- work on panels kept in git: Lua in separate `.lua` files, or a panel generated from a spec.

The agent never needs CtrlrX to be running. What it changes is checked by the same code CtrlrX runs.

The skill is measured on 18 realistic panel tasks, graded by scripts that lint the result and run
it in the probe; see
[`evals/benchmark.md`](ctrlrx-panel-builder/evals/benchmark.md) for the current pass rates per model.

### What you need

- **Python 3.8 or newer** as `python3`. The scripts use the standard library only.
- **A `CtrlrPanelProbe` binary**, for testing. Either build it from a CtrlrX clone
  (`cmake --build build --target CtrlrPanelProbe`, see
  [Compiling](../Doc/readme/04-compiling.md#5--run-the-tests-optional)) or
  download the `CtrlrPanelProbe-<Linux|macOS|Windows>` artifact of a CI run (see
  [PanelProbe.md](../Doc/PanelProbe.md#getting-the-binary)). Then point the skill at it:

  ```bash
  export CTRLRX_PANEL_PROBE=/path/to/CtrlrPanelProbe     # e.g. in ~/.bashrc or ~/.zshrc
  ```

  Without the variable, the skill looks for `build*/Tests/CtrlrPanelProbe` in a CtrlrX clone at or
  above the current directory, which works only when you run the agent inside that clone.
  Without a probe, the agent can still edit and lint panels, but can't check what they do.

### Install

Copy or symlink `skills/ctrlrx-panel-builder` into a skills folder your agent reads. A **personal**
folder makes the skill available in every project; a **project** folder only in that repository
(commit it there if your collaborators should have it too).

| Agent | Personal folder | Project folder |
|---|---|---|
| Claude Code | `~/.claude/skills/` | `.claude/skills/` |
| GitHub Copilot (VS Code) | `~/.copilot/skills/`, also reads `~/.claude/skills/` and `~/.agents/skills/` | `.github/skills/`, also `.claude/skills/` and `.agents/skills/` |
| opencode | `~/.config/opencode/skills/`, also reads `~/.claude/skills/` and `~/.agents/skills/` | `.opencode/skills/`, also `.claude/skills/` and `.agents/skills/` |
| pi | `~/.agents/skills/` | `.agents/skills/` |
| Hermes Agent | `~/.hermes/skills/` | `.hermes/skills/` or `.agents/skills/` (after `hermes skills trust`) |
| Codex and other Agent Skills agents | `~/.agents/skills/` | `.agents/skills/` |

`~/.claude/skills/` covers Claude Code, Copilot in VS Code and opencode; `~/.agents/skills/` covers
the others. Put the skill in one folder per agent: an agent that reads two folders holding the
same skill may warn about the duplicate name.

A symlink into a CtrlrX clone stays current with `git pull`:

```bash
git clone https://github.com/damiensellier/CtrlrX.git ~/src/CtrlrX
mkdir -p ~/.claude/skills
ln -s ~/src/CtrlrX/skills/ctrlrx-panel-builder ~/.claude/skills/ctrlrx-panel-builder
```

On Windows, copy the folder, or link it from `cmd` with
`mklink /J %USERPROFILE%\.claude\skills\ctrlrx-panel-builder C:\src\CtrlrX\skills\ctrlrx-panel-builder`.

To check the install, start a new agent session in a folder with a panel and ask for something like
*"Add a knob to MySynth.panel that sends CC 74 on channel 1, and test it."* The agent should read
the skill's `SKILL.md` before it edits the file. To check the probe setup, run
`python3 scripts/probe_harness.py where` in the skill folder; it prints the binary it will use.

### When the skill is wrong

The skill tells the agent to report, not fix, a statement that contradicts what it measured. If
you get such a report, please open an issue at https://github.com/damiensellier/CtrlrX/issues
with what the skill says, what was observed, and the probe script or source line that shows it.

Changes to the skill go in as pull requests. [`MAINTAINING.md`](ctrlrx-panel-builder/MAINTAINING.md)
says what a change needs: evidence, where it goes, and the old-versus-new eval run. How to run and
grade the evals is in [`evals/README.md`](ctrlrx-panel-builder/evals/README.md).
