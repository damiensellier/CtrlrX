---
name: ctrlrx-panel-builder
description: >-
  Authors, edits, debugs and tests CtrlrX panels (.panel, .panelz, .bpanelz) and their Lua by
  editing the panel's XML ValueTree directly instead of clicking through the CtrlrX GUI, and
  verifies them headlessly with CtrlrPanelProbe. Use it whenever a task touches a CtrlrX or Ctrlr
  panel: adding or changing a modulator (uiSlider, uiCombo, uiButton, uiToggleButton, uiLabel,
  uiCustomComponent), wiring MIDI (CC, NRPN/RPN, 14-bit, Program Change, Pitch Bend, a SysEx
  formula or a Multi message list), writing panel Lua (luaModulatorValueChange,
  luaPanelMidiReceived, timers, paint and mouse callbacks), decoding a SysEx dump, building a value
  map, generating a panel from a spec, building a test-fixture panel, testing a panel without
  hardware, or finding out why a panel won't load or sends the wrong MIDI. Use it even when the
  user only names a synth they want a CtrlrX editor for.
---

# Building CtrlrX panels by editing the file

A CtrlrX **panel** is a JUCE `ValueTree`, saved as XML in a `.panel` file. Controls, their MIDI,
value maps and the Lua source all live in that tree. You author the `.panel` (XML) form; the
CtrlrX app's **File → Export** produces the `.bpanelz` that also embeds images and fonts.

## Rules that prevent the most common failures

Each of these has cost real panels silent wrong MIDI, lost data or a crash. Details are in the
reference named at the end of each rule.

1. **Copy, don't invent.** Start a new panel from `assets/template-panel.panel`. Add a control by
   copying a whole working `<modulator>` of the same `uiType` (`panel_tool.py dump FILE
   modulator:NAME`) and changing only the attributes in "The panel model" below. Never rebuild or
   sort an element's attributes: CtrlrX applies them in file order, and two attributes can write
   the same target. (`references/panel-file-format.md`)
2. **A modulator's own channel is ignored unless `midiMessageChannelOverride="1"`, in both
   directions.** Without the override it sends on, and only matches incoming MIDI on, the panel's
   `panelMidiOutputChannelDevice`, not on `panelMidiInputChannelDevice`.
   (`references/midi-and-values.md`)
3. **Value-change callbacks run after the setter returns** (asynchronously), so a flag set around
   a set cannot suppress them. Choose the setter by what it does (measured):

   | Lua call | sends the modulator's MIDI | runs its `luaModulatorValueChange` |
   |---|---|---|
   | `m:setValue(v, false)` | yes, only if `v` changed | yes, later, `source` 5, only if changed |
   | `m:setValue(v, true)` | yes, even if unchanged | yes, later, `source` 5 |
   | `m:setValue(v, false, true)` | no (muted) | yes, later, `source` 5 |
   | `m:setModulatorValue(v, false, false, false)` | no | yes, later, `source` 5 |
   | `m:setModulatorValue(v, false, false, true)` | no | **never** (`ui=true` marks it "changed by program") |
   | `m:setValue(v)` (one argument) | error "No matching overload"; the calling method is **disabled** | — |

   A real knob turn arrives with `source` 4. To load a device dump without echoing it back, use
   the silent `setModulatorValue(v, false, false, true)` (then run dependent updates yourself), or
   gate the callback's send on `source == 4`. (`references/lua-api.md`)
4. **`getName()`, `getText()` and `getPropertyString()` return a JUCE `String` userdata.**
   `== "cutoff"` is always false, and `"" .. s` **crashes CtrlrX**. Identify a modulator by
   `getVstIndex()` or `getProperty("name")`; read text with `getProperty(...)`.
   (`references/lua-api.md`)
5. **A button that triggers an action is wired on `componentLuaMouseUp`**, not on
   `luaModulatorValueChange`. A button whose `uiButtonContent` has one line never changes value,
   so its value-change callback never fires. Wire one of the two, never both (it fires twice).
   A `uiCustomComponent` paints and takes clicks only through its own `uiCustom*Callback` hooks
   (card below); its `componentLuaMouse*` hooks never fire.
   (`references/components.md`)
