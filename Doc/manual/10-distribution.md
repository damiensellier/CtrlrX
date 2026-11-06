[← 09 Debugging](09-debugging.md) | [Index](README.md) | [Lua Guide](lua/01-lua-guide.md)

---

# 10 — Distribution & Advanced Features

> **TL;DR**
> - Save as `.panel` (XML) while working; share as **`.bpanelz`** (compressed binary + resources) — smallest and self-contained.
> - **Snapshots/programs** capture all control values as a patch; a **librarian** manages many patches.
> - Used as a **VST/AU plugin**, controls with a **VST index** become host-automatable parameters.
> - **Global variables** and **state callbacks** persist data with the panel.

## Contents

- [Saving and export formats](#saving-and-export-formats)
- [Snapshots, programs, and a librarian](#snapshots-programs-and-a-librarian)
- [Running as a plugin & host automation](#running-as-a-plugin--host-automation)
- [Global variables & persistent state](#global-variables--persistent-state)
- [Troubleshooting & platform notes](#troubleshooting--platform-notes)
- [Where to go from here](#where-to-go-from-here)

---

## Saving and export formats

CtrlrX can write your panel several ways (**File → Save** and **File → Export**):

| Format | Extension | When to use |
|---|---|---|
| XML | `.panel` | While building — human-readable, diff-friendly, version-controllable. |
| Compressed XML | `.panelz` | Smaller XML, still just the panel definition. |
| Binary | `.bpanel` | Binary form of the panel. |
| Binary compressed + resources | `.bpanelz` | **Sharing** — smallest, loads fast, bundles images/fonts. |

> 💡 Tip: Distribute finished panels as **`.bpanelz`** so recipients get one self-contained file with
> all artwork included. Keep the `.panel` (XML) in version control for yourself.

> ⚠️ Gotcha: Use **File → Save versioned** during development to keep numbered backups. Panel building
> is fiddly; a recoverable history saves grief.

## Snapshots, programs, and a librarian

A **snapshot** (a.k.a. program/patch) is the set of all your controls' current values. The
**Programs** menu lets you capture and recall them.

A **librarian** is a panel feature you build to manage many patches — typically with a `uiListBox` of
patch names and Lua to send/receive SysEx bulk dumps. A common workflow for hardware:

1. Request or receive a bank/patch dump from the device (SysEx) → store it.
2. Show patches in a list; on selection, send that patch's SysEx to the device.
3. Save/load banks to `.syx` files on disk (via `uiFileListBox` and Lua file I/O).

Each stored patch is naturally a **`MemoryBlock`** — the device's raw SysEx image. Rather than
scattering the values across modulators, many librarians keep each patch as one block and mirror it
onto the controls only when it's selected. That "keep the whole patch as an image, edit part of it"
approach is [Keeping a patch image (shadow state)](08-sending-receiving.md#keeping-a-patch-image-shadow-state)
in Chapter 8.

> 🔗 Deeper: the building blocks are `uiListBox`/`uiFileListBox` ([Chapter 4](04-gui-elements.md)),
> SysEx send/receive and the patch-image pattern ([Chapter 8](08-sending-receiving.md)), and Lua
> `File`/`MemoryBlock` APIs ([Lua reference](lua/02-lua-reference.md#memoryblock)). The
> `panel:getProgramState()` / `setProgramState()` methods help capture/restore values.

## Running as a plugin & host automation

Load the panel into the CtrlrX **plugin** (VST3/AU) in your DAW and its controls can be **automated**
and saved with your project. Install the plugin into your OS's standard plugin folder first — see the
per-platform locations in [Chapter 1](01-getting-started.md#download-a-release-build-recommended).

For a control to appear as an automatable parameter to the host, give it a **VST index** (in the
*Modulator* property group). The host then sees that parameter; in Lua you can read it with
`modulator:getVstIndex()`.

> ⚠️ Gotcha: VST indices should be unique and stable. Changing them after a project references them
> will break saved automation.

## Global variables & persistent state

Two mechanisms let a panel keep data:

- **Global variables** — a panel-wide array of integers, handy for cross-method state:
  ```lua
  panel:setGlobalVariable(0, 42)
  local v = panel:getGlobalVariable(0)
  ```
  A change fires the `luaPanelGlobalChanged(index, value)` callback. There are **64 of them**, at
  fixed indices **0–63**, and they store **integers only**. Setting an index outside 0–63 is silently
  ignored; only 0–63 are saved with the panel. These are the same globals a
  [SysEx formula](08-sending-receiving.md#sysex-messages-formulas--tokens) can read with its
  `k…`/`o…`/`p…`/`n…` tokens, and a [value expression](05-value-mapping.md) can set with `setGlobal`.
- **State callbacks** — `luaPanelSaveState(tree)` / `luaPanelRestoreState(tree)` let you stash and
  restore arbitrary data in the panel's saved state (and `luaCtrlrSaveState`/`luaCtrlrResoreState`
  at the Ctrlr level).

> 🔗 Deeper: all of these are in the [Lua reference](lua/02-lua-reference.md#callback-hooks).

## Troubleshooting & platform notes

Most day-to-day issues are covered in [Chapter 9](09-debugging.md). The items below are
**install- and platform-level** problems — mostly macOS security and file-location questions — that
are documented in detail on the project wiki:

| Symptom | Where to look |
|---|---|
| macOS won't open CtrlrX ("unidentified developer" / quarantined) | [Whitelisting Ctrlr on macOS & avoiding quarantine](https://github.com/damiensellier/CtrlrX/wiki/Whitelisting-Ctrlr-on-macOS-and-avoid-quarantine) |
| An **exported** instance is rejected for a broken/invalid signature on macOS | [Replacing a CtrlrX exported instance's corrupted codesigning](https://github.com/damiensellier/CtrlrX/wiki/Replacing-CtrlrX-exported-instance-corrupted-codesigning-on-macOS) |
| The AU won't validate / doesn't show in the DAW | [AudioUnit validation test — command line](https://github.com/damiensellier/CtrlrX/wiki/AudioUnit-Validation-Test-Command-Line) |
| Images/artwork missing after moving a panel from old Ctrlr to CtrlrX | [Fixing missing panel resources when migrating](https://github.com/damiensellier/CtrlrX/wiki/FIX-for-missing-panel-resources-%28images-etc%29-when-migrating-from-Ctrlr-to-CtrlrX) |
| Native file dialogs misbehave on macOS Big Sur and later | [Uncheck OS-native dialog windows (macOS Big Sur+)](https://github.com/damiensellier/CtrlrX/wiki/Uncheck-OS-native-dialog-widows-(macOS-BigSur-&-above)) |
| CtrlrX crashes immediately on startup | [Reset Ctrlr if it crashes on startup](https://github.com/damiensellier/CtrlrX/wiki/Reset-CTRLR-if-it-crashes-on-startup) |
| Where are my settings stored? | [Ctrlr.settings file location](https://github.com/damiensellier/CtrlrX/wiki/Ctrlr.settings-file-location) |

> 🔗 Deeper: the full, up-to-date list lives in the
> [CtrlrX wiki](https://github.com/damiensellier/CtrlrX/wiki).

## Where to go from here

You now have the full building loop: lay out controls, map them to MIDI, script behavior, debug, and
ship. To go further:

- **Learn Lua properly** → [Lua Guide](lua/01-lua-guide.md)
- **Look up an API** → [Lua API Reference](lua/02-lua-reference.md)
- **Study real panels** — open existing `.panel` files in Edit mode and read their mappings and
  methods. This was the original community's #1 learning tip, and it still is.
- **Let an AI coding agent edit the panel file** → the `ctrlrx-panel-builder` skill in
  [skills/](../../skills/README.md) teaches agents such as Claude Code or GitHub Copilot to edit
  `.panel` files directly and test them with [CtrlrPanelProbe](../PanelProbe.md).

---

[← 09 Debugging](09-debugging.md) | [Index](README.md) | [Lua Guide](lua/01-lua-guide.md)
