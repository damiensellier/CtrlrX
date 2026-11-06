# Workflow: Lua kept in `.lua` files

Panel Lua is easier to review, diff, lint and unit-test as ordinary `.lua` files. There are two
ways to keep it there; pick one per panel and say which in the project's README.

## Contents

- Choosing: CtrlrX external files or an inliner script
- Option A: `luaMethodSource="1"`
- Option B: an inliner script
- Rules for an inliner
- The loop
- Checks worth automating

## Choosing: CtrlrX external files or an inliner script

| | A: `luaMethodSource="1"` | B: inliner script |
|---|---|---|
| Panel file | refers to files in `<panel name>/lua/` | self-contained, code in `luaMethodCode` |
| Shipping | export (`.bpanelz`) inlines the files | the committed `.panel` already works anywhere |
| Risk | a `.panel` copied without its lua directory loads with empty methods | a hand edit of the panel is overwritten by the next inline |
| Use when | the panel is only ever shipped as an export | the `.panel` itself is distributed or tested by others |

## Option A: `luaMethodSource="1"`

Each method names its file in `luaMethodSourcePath`, relative to `<panel path without
extension>/lua/` (`panel-file-format.md`, "Lua in external files"). CtrlrX's GUI can convert a
panel's methods to files and writes one file per method, in subdirectories that mirror the method
groups. `panel_tool.py lint` reports every path that doesn't resolve; a missing file otherwise
loads as an empty method that reports no error.

## Option B: an inliner script

A small script reads each `.lua` file and writes its text into the matching `<luaMethod>`'s
`luaMethodCode`, leaving everything else in the panel untouched. The `.lua` files are the source;
the panel is the output.

## Rules for an inliner

- **Files win.** Anything that edits `luaMethodCode` in the panel directly (the CtrlrX GUI, a
  script) is reverted by the next inline, silently. Make edits in the files, and treat a diff in
  the panel's Lua that has no matching file change as a bug.
- **Copy the bytes verbatim.** Keep CRLF, lone CR and every other character exactly as in the
  file; don't normalise line endings or strip trailing whitespace.
- **Escape once.** If the script writes the panel with an XML library, pass the plain text: the
  library escapes it. If it splices text into the XML itself, escape with the same rules as
  `panel_tool.py escape` (including CR as `&#13;`). Doing both gives `&amp;quot;`.
- **Idempotent.** Running it twice changes nothing; a second run's diff must be empty.
- **Match by name.** Map file → method by `luaMethodName` (or a path that mirrors the groups).
  A file with no method either creates it (when the user wants one file = one method) or is an
  error; a method with no file is an error. Decide which, and report every method the script adds:
  never append one silently. If you check that a file defines its method, accept both `function name(...)` and
  `name = function(...)`: they are the same in Lua, and panels use both.
- **Preserve the panel's attribute order and formatting** (`panel-file-format.md`, "Attribute order
  is behaviour"). Change only `luaMethodCode`.
- **No load-order dependencies.** Top-level code in one file must not read globals defined by
  another: CtrlrX's exporter reorders methods. Put shared state behind functions or initialise it
  from `luaPanelLoaded`.
- **Lua 5.1 language only.** Check every file with `luac5.1 -p` before inlining: `//`, `&` and
  other 5.3 syntax pass a newer `luac` and fail at panel load. `luac5.1` also rejects `goto`, which
  CtrlrX accepts only from 5.6.36 (LuaJIT); `panel_tool.py lint` warns about all three.
- Never put the methods in the `Built-In` group.

## The loop

```
Lua-in-files loop:
- [ ] 1. Edit the .lua file
- [ ] 2. luac5.1 -p <file>   → fix syntax (or accept a `goto` warning if 5.6.36+ only)
- [ ] 3. Run the inliner (Option B) — or nothing (Option A)
- [ ] 4. python3 scripts/panel_tool.py lint <panel>   → clean
- [ ] 5. Run the panel's probe checks; plain-Lua unit tests under lua5.1 where the code allows
- [ ] 6. Check that panel and files agree (below), then commit both
```

## Checks worth automating

- **Panel and files agree**: for every method, the panel's `luaMethodCode` equals its file's bytes
  (Option B), or every `luaMethodSourcePath` resolves (Option A, `panel_tool.py lint`). Run it in
  CI so a GUI edit or a forgotten inline fails the build.
- **Inliner round trip**: inline into a copy, inline again, compare: no diff.
- **Unit tests without CtrlrX**: code that touches CtrlrX only through thin wrappers (send MIDI,
  read a modulator) runs under a bare `lua5.1` with stub wrappers, in milliseconds per test. Keep
  the probe for the wiring.