6. **Lua code is an XML attribute.** Escape it with `panel_tool.py escape` only when you paste
   raw text into the XML; an XML library (ElementTree, lxml) escapes on write, and escaping first
   gives `&amp;quot;`. The inner function name equals `luaMethodName`. Never put your methods in
   the `Built-In` group (CtrlrX empties it on load). The language is Lua 5.1 (LuaJIT 2.1 from
   CtrlrX 5.6.36): no `//` or `&`/`|` operators; `goto` and `bit.band` only from 5.6.36.
   Top-level code in one method must not read another method's globals: CtrlrX's exporter
   reorders methods. (`references/panel-file-format.md`)
7. **Panel settings that bite:** save with `uiPanelEditMode="0"` (in edit mode a control hidden
   through its property is only dimmed); keep `panelMidiThruD2D="0"` unless the user wants
   device→device thru (it echoes every device message, a dump included, straight back); set
   `panelMidiGlobalDelay="2"` when Lua sets several NRPN/Multi modulators in one go (at 0 their
   bursts interleave on the wire). The template already has all three.
   (`references/panel-file-format.md`)
8. **The tree is not the screen.** Hide a control by setting its `componentVisibility` property
   (`comp:setPropertyInt("componentVisibility", 0)`); `comp:setVisible()` changes the screen only.
   Nothing runs a value-change callback on load, so state derived from a selector (visibility,
   dependent ranges, labels) needs an init call from `luaPanelLoaded`.
   (`references/lua-api.md`)
9. **Verify, then claim.** Run `panel_tool.py lint` after every edit. Prove every behaviour claim
   (bytes sent, callback ran, control hidden) with the probe, assert on `summary.errorCount`, and
   run each new check once against a broken panel to see it fail: a check that never failed proves
   nothing. A run with `errorCount` above 0 (`probe_harness.py` exit 1) is a failure, however the
   rest looks: a method that raised is disabled and does nothing from then on. Read its `errors`
   and fix them before you report anything as working. `lint` does not run Lua.
   (`references/verify-with-probe.md`)
10. **Never hand-edit a generated panel**, and when Lua lives in `.lua` files the files win: edit
    the source, then regenerate or re-inline. (`references/workflow-generated-panel.md`,
    `references/workflow-lua-in-files.md`)

## Lua API card

Everything here is measured. Before using a call that isn't here, look it up in
`references/lua-api.md` (section in the right column): an invented method or argument list fails
only at runtime, and CtrlrX then disables the whole method.

**Hooks.** The attribute names a method; its parameter list is fixed.

| Hook attribute (element) | Called as |
|---|---|
| `luaModulatorValueChange` (`<modulator>`) | `f(mod, value, source)`; `source` 4 = user, 5 = Lua, 2 = MIDI in |
| `componentLuaMouseUp` / `…Down` (`<component>`; not on a `uiCustomComponent`) | `f(comp, event)` |
| `uiCustomPaintCallback` (`uiCustomComponent`) | `f(comp, g)` |
| `uiCustomMouseDownCallback` / `…Up…` / `…Drag…` (`uiCustomComponent`) | `f(comp, event)`; `event.x`, `event.y` relative to the component |
| `luaPanelLoaded` (`<panel>`) | `f()` |
| `luaPanelMidiReceived` (`<panel>`) | `f(midi)`, a `CtrlrMidiMessage`; a SysEx arrives whole |
| `timer:setCallback(id, f)` | `f(id)` |

More hooks: `lua-api.md`, "Callback hooks and signatures".

**Calls.**

