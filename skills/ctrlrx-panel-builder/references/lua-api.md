# Lua API for panels

CtrlrX runs the **Lua 5.1** language: plain Lua 5.1 up to CtrlrX 5.6.35, LuaJIT 2.1 from 5.6.36
(`_VERSION` is `"Lua 5.1"` in both). No `//` or `&`/`|`/`<<` operators on any version; from
5.6.36 `goto` works and the `bit` library (`bit.band`, `bit.bor`, `bit.lshift`, …) exists. For a
panel that must run on older CtrlrX too, use neither.

This page is the API panel authors use. The bindings are the authority:
`Source/Lua/CtrlrLuaManager.cpp`, `Source/Lua/CtrlrComponentLuaRegistration.cpp`,
`Source/Lua/JuceClasses/`; in the running app `how()` lists the bound classes and `what(obj)` an
object's methods.

## Contents

- How methods attach and when their code runs
- Globals
- CtrlrPanel
- CtrlrModulator: setters, callbacks and `source`
- CtrlrComponent: visibility and properties
- Callback hooks and signatures
- Sending MIDI
- Reading an incoming message
- Timers
- Dialogs
- JUCE `String` userdata
- Files and `MemoryBlock`
- Painting a `uiCustomComponent`
- Version floor
- A complete small example

## How methods attach and when their code runs

Methods live in `<luaManager>` and are referenced **by name** from callback attributes; the inner
function name equals `luaMethodName`, and a callback naming a method that doesn't exist never
fires (`panel-file-format.md` has the XML).

- Each method body is a **chunk run at panel load**, so top-level statements run then, and one
  method may define constants and several helpers.
- Load order is not something to rely on: CtrlrX's exporter reorders the methods. Top-level code
  must not read globals another method defines; read them inside function bodies, which run later.
- A runtime error inside a callback **disables that method** for the rest of the session.

## Globals

| Global | What it is |
|---|---|
| `panel` | The panel (`CtrlrPanel`), the entry point. |
| `utils` | Dialogs, encoding helpers, version string. |
| `timer` | Repeating timers (`CtrlrLuaMultiTimer`). |
| `resources` | Embedded images / fonts / data. |
| `devices` | MIDI device manager. |

