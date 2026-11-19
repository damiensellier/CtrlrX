# Local changes to vendored JUCE

CtrlrX vendors JUCE in `JUCE/`. A local change is made in place in `JUCE/` and is also saved
here as a `.patch` file, so it is easy to see and to carry to the next JUCE version.

**The files here are documentation only. CMake does not apply them.** Each one must equal
`git diff <pristine JUCE> HEAD -- <file>`. To check one from the repo root:

```bash
git apply -R --check patches/<name>.patch
```

(`juce6_xml_parser_perf.patch` has paths relative to `JUCE/`: check it from there with
`git apply -R --check -p1 ../patches/juce6_xml_parser_perf.patch`.)

**Re-check every patch at each JUCE upgrade.** Keep a patch only if the new pristine JUCE still
has the defect. Rebase it if the file changed.

## Current patches (JUCE 6.0.8)

| Patch | JUCE file | Reason | Origin |
|---|---|---|---|
| `juce6_xml_parser_perf.patch` | `modules/juce_core/xml/juce_XmlDocument.cpp` | `readQuotedString()` is O(n²) in the attribute length. Large panels (base64 images, MB of escaped Lua) load very slowly. | CtrlrX #341, Francois Rossi. |
| `juce6_midioutput_send_message_at.patch` | `modules/juce_audio_devices/midi_io/juce_MidiDevices.{h,cpp}` | Adds `MidiOutput::sendMessageAt()`: queue one message of any size on the background thread. A `MidiBuffer` event holds at most 65529 bytes of data, so a longer SysEx was sent with `sendMessageNow()` on another thread, out of order and mixed with other messages on macOS. JUCE has no other way to queue it. Also: `run()` sends a message that is more than 200 ms late instead of dropping it, so a slow output no longer loses the messages queued behind a long SysEx. | CtrlrX 5.6.37, long SysEx device output order: thru D2D sent broken SysEx on macOS (CI run 43745732738), Hedde Bosman. |
