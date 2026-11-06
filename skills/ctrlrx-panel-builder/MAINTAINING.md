# Maintaining `ctrlrx-panel-builder`

For the people who change this skill, and for an agent that is explicitly asked to. An agent using
the skill for a panel task does not need this file: `SKILL.md` tells it to report a wrong
statement to the user, not to edit the skill.

## Where changes come from

Reports arrive as issues at https://github.com/damiensellier/CtrlrX/issues, ideally with what the
skill says, what was observed, and the probe script or source line that shows it. Changes go in
as pull requests against that repository. Copies installed elsewhere (a plugin cache, a personal
skills directory) are overwritten on update, so a fix made only there is lost.

## Before changing a fact

- **Reproduce it.** Write a probe script, or name the source line at a release tag. Put the
  evidence in the pull request. A claim that was never measured or read in the source doesn't go in.
- **Name the version.** CtrlrX changes underneath the skill (Lua 5.1 became LuaJIT 2.1 in 5.6.36;
  the Multi-list parser was rewritten in 5.6.35). Write boundaries as "from 5.6.36" / "up to
  5.6.35", never "currently" or a date.
- **Correct, don't append.** Change the existing text; a new note that contradicts an old one
  leaves the reader to guess which is true.
- **Run every code sample** in the probe before it goes into a reference.

## When an eval fails

A failing eval shows where an agent went wrong; on its own it doesn't justify a change. Tuning the
skill to the cases makes it worse for every task they don't cover.

- **Name a real task** besides the eval that the change helps. "The case passes" is not a reason.
- **Teach the fact, not the case's answer.** No case's names, bytes, CC numbers or sizes in
  examples, and no worked solution shaped like one case.
- **SKILL.md only for repeated mistakes**: the same one in at least two cases or on two models.
  Otherwise the detail goes in its reference.
- **Ignore noise.** A drop smaller than the run-to-run spread is not a reason to edit.
- **Stop when a fix moves the failure.** If a change fixes one case and breaks another, narrow or
  revert it; don't add more.
- **A model that ignores the pointers it is given** has reached its limit. Record that in
  `evals/benchmark.md` instead of copying references into `SKILL.md`.
- Check the result on the `held_out` cases (`evals/README.md`).

## Where things go

| Kind of content | Place |
|---|---|
| A rule that prevents a common, costly failure (silent wrong MIDI, data loss, a crash) | `SKILL.md`, "Rules" (keep about ten; replace one rather than add an eleventh) |
| The steps of a task | the matching checklist in `SKILL.md` |
| Detail, explanation, edge cases | the most specific file in `references/` |
| A mechanical check | `scripts/panel_tool.py lint`, with one case that must fire and one that must not |
| How to run and read the probe | `references/verify-with-probe.md` |
| The probe's full schema | `Doc/PanelProbe.md` (link it; don't copy it) |
| A whole way of working (generated panels, Lua in files) | a `references/workflow-*.md` file |

## Structure and portability rules

- `SKILL.md` stays well under 500 lines. Every reference is linked directly from `SKILL.md` (one
  level deep) and starts with a table of contents when it is over 100 lines.
- The `description` is third person, says what the skill does and when to use it, and stays under
  1024 characters.
- Any agent that can read files and run a shell must be able to follow the skill: no
  agent-specific mechanics, every step a concrete command or edit, decisions as tables.
- Scripts are standard-library `python3`, have a complete `--help`, and exit non-zero with a
  one-line, actionable error.
- No personal paths, private projects or session history in any file.

## Checks before a pull request

- [ ] `python3 scripts/panel_tool.py lint` has no errors on `assets/template-panel.panel` and on
      every panel in `evals/files/` except `broken_panel.panel` (broken on purpose for eval 2,
      exactly 1 error). The one `json4lua` warning is known.
- [ ] For a lint change: lint every `Tests/fixtures/*.panel` before and after, and explain every
      difference in the output.
- [ ] `--help` works for each script in `scripts/`.
- [ ] For a grader change: one correct and one wrong output made by hand; the grader passes the
      first and fails the second (`evals/README.md`).
- [ ] If the change documents probe behaviour: `ctest -R PanelProbeSelfTest` passes, and a
      self-test scenario pins the behaviour.
- [ ] **Old-vs-new eval run** (`evals/README.md`, "Running a case"): the previous release of the
      skill and the changed one, on the same probe build and the same models. The changed skill
      must not score lower on any model, never worse on scripted expectations, and where pass rates
      tie it must not cost more (cost and turns per run). Record pass rates, date and the skill and
      probe commits in `evals/benchmark.md`.
