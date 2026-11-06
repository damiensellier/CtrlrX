# Panel file format

A `.panel` file is a JUCE `ValueTree` serialized as XML. The same tree also ships as `.panelz`
(gzipped XML), `.bpanelz` (gzipped *binary* ValueTree that also embeds resources) and `.bpanel`
(the binary, uncompressed). `scripts/panel_tool.py` reads all four (`summary`, `lint`, `dump`); it
writes nothing, and you author the XML form.

## Contents

- Top-level shape and element order
- The `<panel>` root: settings that change behaviour
- `<uiPanelEditor>`: edit mode and canvas layers
- The `<modulator>` element and its `<midi>` / `<component>` children
- Attribute order is behaviour
- The `<luaManager>` block and method groups
- Escaping `luaMethodCode`
- Lua in external files (`luaMethodSource="1"`)
- Method code runs at load; method order is not stable
- `.bpanelz`, export and resources
- Authoring checklist

## Top-level shape and element order

```xml
<?xml version="1.0" encoding="UTF-8"?>
<panel name="My Panel" ...about 80 attributes...>
  <uiWindowManager/>
  <luaManager>
    <luaManagerMethods>
      <luaMethodGroup name="Built-In" uuid="…"> <!-- CtrlrX's utility methods only --> </luaMethodGroup>
      <luaMethod .../>     <!-- the panel's own methods -->
    </luaManagerMethods>
  </luaManager>
  <uiPanelEditor uiPanelCanvasRectangle="0 0 600 400" uiPanelEditMode="0" ...>
    <uiPanelCanvasLayer uiPanelCanvasLayerUid="…" uiPanelCanvasLayerIndex="0" .../>
  </uiPanelEditor>
  <modulator ...> <midi .../> <component .../> </modulator>
  ...
</panel>
```

Keep this order when generating a panel: `<uiWindowManager/>`, `<luaManager>`, `<uiPanelEditor>`,
then the modulators.

## The `<panel>` root: settings that change behaviour

Copy the root from `assets/template-panel.panel` and change only what you need.

| Attribute | Meaning | Recommended |
|---|---|---|
| `name`, `panelVendor`, `panelDevice`, `panelVersionMajor`/`Minor` | identity, shown in hosts | set per panel |
| `panelMidiOutputChannelDevice` | **the channel every modulator without `midiMessageChannelOverride="1"` sends on and matches on** | the device's channel (template: 1) |
| `panelMidiInputChannelDevice` | panel input channel setting; does **not** pick which channel modulators match | same as output |
| `panelMidiInputDevice` / `panelMidiOutputDevice` | selected ports (device-specific strings; the probe rebinds them to mocks) | leave |
| `panelMidiThruD2D` | device → device thru: every device message is echoed back to the device | `0` unless the user wants thru |
| `panelMidiThruH2D`, `…D2H`, `…H2H` | host↔device thru | leave unless asked |
| `panelMidiGlobalDelay` | ms between outgoing buffers; at `0`, Multi/NRPN bursts sent together interleave | `2` |
| `luaPanelLoaded`, `luaPanelMidiReceived`, … | panel-level callbacks: a method name or `"-- None"` (`lua-api.md`) | as needed |
| `panelGlobalVariables` | 64 colon-separated ints (`-1` = unset) | leave |
| `panelUID`, `panelInstanceUID` | identity; unique per panel | new value per new panel |

## `<uiPanelEditor>`: edit mode and canvas layers

- **`uiPanelEditMode="0"`** saves the panel in run mode, the way users play it. A panel saved with
  `"1"` (or without the attribute) opens in edit mode, where CtrlrX only **dims** a control hidden
  via its `componentVisibility` property and ignores slider drags. The match table for incoming
  MIDI is built either way. The probe runs panels in run mode unless `--edit-mode-as-saved` is
  given, and warns (`editMode`) about a file saved in edit mode.
