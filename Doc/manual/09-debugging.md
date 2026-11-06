[← 08 Sending & Receiving](08-sending-receiving.md) | [Index](README.md) | Next: [10 — Distribution & Advanced →](10-distribution.md)

---

# 09 — Debugging

> **TL;DR**
> - **MIDI Monitor** (Tools menu) shows every byte in/out — your first stop for "is it sending?".
> - **Lua console** (Panel → LUA Console, or the LUA Editor's **Console** tab) runs Lua live and prints `console(...)` output.
> - `how()` lists all classes bound to Lua; `what(obj)` lists the methods of one object.
> - Most "it doesn't work" cases are: wrong mode, unassigned Lua method, or a MIDI device not selected.
> - **CtrlrPanelProbe** checks a panel's behaviour from a script, with no hardware and no window.

## Contents

- [Tool 1 — the MIDI Monitor](#tool-1--the-midi-monitor)
- [Tool 2 — the Lua console](#tool-2--the-lua-console)
- [Tool 3 — introspection: `how()` and `what()`](#tool-3--introspection-how-and-what)
- [Reading Lua errors](#reading-lua-errors)
- [Testing a panel without hardware](#testing-a-panel-without-hardware)
- [The usual suspects — a checklist](#the-usual-suspects--a-checklist)

---

## Tool 1 — the MIDI Monitor

Covered in [Chapter 6](06-midi-basics.md), it's worth repeating: **Tools → MIDI Monitor** is the
ground truth for MIDI. If a control "isn't working", check here first:

- **Nothing appears when you move a control** → it has no MIDI mapping, you're in Edit mode, or no
  Output device is selected.
- **Bytes appear but the synth ignores them** → wrong channel, wrong CC/number, or the synth expects
  SysEx/NRPN, not CC.
- **Incoming (green) messages don't move your control** → the control's receive mapping doesn't match
  (type/channel/number), or no Input device is selected.

## Tool 2 — the Lua console

The console runs Lua statements immediately against the loaded panel — perfect for poking at things.

**Steps**
1. Open it via **Panel → LUA Console**, or the **Console** tab at the bottom of the LUA Editor.
2. Type a statement and run it:
   ```lua
   panel:getModulatorByName("filterCutoff"):getValue()
   ```
3. Use `console(...)` (or `print(...)`) inside methods to log while a panel runs.

![The Lua console](images/lua-console.png)

> 💡 Tip: `console(string.format("cutoff=%d", v))` formats output. `%d` integer, `%f` float, `%s`
> string. Lua converts on the fly but you tell it the format.

## Tool 3 — introspection: `how()` and `what()`

You don't have to memorize the API — ask the running app:

```lua
how()      -- prints every class bound to Lua (Ctrlr + JUCE)

mod = panel:getModulatorByName("filterCutoff")
what(mod)               -- prints all methods available on the modulator
what(mod:getComponent())-- prints all methods on its component
```

This is the most reliable reference of all, because it reflects *exactly* what your build exposes.

> 🔗 Deeper: a curated, readable version of the same surface is the
> [Lua reference](lua/02-lua-reference.md).

## Reading Lua errors

When a method fails to compile or run, the **Output** pane (bottom of the Lua editor) shows the error
with a line number, formatted like:

```
ERROR: [string "myMethod"]:42: 'end' expected (to close 'function' at line 3)
```

The number **between the colons** (`42`) is the line in your method. Common causes:

| Symptom | Likely cause |
|---|---|
| `'end' expected` | A missing `end` for an `if`/`for`/`function`. |
| `attempt to index a nil value` | `getModulatorByName(...)` returned `nil` — check the name (case-sensitive!). |
| `'<eof>' expected` / unfinished long comment | A `--[[` block comment never closed with `]]`. |
| Method never runs | It isn't assigned in the callback dropdown ([Chapter 7](07-making-responsive.md)). |

## Testing a panel without hardware

`CtrlrPanelProbe` is a small command-line tool built next to the test suite. It loads a panel, runs
a script of actions against it (move a control, click a button, feed in MIDI as if it came from the
synth, call a Lua method) and prints a JSON report: the MIDI bytes the panel sent, any Lua errors,
and the final value of every modulator. No synth, MIDI interface or open window is needed, so you
can check a panel in a script or in CI, and re-check it after every edit.

```bash
cat > check.json <<'EOF'
{ "steps": [ { "action": "setValue", "modulator": "filterCutoff", "value": 64 } ] }
EOF
CtrlrPanelProbe MySynth.panel check.json | jq '.steps[0].deviceMidiOut, .summary.errorCount'
```

If the first line prints `[]`, moving that control sends nothing and its MIDI wiring is dead.

> 🔗 Deeper: [CtrlrPanelProbe](../PanelProbe.md) covers how to build or download it, every action,
> and the full report format.

## The usual suspects — a checklist

When something doesn't work, walk this list:

1. **Right mode?** Editing needs Edit mode; using/testing needs Panel mode (**Ctrl/Cmd+E**).
2. **MIDI devices selected?** Output for sending, Input for receiving (**MIDI** menu).
3. **Mapping correct?** Type, channel, and number match the device's MIDI implementation.
4. **Lua method assigned?** Created *and* selected in the callback dropdown.
5. **Names exact?** `getModulatorByName` is case-sensitive and must match the modulator's **Name**.
6. **Value range sane?** Max value matches the parameter's resolution (127 / 1 / 16383).
7. **Feedback loop?** If receiving MIDI re-sends MIDI, guard with the `source` argument
   ([Chapter 8](08-sending-receiving.md)).

> ⚠️ Gotcha: After reorganizing methods into folders in the Lua editor, you may need to **save and
> reopen the panel** to see the new structure.

> 🔗 Deeper: Find more info, FAQ and examples in the
> [CtrlrX wiki](https://github.com/damiensellier/CtrlrX/wiki).

---

[← 08 Sending & Receiving](08-sending-receiving.md) | [Index](README.md) | Next: [10 — Distribution & Advanced →](10-distribution.md)
