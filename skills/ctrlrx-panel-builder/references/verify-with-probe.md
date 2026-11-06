# Verifying a panel with CtrlrPanelProbe

`CtrlrPanelProbe` loads a panel headlessly, rebinds its MIDI to mock devices, runs a JSON script
of actions and prints a JSON report: the MIDI each step sent, every error, Lua return values, and a
final snapshot of every control. It runs CtrlrX's own code, so it shows what CtrlrX would do. This
page is the working guide; the full schema is `Doc/PanelProbe.md` in the CtrlrX repository
(https://github.com/damiensellier/CtrlrX/blob/master/Doc/PanelProbe.md).

## Contents

- Getting the probe
- The loop: script, run, assert
- Actions
- Reading a report
- Traps when reading MIDI and state
- Dialogs, timers and asynchronous output
- Custom components: paint, mouse, screenshots and pixels
- Edit mode
- Checks that can fail
- Making panel Lua testable
- Running many scripts

## Getting the probe

`python3 scripts/probe_harness.py where` prints the binary the harness will use (how it is
found, and how to get one without a clone: `SKILL.md`, "Tools"). In a CtrlrX clone, build it:
`cmake -B build . && cmake --build build --target CtrlrPanelProbe`. CI also publishes it as a
build artifact (`Doc/PanelProbe.md`, "Getting the binary"). A Release build is faster and is what
wall-clock assertions should use.

## The loop: script, run, assert

```bash
cat > check.json <<'EOF'
{ "steps": [
  { "action": "setValue", "modulator": "cutoff", "value": 100 },
  { "action": "click",    "modulator": "sendPatch" },
  { "action": "eval",     "lua": "return panel:getModulatorByName('cutoff'):getValue()" }
] }
EOF
python3 scripts/probe_harness.py run synth.panel check.json
```

The harness prints a digest per step (action, `deviceMidiOut`, `hostMidiOut`, `returnValue`,
errors, dialogs, console) and the `errorCount`; exit 0 clean, 1 errors, 2 fatal. `--report`
prints the full JSON for `jq`. `--steps '[…]'` takes the steps inline.

From Python (pytest or a script), put `scripts/` on `sys.path`:

```python
from probe_harness import run_script, set_value, click, evaluate, sent, values
r = run_script("synth.panel", [set_value("cutoff", 100), click("sendPatch")])  # raises unless errorCount == 0
assert sent(r["steps"][0]) == [bytes.fromhex("b04a64")]
assert values(r)["cutoff"] == 100
```

Then: assert → change the panel → re-run the same script, until it passes.

## Actions

| Action | Required | Does |
|---|---|---|
| `setValue` | `modulator`, `value` | Moves the control like a user (GUI path): MIDI is sent and `luaModulatorValueChange` runs (`source` 4). `value` in the modulator's own units. |
| `click` | `modulator` | Mouse down + up at the centre: buttons trigger, `componentLuaMouseDown/Up` fire (on a `uiCustomComponent` only its `uiCustomMouse*Callback`s). |
| `injectDeviceMidi` | `bytes` | MIDI arriving from the device: input matching + `luaPanelMidiReceived`. |
| `injectHostMidi` | `bytes` | MIDI arriving from the host. |
| `callLuaMethod` | `method` (`args`: a JSON array) | Calls a panel method; `returnValue` is its result. |
| `eval` | `lua` | Runs a chunk in the panel's own Lua state; `returnValue`. Tried as `return <chunk>` first. |
| `waitFor` | `lua` (`timeoutMs`, `pollMs`) | Polls a Lua condition until truthy; a timeout is an `action` error. |
| `wait` | (`ms`) | Waits exactly `ms`, no early exit. |
| `screenshot` | `file` | Renders the canvas to a PNG. |
| `paint` | `modulator` (`file`) | Paints one component now, counts it, optionally writes its PNG. |
| `mouse` | `modulator`, `gesture` (`x`, `y`, `steps`, `button`, `mods`, `holdMs`, `dx`, `dy`) | Mouse events on a `uiCustomComponent` at a position: `down`, `drag`, `up`, `click`, `doubleClick`, `move`, `wheel`. |
| `setLayerVisible` | `layer`, `visible` | Shows or hides a canvas layer (by uid, then name). |

- Every action settles afterwards: it waits until nothing happened for `quietMs` (default 30),
  at most `settleMs` (default 300). Both can be set script-wide or per step. `settleMs` is a
  ceiling, not a floor.
- `bytes` is hex, space- or comma-separated (`"F0 42 30 F7"`).
- In JSON, write Lua strings with single quotes or escape the double quotes.
- An unknown action is an `action` error; the script continues.

## Reading a report

| Key | What it tells you |
|---|---|
| `summary.errorCount` | **The single number to assert on**: action errors + Lua runtime errors + compile errors. |
| `panel.luaCompileErrors` | Methods that failed to compile at load (under `panel`, not the report root). |
| `steps[i].errors` | `{type: "action" \| "lua", message}` for that step. |
| `steps[i].deviceMidiOut` / `hostMidiOut` | Lowercase hex messages sent during the step (`"b0 4a 64"`). |
| `steps[i].returnValue` | From `callLuaMethod` / `eval` / `waitFor`; `null` for `nil`. |
| `steps[i].console` | That step's `console()` lines. |
| `steps[i].dialogs`, `panel.dialogs` | Dialogs dismissed during the step / during load. |
| `finalState.modulators[]` | Per control after the last step: `value`, `min`, `max`, `text` (the readout), `visible`, `liveVisible`, `effectiveVisible`, `componentRectangle`, `canvasRectangle`, `vstIndex`. |
| `finalState.overlaps` | Pairs of visible controls whose rectangles intersect. |
| `warnings[]` | Things that are not failures: `paint`, `mouse`, `editMode`. |
| `summary.disabledMethods` | Methods invalid now (compile error or disabled by a runtime error). |

- Exit code: 0 clean, 1 `errorCount > 0`, 2 fatal (panel or script missing / unreadable). A fatal
  report has `error` and `detail` and no `summary`; the harness raises on it.
- A panel whose XML doesn't parse is fatal (exit 2); `detail` gives the parser's reason
  (`unmatched tags`, `expected '=' after attribute …`). Older probe builds loaded a file that ends
  inside its root tag as an empty panel with exit 0, so the harness also refuses a report with 0
  modulators unless `--allow-empty`. Lint first either way: it names the line.
- `finalState` is taken once, after the last step. To see a value mid-script, `eval` it.
- Assert visibility on `effectiveVisible`. `visible` is only the property; a control on a hidden
  layer or in a hidden group still has `visible: true`.
- A missing `canvasRectangle` means the component has no parent: its `componentLayerUid` names no
  layer, and it is never drawn.

## Traps when reading MIDI and state

- **No MIDI is not proof of dead wiring.** `setValue` to the value a control already holds sends
  nothing and runs no callback. Set a different value first (and ignore that step).
- **A `uiCombo` sends twice under `setValue`** (a user's click sends once), either interleaved
  (`99 99 98 98 06 06 26 26`) or as two whole bursts. Fold it with `fold_double_send()` from
  `probe_harness.py`; two *different* writes survive the fold.
- **`setValue` bypasses the component's range**: a slider limited to 0..127 still sends 16383 when
  the script sets it. When the range is under test, `eval` back `getMaxModulatorValue()` and the
  component's `uiSliderMax`.
- **Inbound MIDI matches on the panel channel** (`panelMidiOutputChannelDevice`) unless the
  modulator has `midiMessageChannelOverride="1"`; inject on the channel the control really uses.
- **Device → host forwarding lags a step**: an injected message may show in the *next* step's
  `hostMidiOut`. Add a `wait` step if you assert on it.
- **`eval` return values**: numbers come back as floats (`2.0`); a table is a JSON array only when
  its keys are exactly `1..n`, otherwise an object with **string** keys (`{[0]=…}` → `{"0": …}`).
- **`String` userdata** (`getName()`, `getText()`) comes back as `"<userdata>"`; return
  `getProperty(...)` instead.
- **SysEx frames**: assert every `f0` message ends in `f7` (`unterminated_sysex()` in the harness);
  the probe records frames a real driver would drop.

## Dialogs, timers and asynchronous output

- Every modal dialog is dismissed as **Cancel** and listed in `dialogs` with its title and
  buttons (`kind`: `alert`, `fileChooser`, `component`). Code after it runs with that answer:
  `utils.questionWindow` and `AlertWindow.showOkCancelBox` return `false`. The message text is not
  reported. A dialog is not an error by itself.
- A `timer` callback that raises is disabled by CtrlrX; the probe reports it as a `lua` error on
  the step plus a "Timer callback error" dialog.
- Value-change callbacks, timer output and Lua-sent bursts arrive after the triggering action
  returns. Assert them with `waitFor` on a condition the panel sets (`"lua": "return dumpDone"`),
  not with a guessed `wait`: a floor that passes today flakes on a loaded machine.
- A method disabled by a runtime error stays disabled for the rest of the process. In `--stdin`
  runs assert `summary.disabledMethods == []` in every report.

## Custom components: paint, mouse, screenshots and pixels

- **`repaint()` paints nothing under the probe**, and nothing fails. Test paint code with the
  `paint` action (one component) or `screenshot` (the canvas). Assert
  `finalState.modulators[i].paint.paints >= 1` **and** `paintMethodValid`: a paint callback
  disabled by an earlier error is skipped silently.
- A component invalidated and never painted gets a `paint` warning at the end of the script. A
  script that doesn't test paint can set `"paintWarnings": false` at the top level.
- **`mouse`** drives a `uiCustomComponent` at component-relative positions:
  `{"action": "mouse", "modulator": "grid", "gesture": "click", "x": 40, "y": 12}`;
  `drag` with `steps` interpolates; `down` … `drag` … `up` holds a press across steps (only `drag`
  and `up` are allowed while it is held); `button: "right"` for a right-click; `holdMs` advances the
  press time (a long press is more than 300 ms). The step reports the delivered events
  (`"down@40,12"`).
- `mouse` doesn't hit-test (it reaches a component even if something covers it; check
  `finalState.overlaps`) and doesn't simulate drag-and-drop (a `mouse` warning says so). Use `click`
  for buttons and `setValue` for value controls.
- **Pixels.** Take a `screenshot` (or `paint` with `file`) and check a solid area away from text
  and edges: `python3 scripts/png_sample.py shot.png 120 40 --expect 2060a0` (exit 1 on a
  mismatch). Never compare whole images: fonts and anti-aliasing differ per platform. A component
  on a hidden layer must be made visible (`setLayerVisible`) before a screenshot shows it.

## Edit mode

The probe runs every panel in run mode, as a user plays it, whatever mode the file was saved in.
A file saved with `uiPanelEditMode="1"` (or without the attribute) adds an `editMode` warning, and
`panel.savedEditMode` is `true`. `--edit-mode-as-saved` reproduces what CtrlrX shows when it opens
such a file: controls hidden after load are only dimmed (they stay `effectiveVisible`), and the
canvas shows the snap grid. Save panels with `uiPanelEditMode="0"` and the difference disappears.

## Checks that can fail

A check that has never failed proves nothing. For every probe check you write:

- [ ] **Run it red once**: against the unfixed panel, or a copy with the behaviour broken (wrong
      CC, callback removed). It must fail for the reason you expect.
- [ ] **Pair every negative with a positive control** in the same run: "this injected CC did not
      move the knob" needs "this other CC did move it", or a broken setup looks the same.
- [ ] **Don't let a round trip cancel itself**: a parser and a builder written from the same wrong
      assumption agree with each other. Compare against independent bytes (a real dump, the
      device's manual) or a second implementation.
- [ ] **Diff `finalState` before and after any scripted bulk edit** of the panel file: lint,
      `summary` and `errorCount 0` don't see a value, range or visibility that changed.
- [ ] **Fail loudly when the probe is missing**: a suite that skips itself when the binary isn't
      found reports green for nothing.

## Making panel Lua testable

- **Split dialogs from work.** Anything behind a dialog cannot be answered under the probe, so put
  the dialog in a thin wrapper and the work in a function of plain values:

  ```lua
  savePresetTo = function(path)                 -- worker: testable, returns a status
      ...
      return "ok " .. #frame
  end
  savePreset = function()                       -- wrapper: dialog only
      local f = utils.saveFileWindow("Save", File(""), "*.syx", true)
      if f:isValid() then savePresetTo(f:getFullPathName()) end
  end
  ```
  Drive the worker with `callLuaMethod` (`"args": ["/tmp/x.syx"]`) and a temp path.
- **Stub from `eval`, don't edit the panel**: replace a guard or a dialog for one script, e.g.
  `{"action": "eval", "lua": "AlertWindow.showOkCancelBox = function() return true end"}`.
- **Keep codecs and view logic in plain Lua functions** with CtrlrX calls in thin wrappers. They
  can then also run under a bare `lua5.1` interpreter, which is far faster than the probe for big
  tables.
- **Drawing surfaces**: hit-testing and editing as functions over numbers, called by the
  `uiCustom*` callbacks; test the functions with `eval` and the wiring with one `mouse` step.
- **Read internals with `eval`** (`"return codec.get(img, 'filters.speed')"`); no need to add a
  test-only method to the panel.

## Running many scripts

- `probe_harness.py batch PANEL scripts.jsonl` (or `PersistentProbe` from Python) runs one
  `--stdin` process: one script per line, one report each, ~1.5 s startup paid once. All scripts
  **share panel state** (values, Lua globals, disabled methods); there is no reload. Seed what you
  assert on, or use a fresh process for tests that need a pristine panel.
- In `--stdin` mode `panel.luaCompileErrors` repeats in every report, so a panel with one broken
  method never reports `errorCount == 0`.
- Put a timeout on every probe run (the harness defaults to 120 s): a panel that re-sets a combo
  from its own callback hangs. When cleaning up, stop only the process you started, never with a
  broad `pkill`.
- A Debug probe appends to `~/Desktop/CtrlrX_DBG.log` on every run; long suites should use a
  Release build.
- No imaging library is needed: `png_sample.py` reads the probe's PNGs with the standard library.
