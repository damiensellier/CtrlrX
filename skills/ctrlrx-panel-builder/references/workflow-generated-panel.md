# Workflow: a panel generated from a spec

For a large panel (hundreds of parameters, several pages, a SysEx codec) the reliable shape is a
**spec** (the device's parameters as data) plus a **generator** script that writes the `.panel`.
The generated panel is build output: nobody edits it by hand.

## Contents

- Recognising a generated panel
- The loop
- What the generator should emit
- Guards that belong in the generator
- Keeping generated output honest
- Starting a generator from an existing panel

## Recognising a generated panel

Look before you edit a panel in a repository: a generator script (`tools/`, `gen/`, a `Makefile`
target such as `make panel`), a `spec/` or data directory, a README or comment saying "generated",
or methods in a `Generated` group. If any is present, the panel is output: change the spec or the
generator, never the `.panel`. A hand edit is lost at the next regeneration, and the check below
reports the file as stale.

## The loop

```
Generated-panel loop:
- [ ] 1. Edit the spec (parameter data) or the generator; hand-written Lua in its source file
- [ ] 2. Regenerate the panel
- [ ] 3. python3 scripts/panel_tool.py lint <panel>  → fix the generator until clean
- [ ] 4. Run the project's probe checks (or a probe script per changed behaviour)
- [ ] 5. Run the staleness check (regenerate to a temp file and compare)
- [ ] 6. Commit spec, generator and generated panel together
```

If step 3 or 4 fails, fix the generator or spec and go back to step 2, not to the panel.

## What the generator should emit

- **Copy, don't invent.** Start from a known-good panel root, layer definitions and one
  modulator per `uiType` (lifted with `panel_tool.py dump`), and substitute only the attributes
  that carry meaning: `name`, `vstIndex`, rectangle, range on modulator and component, `<midi>`.
- **Data tables, not code per parameter.** Emit the per-parameter facts (field offset, value map,
  display formatter id) as Lua tables keyed by `vstIndex`, and one shared callback for all
  parameters that dispatches on `mod:getVstIndex()`.
- **Hand-written Lua stays hand-written.** Keep codec, page logic and helpers in `.lua` source
  files; the generator inlines them (split on clear markers) rather than producing them.
- **Write the XML through a library** (`xml.etree.ElementTree` or similar) and pass plain Lua as
  the attribute value; the library escapes. Preserve attribute order from the template.
- **Same text in two places must match.** If the generator formats a value for display and the
  panel Lua formats the same value at runtime, test that both give identical strings for every
  value.
- Panel defaults: `uiPanelEditMode="0"`, `panelMidiThruD2D="0"`, `panelMidiGlobalDelay="2"`, the
  device's channel in `panelMidiOutputChannelDevice`.

## Guards that belong in the generator

Make generation fail, with a message naming the parameter, when:

- two modulators get the same `name` or `vstIndex` (also across device variants or pages);
- two modulators claim the same spec field;
- a control's rectangle overlaps another visible control or leaves its group (the probe's
  `finalState.overlaps` cross-checks this after load);
- a page or layer referenced by a control doesn't exist, or a page map and its enum disagree;
- a value map is not total over the device's values (a missing value silently loads index 0).

## Keeping generated output honest

- **Staleness check in CI**: the generator gets a `--check` mode that generates to memory and
  exits non-zero when the committed panel differs. Every derived artifact (panel, generated Lua
  tables, docs) goes through the same check, or one of them drifts silently.
- **Probe after every regeneration**, at least a load with no script: lint passes on panels that
  CtrlrX loads with controls missing or invisible.
- **Diff `finalState` when changing the generator**: load the old and new panel in the probe and
  compare values, ranges and visibility; the intended change should be the only difference.

## Starting a generator from an existing panel

1. `panel_tool.py summary` the panel and save it: it is the inventory the spec must reproduce.
2. `panel_tool.py dump` the root (`panel`), the editor (`editor`) and one modulator per `uiType`
   as templates.
3. Write the spec as data (CSV, JSON, YAML, or Python literals) from the summary and the device
   manual.
4. Generate, then compare `summary` of the original and the generated panel until they match
   except where you meant to change something. Probe both and diff `finalState`.