| To | Call | `lua-api.md` section |
|---|---|---|
| find a modulator | `panel:getModulatorByName("name")`, `nil` if there is none | CtrlrPanel |
| read its name, value | `mod:getProperty("name")` (a Lua string, rule 4), `mod:getValue()` | CtrlrModulator |
| set its value | a setter from the table in rule 3 | CtrlrModulator |
| show or hide it | `mod:getComponent():setPropertyInt("componentVisibility", 0)` (1 shows) | CtrlrComponent |
| send MIDI | `panel:sendMidiMessageNow(CtrlrMidiMessage({0xB0, 0x07, 0x64}))`: the one send call; a SysEx byte table runs from `0xF0` to `0xF7` | Sending MIDI |
| read incoming MIDI | `local d = midi:getData()` (a MemoryBlock), `d:getByte(i)` (0-based), `math.floor(tonumber(d:getSize()))` | Reading an incoming message |
| paint | `g:setColour(Colour(0xff336699))`, `g:fillRect(x, y, w, h)`, `g:drawRect(x, y, w, h, 1)` (1 = line width); integers only | Painting a `uiCustomComponent` |
| run code later | `timer:setCallback(id, f)`, `timer:startTimer(id, ms)`, `timer:stopTimer(id)`; every timer repeats until stopped, and ids are shared by the whole panel | Timers |
| log | `console("text")` | Globals |

## Tools — run them, don't read them

All are stdlib `python3`; `--help` on each is complete. Paths are relative to this skill directory.

| Command | Use it to |
|---|---|
| `python3 scripts/panel_tool.py summary FILE` | see every modulator: name, uiType, MIDI, range, Lua. Works on all four formats. |
| `python3 scripts/panel_tool.py dump FILE modulator:NAME` | print one element as a copy-template (also `editor`, `panel`, `modulators`). |
| `python3 scripts/panel_tool.py lint FILE` | check structure and wiring traps. Exit 0 = no errors; read the warnings too. |
| `python3 scripts/panel_tool.py xml IN OUT.panel` | turn a `.bpanelz` / `.panelz` into an XML `.panel` to edit. Decompressed bytes are not XML: never save them as `.panel`. |
| `python3 scripts/panel_tool.py escape lua.txt` | escape raw Lua for a `luaMethodCode` attribute (stdin with `-`). |
| `python3 scripts/probe_harness.py run PANEL SCRIPT.json` | run the panel headlessly and print what it sent, returned and raised. |
| `python3 scripts/png_sample.py shot.png X Y --expect rrggbb` | check one pixel of a probe screenshot. |

**The probe binary.** `probe_harness.py` uses `$CTRLRX_PANEL_PROBE` if set, otherwise the newest
`build*/Tests/CtrlrPanelProbe` (or `.exe`, also one `<Config>/` level deeper) in a CtrlrX clone
found from the current directory upwards, or given with `--ctrlrx DIR`. `probe_harness.py where`
shows which one it picked. If there is none: in a clone, build it
(`cmake --build build --target CtrlrPanelProbe`); without a clone, ask the user to download the
`CtrlrPanelProbe-<OS>` artifact of a CI run of https://github.com/damiensellier/CtrlrX (Actions → a
`build_and_test` run → Artifacts; needs a GitHub login) and set `CTRLRX_PANEL_PROBE` to the
unzipped binary.

## Pick your task

| Task | Do | Read |
|---|---|---|
| New panel | checklist A | `panel-file-format.md` |
| Add or change a control | checklist B | `components.md` |
| Wire or fix MIDI | checklist C | `midi-and-values.md` |
| Write or attach Lua | checklist D | `lua-api.md` |
| Panel won't load / sends the wrong thing / a control is dead | checklist E | `verify-with-probe.md` |
| Prove behaviour, write a regression test | checklist F | `verify-with-probe.md` |
| Panel built by a generator, or Lua kept in `.lua` files | checklist G | `workflow-generated-panel.md`, `workflow-lua-in-files.md` |

All references are in `references/`. Copy the checklist into your notes and tick it off.

**A. New panel**
- [ ] Copy `assets/template-panel.panel`; set the root `name`, `panelVendor`, `panelDevice`.
- [ ] Keep the template's channel (1), `uiPanelEditMode="0"`, `panelMidiThruD2D="0"`,
      `panelMidiGlobalDelay="2"` unless the task needs otherwise.
- [ ] Add controls with checklist B. Every modulator: unique `name`, unique `vstIndex`, a
      `componentLayerUid` that names a layer in `<uiPanelEditor>`.
- [ ] Lint → fix → lint until clean. Load it in the probe (no script) and check `finalState`
      lists every control with `effectiveVisible: true`.

