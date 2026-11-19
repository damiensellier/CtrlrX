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
| `juce8_alsa_sysex_input.patch` | `modules/juce_audio_devices/native/juce_Midi_linux.cpp` | A legacy (non-UMP) ALSA client gets a long SysEx as events of at most 256 bytes. JUCE 6 joined them; JUCE 8.0.12 converts each one as a whole message, so a SysEx over 256 bytes from a device arrives cut off. The patch joins them with JUCE's own `BytestreamSysexExtractor`. Hits alsa-lib < 1.2.10 or a kernel < 6.5, and the test mock. JUCE develop (2026-11) has the same code. | CtrlrX 8.0.85, long SysEx on JUCE 8, Hedde Bosman. |
| `juce8_midioutput_send_message_at.patch` | `modules/juce_audio_devices/midi_io/juce_MidiDevices.h` | Adds `MidiOutput::sendMessageAt()`: queue one message of any size on the background thread. A `MidiBuffer` event holds at most 65529 bytes of data, so a longer SysEx was sent with `sendMessageNow()` on another thread: out of order, and on JUCE 8 broken by the other thread's packets on every OS. JUCE has no other way to queue it. | CtrlrX 5.6.37 `juce6_midioutput_send_message_at.patch` (long SysEx device output order), Hedde Bosman; carried to JUCE 8.0.12. |
| `juce8_scheduled_event_thread_keep_event.patch` | `modules/juce_audio_devices/midi_io/juce_ScheduledEventThread.h` | `MidiOutput`'s background thread lost every event queued more than 20 ms ahead, so with `panelMidiGlobalDelay` over 20 ms (or a burst at a smaller delay) most device MIDI never left. **Fixed in JUCE 8.0.13** (`82859cc2b`): drop this patch at the upgrade. | JUCE's own fix, back-ported to 8.0.12 by CtrlrX 8.0.85. |

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

- `juce8_scheduled_event_thread_keep_event.patch` is JUCE's own fix from 8.0.13: drop it.
- JUCE 8.0.17 adds entity-expansion and nesting limits to `juce_XmlDocument.cpp`, but
  `readQuotedString()` is unchanged. The XML patch is still needed there and must be rebased.
