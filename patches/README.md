# Local changes to vendored JUCE

CtrlrX vendors JUCE in `JUCE/`. A local change is made in place in `JUCE/` and is also saved
here as a `.patch` file, so it is easy to see and to carry to the next JUCE version.

**The files here are documentation only. CMake does not apply them.** Each one must equal
`git diff <pristine JUCE> HEAD -- <file>`. To check one from the repo root:

```bash
git apply -R --check patches/<name>.patch
```

**Re-check every patch at each JUCE upgrade.** Keep a patch only if the new pristine JUCE still
has the defect. Rebase it if the file changed.

## Current patches (JUCE 8.0.12)

| Patch | JUCE file | Reason | Origin |
|---|---|---|---|
| `juce8_xml_parser_perf.patch` | `modules/juce_core/xml/juce_XmlDocument.cpp` | `readQuotedString()` is O(n²) in the attribute length. Large panels (base64 images, MB of escaped Lua) load very slowly. JUCE 8.0.12 still has the same function. | CtrlrX #341 (5.6.37, `juce6_xml_parser_perf.patch`), Francois Rossi. Also in John Goodland's JUCE 8 fork as `juce_xml_parser_perf.patch`. |
| `juce8_codeeditor_setfont.patch` | `modules/juce_gui_extra/code_editor/juce_CodeEditorComponent.cpp` | `setFont()` with a font of size 0 gives a zero char width and line height, and the editor divides by them: crash when a user sets the Lua editor font size to 0. JUCE 8.0.12 has the same code. | CtrlrX v5.6.34 (#140), Damien Sellier and John Goodland. |

`juce8_xml_parser_perf.patch` replaces 5.6.37's `juce6_xml_parser_perf.patch`: the code change is
the same, rebased on JUCE 8.0.12.

## Earlier local changes, not carried to JUCE 8

| JUCE file | JUCE 6 change | State on JUCE 8.0.12 |
|---|---|---|
| `extras/Build/juceaide/CMakeLists.txt` | Pass `CMAKE_OSX_DEPLOYMENT_TARGET` to the juceaide build (#338). | **Dropped.** JUCE 8.0.12 forwards it itself (`PASSTHROUGH_ARGS`). |
| `modules/juce_gui_basics/native/juce_XWindowSystem_linux.cpp` | Keypad minus: report keycode 82 as 20, so the zoom-out shortcut works (#283, v5.6.36). | **On hold.** The bug depends on the X11/Wayland setup. It is re-tested on JUCE 8 first. If it still fails, a CtrlrX key binding (`KeyPress::numberPadSubtract`) is preferred over a JUCE change. |
| `modules/juce_gui_basics/lookandfeel/juce_LookAndFeel.cpp` | `jassertfalse` in `LookAndFeel::findColour()` commented out (v5.6.34), so Debug builds don't stop on panel load. | **Replaced** by a CtrlrX change: `gui::initLookAndFeelDefaults()` defines the missing colour IDs as black, the colour JUCE returned for them. |
| `modules/juce_audio_plugin_client/AAX/juce_AAX_Wrapper.cpp` | `AAX_eStemFormat_Reserved_1`–`3` cases removed (v5.6.34), for AAX SDK 2.8+. | **Dropped.** JUCE 8.0.12's AAX wrapper doesn't use them. |

## Notes for the next JUCE upgrade

- JUCE 8.0.17 adds entity-expansion and nesting limits to `juce_XmlDocument.cpp`, but
  `readQuotedString()` is unchanged. The XML patch is still needed there and must be rebased.
