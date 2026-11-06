# Component (`uiType`) catalogue

`<component uiType="…">` selects the widget. **Static** components hold no value and send no MIDI
(`modulatorIsStatic="1"`, `<midi midiMessageType="9">`). Copy a same-`uiType` `<component>` from a
real panel (`panel_tool.py dump FILE modulator:NAME`, or `Tests/fixtures/fixture_*.panel` in the
CtrlrX repo): each type carries its own block of look attributes. All types and their properties:
`Source/UIComponents/CtrlrComponents/`.

## Contents

- Quick chooser
- Sliders: ranges, clamping, per-value text
- Buttons: triggers, gates, one method for many buttons
- Combos and value maps
- Labels and statics
- Groups, tabs and layers
- Custom components (paint and mouse)
- Other components

## Quick chooser

| Need | `uiType` |
|---|---|
| on/off | `uiToggleButton` (`uiImageButton` for custom art) |
| continuous value | `uiSlider` (`uiImageSlider` for a filmstrip knob) |
| a few discrete steps, or per-value text on the knob | `uiFixedSlider` / `uiCombo` |
| named choices | `uiCombo` |
| a trigger ("Send patch") | `uiButton` + `componentLuaMouseUp` |
| caption / readout / indicator LED | `uiLabel` / `uiLCDLabel` |
| container / sections | `uiGroup` / `uiTabs` |
| drawn by Lua (grid, envelope, meter) | `uiCustomComponent` |
| play notes | `uiMidiKeyboard` |
| list of patches / files | `uiListBox` / `uiFileListBox` |

## Sliders: ranges, clamping, per-value text

| `uiType` | What it is |
|---|---|
| `uiSlider` | Knob or fader drawn by CtrlrX; style via `uiSliderStyle`. |
| `uiImageSlider` | Drawn from a filmstrip image (one frame per value; a 0–127 knob needs 128 frames). |
| `uiFixedSlider` | Snaps to a value map's entries and shows their text. |
| `uiFixedImageSlider` | The same with a filmstrip. |

`uiSliderStyle` (strings): `LinearHorizontal`, `LinearVertical`, `LinearBar`,
`LinearBarVertical`, `Rotary`, `RotaryHorizontalDrag`, `RotaryVerticalDrag`,
`RotaryHorizontalVerticalDrag`, `TwoValueHorizontal`/`Vertical`, `ThreeValueHorizontal`/`Vertical`,
`IncDecButtons`.

- **Set the range twice.** The visible and clamped range is the component's `uiSliderMin` /
  `uiSliderMax`, not `modulatorMin` / `modulatorMax`; with only the modulator range set, the knob
  stays at the default `0..127`. A ±2400 knob needs both pairs at `-2400` / `2400`. At runtime the
  component range wins: changing `uiSliderMax` from Lua also rewrites `modulatorMax`
  (`CtrlrSlider::valueTreePropertyChanged`).
- **Narrowing a range does not clamp the value.** After
  `comp:setPropertyString("uiSliderMax", "920")` a modulator at 1023 reads value 1023, max 920.
  Clamp in the same Lua, choosing the setter from the table in `SKILL.md` rule 3:

  ```lua
  local ceiling = 920
  local mod = panel:getModulatorByName("cutoff")
  panel:getComponent("cutoff"):setPropertyString("uiSliderMax", tostring(ceiling))
  if mod:getModulatorValue() > ceiling then
      -- no MIDI to the device; false as the last argument keeps the value-change callback,
      -- true skips it (use true only when nothing depends on this control)
      mod:setModulatorValue(ceiling, false, false, false)
  end
  ```
- **Per-value text on a knob needs `uiFixedSlider`.** A `uiSlider` only shows
  `number + uiSliderValueSuffix`; value expressions never change the display. Put the entries in
  `uiFixedSliderContent`, one `TEXT=raw` line each (`221 Hz=42`), in ascending raw order, and keep
  `uiSliderStyle="Rotary"` for a knob. The readout is `finalState.modulators[].text` in the probe.