`console("text")` prints to the Lua console (and to the probe's `steps[i].console`).

## CtrlrPanel

| Method | Purpose |
|---|---|
| `getModulatorByName(name)` | The modulator with that `name` (case-sensitive), or `nil`; check before chaining. |
| `getModulatorByIndex(i)` / `getNumModulators()` | Index access / count. |
| `getComponent(name)` / `getLabel(name)` / `getComboComponent(name)` | A modulator's component by name. |
| `getCanvas()` | The canvas; `getCanvas():getLayerByName(n):setVisible(b)` switches a layer. |
| `sendMidiMessageNow(msg)` | Send a `CtrlrMidiMessage` now. |
| `getGlobalVariable(i)` / `setGlobalVariable(i, v)` | Panel-wide integer slots. |
| `getProgramState()` / `setProgramState(...)` | Capture / restore all values. |
| `isRestoring()` | True while the panel restores state. |

## CtrlrModulator: setters, callbacks and `source`

`local m = panel:getModulatorByName("cutoff")`

Getters: `getValue()` / `getModulatorValue()` (numbers), `getMinModulatorValue()` /
`getMaxModulatorValue()`, `getVstIndex()` (a number), `getComponent()`, `getMidiMessage()`,
`getProperty("name")` (a Lua string; not `getName()`, see "JUCE `String` userdata").

**Setters — measured with the probe** (`luaModulatorValueChange` attached to `m`):

| Lua call | sends the modulator's MIDI | runs its `luaModulatorValueChange` |
|---|---|---|
| `m:setValue(v, false)` | yes, only if `v` changed | yes, later, `source` 5, only if changed |
| `m:setValue(v, true)` | yes, even if unchanged | yes, later, `source` 5 |
| `m:setValue(v, false, true)` | no (third argument = mute) | yes, later, `source` 5 |
| `m:setModulatorValue(v, false, false, false)` | no | yes, later, `source` 5 |
| `m:setModulatorValue(v, false, true, false)` | yes | yes, later, `source` 5 |
| `m:setModulatorValue(v, false, false, true)` | no | **never** |
| `m:setModulatorValue(v, false, true, true)` | yes | **never** |
| `m:setValue(v)` | error "No matching overload"; the method calling it is disabled | — |

`setModulatorValue(v, vst, midi, ui)`: `ui=true` tags the change "changed by program", and CtrlrX
skips the callback for those. Panels written for older Ctrlr use
`setModulatorValue(v, true, false, true)` to load a patch; on CtrlrX that silently skips every
dependent callback (labels, ranges, visibility).

**Callbacks run later.** `luaModulatorValueChange` runs from an async update, after the Lua chunk
that set the value has returned (measured: inside the chunk the callback has not run; on the next
step it has). Consequences:

- A flag set before and cleared after a set (`loading = true … loading = false`) is already false
  when the callbacks run, so it can't suppress their MIDI.
- A value set in a chunk can't drive a dependent setting read later in the same chunk.
- Setting many values with `setValue(v, true)` in a loop costs one callback round per value; a
  bulk load is much faster with the silent setter plus one explicit update.

To load a device dump without echoing it back, either set every value with
`setModulatorValue(v, false, false, true)` and then call the dependent update functions yourself,
or keep `setValue` and send from the callback only for genuine user edits:

```lua
cutoffChanged = function(mod, value, source)
    if source ~= 4 then return end          -- 4 = GUI; 5 = Lua; 2 = MIDI in
    -- F0, manufacturer, device, parameter, value, F7: copy the frame from the device's spec
    panel:sendMidiMessageNow(CtrlrMidiMessage({0xF0, 0x7D, 0x00, 0x01, value, 0xF7}))
end
```

`source` values: `0` initial, `1` host, `2` MIDI in, `3` controller, `4` GUI, `5` Lua, `6`
program, `7` link, `8` unknown (`LastChangeSource` in `CtrlrModulatorProcessor.h`).

No callback runs when a panel loads. State derived from a selector needs an init call from the
panel's `luaPanelLoaded` method with the restored values.

## CtrlrComponent: visibility and properties

`local c = m:getComponent()` (or `panel:getComponent("cutoff")`)

| Group | Methods |
|---|---|
| Geometry | `getX/Y/Width/Height()`, `getBounds()`, `setBounds()` |
| Properties | `getProperty(p)`, `setProperty(p, v, false)`, `setPropertyInt(p, n)`, `setPropertyString(p, s)` |
| Owner | `getOwner()` (the modulator) |
| Paint | `repaint()` (has no effect under the probe) |

- **Hide and show with the `componentVisibility` property**:
  `c:setPropertyInt("componentVisibility", on and 1 or 0)`. `c:setVisible()` changes only the
  screen, not the panel state; the probe's `visible` field reads the property. Re-apply visibility
  after a layer or page switch, and at load from `luaPanelLoaded`.
- **Property names differ per `uiType`** and a wrong name fails silently. Take the name from a
  dumped component (`panel_tool.py dump`) and read it back with `eval` in the probe.
- `getProperty` returns a native Lua value of the type it was stored as; values loaded from the
  XML are usually strings (`getProperty("uiSliderMax")` → `"127"`), so `tonumber()` them.
  `getPropertyString` returns a `String` userdata (below).

Type-specific: toggle/image button `getToggleState()`; label `setText()`; combo
`getSelectedItemIndex()` / `setSelectedItemIndex(i, dontNotify)`; list box `getSelectedRow()`,
`updateContent()`; file list `getSelectedFile()`.

## Callback hooks and signatures

The parameter list is fixed per hook; write the body, keep the parameters.

**Panel** (attributes on `<panel>`):

| Attribute | Fires when | Signature |
|---|---|---|
| `luaPanelBeforeLoad` / `luaPanelLoaded` / `luaPanelResourcesLoaded` | lifecycle | `function()` |
| `luaPanelSaved` | panel saved | `function(saveType, file)` |
| `luaPanelMidiReceived` | any MIDI in (SysEx when complete) | `function(midi)` |
| `luaPanelModulatorValueChanged` | any modulator changed | `function(mod, value)` |
| `luaPanelProgramChanged` | program change | `function(prog, bankLsb, bankMsb)` |
| `luaPanelGlobalChanged` | global variable changed | `function(index, value)` |
| `luaPanelPaintBackground` | custom background | `function(canvas, g)` |
| `luaPanelSaveState` / `luaPanelRestoreState` | persist custom state | `function(tree)` |

**Modulator** (attributes on `<modulator>`):

| Attribute | Fires when | Signature |
|---|---|---|
| `luaModulatorValueChange` | this modulator's value changed | `function(mod, value, source)` |
| `luaModulatorGetValueForMIDI` | value → MIDI value | `function(mod, value)` returns the MIDI value |
| `luaModulatorGetValueFromMIDI` | MIDI value → value | `function(mod, midiValue)` returns the value |

**Component** (attributes on `<component>`): `componentLuaMouseDown` / `componentLuaMouseUp` on
any component, called `f(component, mouseEvent)`; on `uiCustomComponent` the `uiCustom*Callback`
set (`components.md`, "Custom components"); `uiTabsCurrentTabChanged` `(modulator, tabIndex)`;
`uiListBoxItemClicked`; `uiFileListFileClicked`.

## Sending MIDI

```lua
panel:sendMidiMessageNow(CtrlrMidiMessage("B0 4A 7F"))                        -- hex string
panel:sendMidiMessageNow(CtrlrMidiMessage(MidiMessage(0x90, 60, 100, 0.0)))   -- status, d1, d2
panel:sendMidiMessageNow(CtrlrMidiMessage({0xF0, 0x7D, 0x00, 0x12, 0x40, 0xF7}))  -- byte table
```

- **End every SysEx frame with `F7` yourself.** Nothing appends it. The probe reports an
  unterminated frame as sent, but real MIDI drivers drop it, so the control is dead on every host.
- The channel is in the status byte (`0x90` = channel 1); a Lua-built message ignores the
  modulator's channel settings.
- A bare string (`panel:sendMidiMessageNow("B0 4A 7F")`) and the byte-table constructor exist only
  from CtrlrX 5.6.35; the `CtrlrMidiMessage(MidiMessage(...))` form works on every version.

## Reading an incoming message

`luaPanelMidiReceived` gets a `CtrlrMidiMessage`; a SysEx arrives whole, `F0` to `F7`.

```lua
midiIn = function(midi)
    local d = midi:getData()                       -- a MemoryBlock; getByte is 0-based
    local n = math.floor(tonumber(d:getSize()))    -- getSize() is a double
    if n == 6 and d:getByte(0) == 0xF0 and d:getByte(1) == 0x7D then
        panel:getModulatorByName("cutoff"):setModulatorValue(d:getByte(4), false, false, false)
    end
end
```

| Method | For `B2 4A 33` (CC 74 = 51, channel 3) | For a SysEx |
|---|---|---|
| `getMidiMessageType()` (= `getType()`) | `0` (`CtrlrMidiMessage.CC`) | `5` (`CtrlrMidiMessage.SysEx`) |
| `getChannel()` | `3` (1-based) | meaningless |
| `getNumber()` / `getValue()` | `74` / `51` | meaningless; read the bytes |
| `getSize()` | `3` | the frame length |
| `getData()` (= `getLuaData()`) | `MemoryBlock` of the bytes | the whole frame |
| `toString()` | `"b2 4a 33"` (a Lua string) | `"f0 7d … f7"` |

Measured in the probe. The other type constants follow `midiMessageType`
(`midi-and-values.md`). Setting values from a dump: pick the setter from the table in
"CtrlrModulator" so the dump isn't echoed back.

## Timers

```lua
timer:setCallback(11, function(id)
    timer:stopTimer(id)          -- first line for a one-shot: every timer repeats
    ledOff("led-send")
end)
timer:startTimer(11, 120)
```

- `setCallback(id, fn)`, `startTimer(id, ms)`, `stopTimer(id)`, `isTimerRunning(id)`. Every timer
  repeats; a one-shot stops itself. Re-arm with `stopTimer` + `startTimer`.
- **Timer ids are panel-wide.** Reusing an id silently replaces another feature's callback. Search
  the panel's Lua for `setCallback(` and keep all ids in one table.
- A timer callback that raises is disabled (CtrlrX shows a dialog; the probe records a `lua`
  error and the dialog).
- Labels set during a long Lua chunk don't repaint until it returns; defer heavy work to a short
  timer so a "working…" label shows.
- In probe tests, wait for timer output with `waitFor` on a Lua condition, not a fixed `wait`.

## Dialogs

`utils.infoWindow`, `utils.warnWindow`, `utils.questionWindow(title, msg, yes, no)`,
`utils.askForTextInputWindow(...)`, `utils.openFileWindow(title, File(""), patterns, true)`,
`utils.saveFileWindow(...)`, `AlertWindow.showOkCancelBox(...)`.

- A cancelled file dialog returns `File()`: test `f:isValid()`, not `nil`.
- `askForTextInputWindow` returns the string `"-1"` on cancel.
- Under the probe every dialog is dismissed as Cancel: `questionWindow` and `showOkCancelBox`
  return `false`, and the dialog is listed in the step's `dialogs`. Put the work behind a dialog in
  a function taking plain values, so a test can call it directly (`verify-with-probe.md`).

## JUCE `String` userdata

The C++ return type decides what Lua gets:

| C++ returns | Lua gets | Examples |
|---|---|---|
| `String` or `const String&` | a Lua string | `File:getFileName()`, `File:getFullPathName()` |
| `const String` | **`String` userdata** | `mod:getName()`, `mod:getLuaName()`, `label:getText()`, `comp:getPropertyString(p)` |

A userdata is never `==` a Lua string, `tostring()` gives `"String object: 0x…"`, and
**`"" .. s` crashes CtrlrX** (not a catchable error). A `waitFor` that compares one is always
false. Instead:

- identify a modulator by `mod:getVstIndex()` or `mod:getProperty("name")`;
- read component text with `comp:getProperty("uiLabelText")` etc.;
- convert when you must: `s:toUTF8():getAddress()` is a Lua string.

`panel_tool.py lint` warns about `getName()` used in a comparison or concatenation.

## Files and `MemoryBlock`

```lua
local mb = MemoryBlock()
mb:setSize(#bytes, true)                               -- setSize needs both arguments
for i = 1, #bytes do mb:setByte(i - 1, bytes[i]) end    -- 0-based
local f = File(path)
if not f:replaceWithData(mb) then console("write failed: " .. path) end

local inMb = MemoryBlock()                             -- fresh: loadFileAsData appends
if f:loadFileAsData(inMb) then
    local n = math.floor(tonumber(inMb:getSize()))     -- getSize() is a double
    for i = 1, n do bytes[i] = inMb:getByte(i - 1) end
end
```

- `MemoryBlock(n)` gives an **empty** block (the numeric constructor isn't bound). Use
  `MemoryBlock()` + `setSize(n, true)`, `MemoryBlock(n, true)`, or `MemoryBlock({b1, b2, …})`.
- `f:loadFileAsData(mb)` **appends** to `mb`: always pass a fresh, unsized block.
- `getByte` / `setByte` are 0-based; `setByte(n, v)` on an `n`-byte block writes past the end.
- `MemoryBlock:loadFromHexString(s)` reads every hex-looking letter as a digit: no labels or
  comments inside the string.
- `File(path)` needs an absolute path; `File("")` is the safe "no initial folder" for a dialog.
- Resources (`resources:getResource(name)`) exist only in a `.bpanelz`; a `.panel` export drops
  their data, so guard a `nil` result loudly.

## Painting a `uiCustomComponent`

```lua
gridPaint = function(comp, g)
    local w, h = comp:getWidth(), comp:getHeight()
    g:fillAll(Colour(0xff202020))
    g:setColour(Colour(0xff2060a0))
    g:fillRect(math.floor(w / 4), 0, math.floor(w / 4), h)     -- integers only
    g:drawFittedText("Step", 0, 0, w, h, Justification(Justification.centred), 1, 1.0)
end
```

- `fillRect` / `drawRect` are bound with integer arguments only, `drawLine` with floats only.
  Floor every rectangle argument. `drawRect` needs the line width too: `g:drawRect(x, y, w, h, 1)`;
  the four-argument call raises "No matching overload".
- `comp:repaint()` paints nothing under the probe; test painting with the probe's `paint` action
  (`verify-with-probe.md`).
- Mouse callbacks: `components.md`, "Custom components".

## Version floor

The probe and the CtrlrX source you read may be newer than the user's installed CtrlrX. Bindings
added later (the string / table MIDI forms above, `CtrlrMidiMessage({…})`) then fail with "No
matching overload" in the user's app while every probe run passes. When the user names a version,
check the binding at that release tag (`git show <tag>:Source/Lua/CtrlrLuaManager.cpp` in a clone,
or `https://raw.githubusercontent.com/damiensellier/CtrlrX/<tag>/Source/Lua/CtrlrLuaManager.cpp`) and avoid
copying components from a newer panel, which bring newer properties.

## A complete small example

A combo `lfoShape` updates a label `lfoLabel`. Method body before escaping:

```lua
lfoShapeChanged = function(mod, value, source)
    local names = { [0] = "Sine", [1] = "Triangle", [2] = "Saw", [3] = "Square" }
    local lbl = panel:getLabel("lfoLabel")
    if lbl then lbl:setText(names[value] or "?") end
end
```

Store it as `<luaMethod luaMethodName="lfoShapeChanged" luaMethodCode="…escaped…">` under
`<luaManagerMethods>`, set `luaModulatorValueChange="lfoShapeChanged"` on `lfoShape`, and call it
once from `luaPanelLoaded` so the label is right before the first change.