**B. Add or change a control**
- [ ] Pick the `uiType` (`components.md`, "Quick chooser"). Dump a same-type modulator and copy it.
- [ ] Set `name`, `vstIndex`, `componentRectangle`, `componentVisibleName`, the range on **both**
      the modulator (`modulatorMin`/`Max`) and a slider's component (`uiSliderMin`/`Max`).
- [ ] Set the `<midi>` child (checklist C) or `midiMessageType="9"` for no MIDI.
- [ ] Lint; probe a `setValue` and assert the bytes (rule 9).

**C. Wire or fix MIDI**
- [ ] `midiMessageType` is an integer: 0 CC, 3 NoteOn, 5 SysEx, 6 Multi, 7 Program Change,
      8 PitchWheel, 9 None (full table in `midi-and-values.md`).
- [ ] Own channel? Set `midiMessageChannelOverride="1"` (rule 2).
- [ ] NRPN / RPN / 14-bit CC → `midiMessageType="6"` with a long-form Multi list copied from
      `midi-and-values.md` (the short `CC,99,2 : …` form ignores its numbers); SysEx → a formula.
      Multi modulators never update from incoming MIDI; decode replies in Lua.
- [ ] Every SysEx frame starts with `F0` and ends with `F7`; a frame built in Lua gets no `F7`
      added for you.
- [ ] Before coding, write the expected bytes for one concrete value straight from the device's
      spec (every constant byte included) and assert exactly those; an expectation derived from
      your own code can't catch a missing byte.
- [ ] Probe: `setValue` → assert exact `deviceMidiOut`; `injectDeviceMidi` → assert the value moved.

**D. Write or attach Lua**
- [ ] Read `references/lua-api.md` first. Call only functions it shows or that you find in the
      CtrlrX source: a guessed method or argument list fails only at runtime ("attempt to call
      method … (a nil value)", "No matching overload"), and CtrlrX then disables the method.
- [ ] Write the method in plain Lua; the callback's parameter list is fixed (`lua-api.md`,
      "Callback hooks").
- [ ] Add a `<luaMethod>` under `<luaManagerMethods>` (not in `Built-In`), escaped (rule 6), and
      set the callback attribute to its name.
- [ ] Check the setter table (rule 3) and the `String` rule (rule 4) against your code.
- [ ] Lint; probe with `callLuaMethod` or `eval`; assert `errorCount` 0 and the effect.