- **A `uiFixedSlider` holds the entry's ordinal, not the device value.** They coincide only when
  the device values run `0..N` without gaps. For a signed range (±12 dB as −120..120) or an
  out-of-band sentinel (255 = "Off" after 0..103), keep an `ordinal → raw` table and translate in
  both directions; append a sentinel as one extra entry past the top. Assert on the bytes sent,
  not on `finalState…value` (the ordinal).

## Buttons: triggers, gates, one method for many buttons

| `uiType` | What it is |
|---|---|
| `uiToggleButton` | On/off button (`modulatorMax="1"`). |
| `uiButton` | Text button, momentary or toggle. |
| `uiImageButton` | Button drawn from filmstrip frames. |
| `uiHyperlink` | Opens a URL. |

- **The one-line `uiButtonContent` trap.** A `uiButton`'s value steps through the lines of
  `uiButtonContent`. With one line (a trigger labelled "Send patch") the value is always 0: the
  value-change callback never fires, and a NoteOn button sends velocity 0. Two lines alternate
  0/1, so the callback fires on every other click. `panel_tool.py lint` warns about one-line
  content.
- **Wire a trigger to `componentLuaMouseUp`** (on `<component>`, set to a method name). It fires
  on every click, independent of the value. The method is called as `f(component, mouseEvent)`:
  the first argument is the **component**; use `component:getOwner()` for the modulator.
  `uiCustomMouseUpCallback` is a different property that exists only on `uiCustomComponent`; a
  `uiButton` ignores it.
- **Wire one hook, not both.** Mouse-up plus `luaModulatorValueChange` on the same method runs it
  twice on some clicks, which cancels a toggle. If the button's own `<midi>` already sends the
  message, the Lua must not send it again; probe one `click` and count the frames.
- **Gates.** `componentLuaMouseDown` is the pair: NoteOn on press, NoteOff on release, with
  `midiMessageType="9"` so only Lua sends:

  ```lua
  gateDown = function(comp, e) panel:sendMidiMessageNow(CtrlrMidiMessage("9F 36 64")) end
  gateUp   = function(comp, e) panel:sendMidiMessageNow(CtrlrMidiMessage("8F 36 00")) end
  ```
  The probe's `click` drives both (`9f 36 64` then `8f 36 00`).
- **One method for many buttons.** Park per-control data in `modulatorCustomIndex` (a Text
  property) and share one callback; `tonumber()` it before arithmetic:

  ```lua
  noteButton = function(comp, e)
      local n = tonumber(comp:getOwner():getProperty("modulatorCustomIndex"))
      panel:sendMidiMessageNow(CtrlrMidiMessage(string.format("9F %02X 64", n)))
  end
  ```
  Never dispatch on `getName()` (a `String` userdata, `lua-api.md`).

## Combos and value maps

- **The value is the 0-based index** of the entry; `uiComboContent` lines become entries
  (`&#10;` between them in the XML). The value map sets the range (`modulatorMax = entries - 1`).
- **Device values that are not `0..N`:** either use `Label=raw` entries, or keep the combo
  index-based and translate index → raw in the value-change Lua from a table. Probe one entry and
  check which number reaches the MIDI and the callback before relying on either.
- **Never leave a blank line** in the content: blank and `__`-prefixed lines are skipped and shift
  every entry below. Use a visible placeholder label without `=`.
- **Never re-set a combo from inside its own `luaModulatorValueChange`**: CtrlrX re-enters
  `setComponentValue` and hangs. Skip the set when `getModulatorValue()` already equals the target.
- **Setting the index a combo already holds does nothing** (no callback). To re-run a selector's
  side effects in a test, set a different value first.