- **Layers.** Each `<uiPanelCanvasLayer>` has a `uiPanelCanvasLayerUid`; a component's
  `componentLayerUid` must equal one of them or the component is never shown (no error). Layers
  stack by `uiPanelCanvasLayerIndex` (higher on top); `uiPanelCanvasLayerVisibility="0"` starts a
  layer hidden. Lift the whole element with `panel_tool.py dump FILE editor`.

## The `<modulator>` element and its `<midi>` / `<component>` children

| `<modulator>` attribute | Meaning |
|---|---|
| `name` | identity; unique, case-sensitive; what `getModulatorByName` finds |
| `modulatorMin`, `modulatorMax`, `modulatorValue` | raw range and value |
| `modulatorIsStatic` | `1` for labels, images, groups |
| `vstIndex` | host parameter index; unique per panel |
| `modulatorValueExpression` / `…Reverse` | value ↔ MIDI math (`midi-and-values.md`) |
| `luaModulatorValueChange`, `luaModulatorGetValueForMIDI`, `luaModulatorGetValueFromMIDI` | method names or `"-- None"` |
| `modulatorCustomIndex` | free Text slot for per-control data |

| `<midi>` attribute | Meaning |
|---|---|
| `midiMessageType` | integer enum (`midi-and-values.md`) |
| `midiMessageChannelOverride` | `1` = use `midiMessageChannel` in both directions; `0` = panel channel |
| `midiMessageChannel` | 1–16, used only with the override |
| `midiMessageCtrlrNumber` | CC / note / program number, or NRPN parameter for Multi |
| `midiMessageCtrlrValue` | ignored for ordinary messages (the value byte is the modulator value) |
| `midiMessageMultiList` | Multi list (long form only) |
| `midiMessageSysExFormula` | SysEx formula |

| `<component>` attribute | Meaning |
|---|---|
| `uiType` | the widget kind (`components.md`) |
| `componentRectangle` | `x y w h` inside its parent (group-relative for a grouped control) |
| `componentVisibleName`, `componentLabelVisible` | the name label (drawn inside the rectangle) |
| `componentVisibility` | `1`/`0`; what Lua should set to hide a control |
| `componentLayerUid` | the canvas layer (see above) |
| `componentLuaMouseDown` / `componentLuaMouseUp` | mouse callbacks for any component |
| type-specific: `uiSliderMin`/`Max`, `uiComboContent`, `uiButtonContent`, … | look and behaviour per type |

## Attribute order is behaviour

CtrlrX restores a modulator's properties in file order, and some properties write the same
target: `uiButtonContent` and `uiButtonTrueValue` both set `modulatorMax`, a slider's
`uiSliderMax` rewrites `modulatorMax`. Which one wins depends on their order in the file.

- Edit attributes in place; never sort, rebuild or re-serialize an element's attribute list in a
  script (one script that sorted attributes flipped `modulatorMax` on every one-line button).
- After any scripted bulk edit, load the panel in the probe before and after and diff
  `finalState`: lint and `errorCount` do not see this kind of damage.

## The `<luaManager>` block and method groups

```xml
<luaMethod luaMethodName="doSend"
           luaMethodCode="doSend = function(mod, value, source)&#10;&#9;console(&quot;sent&quot;)&#10;end"
           luaMethodLinkedProperty="" luaMethodSource="0" uuid="00000000000000000000000000000013"
           luaMethodValid="1"/>
```

| Attribute | Meaning |
|---|---|
| `luaMethodName` | the name callbacks refer to; the inner function name must match |
| `luaMethodCode` | the Lua, XML-escaped (absent with `luaMethodSource="1"`) |
| `luaMethodSource` | `0` code in `luaMethodCode`; `1` code in an external file |
| `luaMethodSourcePath` | with `1`: the file, relative to the panel's lua directory |
| `uuid` | unique id (32 hex digits) |
| `luaMethodValid` | `1` |

- Methods may sit directly under `<luaManagerMethods>` or in `<luaMethodGroup>`s of your own
  (groups may nest). **Never in `Built-In`**: CtrlrX discards that group's contents on load, so a
  method there vanishes.