**E. Debug a panel**
- [ ] `panel_tool.py lint` — fix every error first (the probe refuses XML that doesn't parse).
- [ ] `panel_tool.py summary` — is the control's callback name a real method? Its channel what you
      think (rule 2)? Its type and number right?
- [ ] Probe a script that reproduces the symptom; read `errors`, `dialogs`, `deviceMidiOut`,
      `disabledMethods` (`verify-with-probe.md`, "Reading a report").
- [ ] Fix one cause, re-run the same script, and keep it as a regression check.

**F. Prove behaviour**
- [ ] One probe script per claim; assert exact bytes / values / `effectiveVisible`, and
      `summary.errorCount == 0`.
- [ ] Make it fail once: run it against the unfixed panel or a deliberately broken copy.
- [ ] For timer- or callback-driven output, wait with `waitFor` on a Lua condition, not a fixed `wait`.

**G. Generated panel or Lua in files**
- [ ] Find the source of truth (spec + generator, or the `.lua` files) and edit only that.
- [ ] Regenerate / re-inline, then lint, then run the panel's probe checks; commit source and
      output together.

## The panel model

```xml
<modulator name="cutoff" modulatorMin="0" modulatorMax="127" vstIndex="3"
           luaModulatorValueChange="cutoffChanged" ...>      <!-- value, range, Lua names -->
  <midi midiMessageType="0" midiMessageChannelOverride="0" midiMessageChannel="1"
        midiMessageCtrlrNumber="74" .../>                     <!-- what it sends and matches -->
  <component uiType="uiSlider" componentRectangle="16 16 72 96"
             componentVisibleName="Cutoff" uiSliderMin="0" uiSliderMax="127" .../>
</modulator>
```

- **`<modulator>`**: `name` (unique, case-sensitive, what Lua looks up), `modulatorMin`/`Max`/
  `Value`, `vstIndex` (unique), value expressions, and the *names* of attached Lua methods
  (`"-- None"` = none).
- **`<midi>`**: `midiMessageType`, channel, `midiMessageCtrlrNumber`, plus
  `midiMessageMultiList` (Multi) or `midiMessageSysExFormula` (SysEx). The value byte comes from
  the modulator's value; `midiMessageCtrlrValue` is ignored for ordinary messages.
- **`<component>`**: `uiType`, `componentRectangle` (`x y w h`), `componentVisibleName`,
  `componentLayerUid`, type-specific look.
- **Lua** lives once, in `<luaManager><luaManagerMethods>` near the top of the file; modulators,
  components and the `<panel>` root refer to methods by name. In every method the globals `panel`,
  `utils`, `timer`, `resources` are available.

## Ground truth: the CtrlrX source

The references were checked against CtrlrX's source. When a detail is missing or the panel
disagrees with them, the source of the user's CtrlrX version decides:

1. A local clone, if you know where it is (ask the user if one might exist).
2. Otherwise the repository on GitHub: https://github.com/damiensellier/CtrlrX. Read a file
   at the user's release tag (tags look like `v5.6.36`; the default branch is `master`) as raw
   text:
   `https://raw.githubusercontent.com/damiensellier/CtrlrX/<tag>/<path>`, e.g.
   `…/v5.6.36/Source/Core/CtrlrIDs.h`.

| Question | Look in |
|---|---|
| Enum values (`midiMessageType`, …) | `Source/MIDI/CtrlrMidiMessage.h` |
| Property / attribute names | `Source/Core/CtrlrIDs.h` |
| Component types and their properties | `Source/UIComponents/CtrlrComponents/` |
| What Lua can call | `Source/Lua/CtrlrLuaManager.cpp`, `Source/Lua/CtrlrComponentLuaRegistration.cpp`, `Source/Lua/JuceClasses/` |
| SysEx tokens and checksums | `Source/Core/CtrlrSysexProcessor.cpp`, `Source/Core/CtrlrSysexToken.*` |
| Callback names and signatures | `Source/Resources/XML/CtrlrLuaMethodTemplates.xml` |
| Panels guaranteed to load | `Tests/fixtures/fixture_*.panel` |
| The probe's full schema | `Doc/PanelProbe.md` |

The probe is built from current source, so a green probe run does not prove a panel runs on an
older installed CtrlrX; check bindings at the user's release tag (`git show <tag>:<file>` in a
clone, or the raw URL above).

## Reference files

Read the one the task needs; each starts with a table of contents.

- `references/panel-file-format.md` — the XML: root attributes (routing, channels, edit mode,
  delay), modulator / midi / component attributes, `<luaManager>`, escaping, Lua in external files,
  load order, `.bpanelz` and export differences.
- `references/midi-and-values.md` — message types, channels, Multi lists (14-bit, NRPN, signed
  values, burst merging), SysEx formulas and checksums, value expressions, value maps, receiving.
- `references/lua-api.md` — globals, `CtrlrPanel` / `CtrlrModulator` / `CtrlrComponent`, setters
  and callbacks, sending MIDI, timers, `String` / `MemoryBlock` / `File` traps, painting.
- `references/components.md` — the `uiType` catalogue, slider ranges, buttons, combos and value
  maps, labels, custom components.
- `references/verify-with-probe.md` — running the probe, actions incl. paint and mouse, reading a
  report, dialogs and timers, edit mode, writing checks that can fail.
- `references/workflow-generated-panel.md` — a panel generated from a spec: the loop and its guards.
- `references/workflow-lua-in-files.md` — Lua kept in `.lua` files and inlined into the panel.

## If this skill is wrong

Don't edit the skill during a task. When something here contradicts what you measured, say so in
your answer: what the skill says, what you observed, and the probe script or source line that
shows it. Tell the user they can report it, with that evidence, at
https://github.com/damiensellier/CtrlrX/issues.