- **Replacing `uiComboContent` at runtime** leaves the combo showing nothing (JUCE index −1) while
  the modulator keeps its old value, and does not re-derive `modulatorMax`. Re-show the item with
  `panel:getComboComponent(name):setSelectedItemIndex(i, true)` (`true` = don't notify) and set
  the range yourself.
- **Reading back:** `comp:getProperty("uiComboContent")` is a Lua string.
  `uiComboSelectedIndex` / `uiComboSelectedId` are never written (always −1); ask the component:
  `panel:getComboComponent(name):getSelectedItemIndex()`.
- Under the probe's `setValue` a combo sends its MIDI twice (`verify-with-probe.md`); a user's
  click sends once.

## Labels and statics

| `uiType` | What it is |
|---|---|
| `uiLabel` | Text label (fixed text or a readout). |
| `uiLCDLabel` | LCD-style display. |
| `uiImage` | Static image. |
| `uiArrow` | Drawn arrow. |
| `uiProgressBar` | Value as a filled bar. |

- **`uiLabelOutline` is a text inset**, not a frame: `uiLabelOutline="4"` pads the text by 4 px on
  every side, so the label needs height ≥ `2 × inset + font height`.
- **`uiLabelOutlineColour` always draws a 1 px frame**, whatever `uiLabelOutline` says. For no
  frame use a transparent colour `"00000000"`.
- **The background is `uiLabelBgColour`** (not `uiLabelBackgroundColour`) and applies live, so a
  small label with empty `uiLabelText` is the cheapest indicator LED:
  `comp:setProperty("uiLabelBgColour", "ffff3000", false)`; decay it with a self-stopping timer.
- Set label text from Lua with `panel:getLabel(name):setText(s)`; read it with
  `comp:getProperty("uiLabelText")` (a Lua string), not `getText()` (userdata).
- Text outside ASCII may render as empty boxes in the default font.

## Groups, tabs and layers

| `uiType` | What it is |
|---|---|
| `uiGroup` | Container; members move with it. |
| `uiTabs` | Tabbed pages; `uiTabsCurrentTabChanged` is called `(modulator, tabIndex)`. |

Every component sits on a canvas layer: its `componentLayerUid` must equal a
`uiPanelCanvasLayerUid` in `<uiPanelEditor>`, or the component is never shown (no error). Layers
stack by `uiPanelCanvasLayerIndex` (higher on top) and pass clicks through, so a layer used as a
page needs its own opaque group. `componentDisabled` disables the component's children, so check
`comp:getChildComponent(0):isEnabled()` when asserting it. A combo saved hidden comes back visible
on load; re-apply visibility from `luaPanelLoaded`.

## Custom components (paint and mouse)

`uiCustomComponent` is a blank area drawn and driven by Lua:

- Paint: `uiCustomPaintCallback` → `f(comp, g)`. `fillRect` / `drawRect` exist only with integer
  arguments, `drawLine` only with floats, so `math.floor` every rectangle argument;
  `drawRect(x, y, w, h, lineWidth)` takes all five.
- Mouse: `uiCustomMouseDownCallback`, `…UpCallback`, `…DragCallback`, `…MoveCallback`,
  `…DoubleClickCallback`, `…WheelMoveCallback` → `f(comp, mouseEvent)` with component-relative
  `e.x` / `e.y`. Wire every callback the interaction needs; an unwired drag fails silently.
- Keep `uiCustomDragAndDropContainer="0"` for a surface you paint by dragging: with `1`, the
  drag-and-drop start callback takes over `mouseDrag`.
- Right-click arrives as modifier 32 on some hosts and 34 on others: test the button before the
  hit test. Floor coordinates for hit tests; round only for value snapping.
- Put hit-testing and editing in plain functions over numbers, called by thin callbacks; then a
  test can drive them with `eval` or the probe's `mouse` action (`verify-with-probe.md`).
- Ctrl/Alt + wheel zooms the whole panel; Lua cannot stop it.

## Other components

| `uiType` | What it is |
|---|---|
| `uiMidiKeyboard` | On-screen keyboard. |
| `uiWaveform` | Audio waveform with zoom/seek. |
| `uiXYSurface` | 2-D pad for two parameters. |
| `uiListBox` | List rendered via Lua (`uiListBoxItemClicked`). |
| `uiFileListBox` | File browser bound to a folder (`uiFileListFileClicked`). |