- Group once a panel has more than a handful of methods: one group per section or feature
  (`Osc`, `Filter`, `Env`, …), a `util` group for helpers, a `Generated` group for script-written
  methods; keep panel-wide entry points (`panelLoaded`, `midiReceived`, the SysEx codec) at the
  root, where readers look first.

## Escaping `luaMethodCode`

| Character | Escape |
|---|---|
| newline | `&#10;` |
| carriage return | `&#13;` |
| tab | `&#9;` |
| `"` | `&quot;` |
| `&` | `&amp;` |
| `<` / `>` | `&lt;` / `&gt;` |

- When you write XML text yourself, run the plain Lua through `panel_tool.py escape` (it keeps
  every byte, CR included).
- When a script writes the panel through an XML library (`xml.etree`, lxml, JUCE), hand it the
  **plain** Lua: the library escapes on write, and pre-escaped input comes out as `&amp;quot;`.
- Lint after every write: a bad escape makes the file unparseable or truncates the method.

## Lua in external files (`luaMethodSource="1"`)

```xml
<luaMethod luaMethodName="codec" luaMethodSourcePath="codec.lua"
           luaMethodLinkedProperty="" luaMethodSource="1" uuid="…" luaMethodValid="1"/>
```

- The file is looked up in `<panel path without extension>/lua/`: for `…/MyPanel.panel` that is
  `…/MyPanel/lua/codec.lua`. An absolute path is used as is. CtrlrX resets `panelFilePath` to the
  file being opened, so a stale path in the XML doesn't matter.
- Export (`.bpanelz` / `.panelz`) inlines the files back into `luaMethodCode`, so the exported
  panel needs no lua directory.
- **A missing file is silent.** The method loads as an empty chunk and is reported valid: no
  compile error, no load error, its globals simply `nil` until something calls them.
  `panel_tool.py lint` reports every `luaMethodSourcePath` that doesn't resolve; run it.
- To keep Lua in files but ship a self-contained `.panel`, inline with a script instead
  (`workflow-lua-in-files.md`).

## Method code runs at load; method order is not stable

Each method's code runs as a chunk when the panel loads, so top-level statements execute then and
one method may define several helpers and constants. Do **not** rely on the order of methods:
CtrlrX's exporter reorders `<luaManagerMethods>` on save, so a top-level statement that reads a
global another method defines works in one file and breaks after the next export. Read such
globals inside function bodies, which run after loading has finished.

## `.bpanelz`, export and resources

- A `.bpanelz` carries **resources** (images, fonts, data) as `<resource>` nodes. An XML `.panel`
  export drops their data, so `resources:getResource(name)` returns `nil` in a `.panel` and any
  panel Lua that silently guards that `nil` loses its artwork. Ship artwork-bearing panels as
  `.bpanelz`, produced by the user via File → Export.
- To edit a `.bpanelz` or `.panelz`, convert it first: `panel_tool.py xml IN OUT.panel` writes the
  XML (attribute order kept) and reports its `<resource>` nodes. A `.bpanelz` decompresses to a
  binary ValueTree, not XML, so decompressed bytes saved as `.panel` don't load. Edit the XML,
  lint it, and probe it; for artwork, the user re-exports from CtrlrX.
- An export is not the `.panel` you wrote: it records the **live** slider ranges (after Lua changed
  them), may add newer attributes, reorders methods and keeps stale root metadata (name, author,
  version). Verify an export by running `panel_tool.py summary` and `lint` on both files and
  comparing, and by loading both in the probe and diffing `finalState`.

## Authoring checklist

- [ ] Root copied from the template; `name`, vendor, device, version set; channel, D2D thru,
      global delay and `uiPanelEditMode="0"` as in the root table.
- [ ] Element order: `<uiWindowManager/>`, `<luaManager>`, `<uiPanelEditor>`, modulators.
- [ ] Every modulator: unique `name` and `vstIndex`, a `<midi>` and a `<component uiType=…>`,
      a `componentLayerUid` that names a layer.
- [ ] Every referenced Lua method exists, outside `Built-In`, with a matching inner name.
- [ ] `panel_tool.py lint` clean; the panel loads in the probe with every control present.
