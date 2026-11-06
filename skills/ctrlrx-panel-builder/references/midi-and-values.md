# MIDI messages and value mapping

Everything here is set on a modulator's `<midi>` child and its value attributes. When the source
disagrees with this page, the source wins: `Source/MIDI/CtrlrMidiMessage.*`,
`Source/MIDI/CtrlrOwnedMidiMessage.cpp`, `Source/Core/CtrlrSysexProcessor.*`.

## Contents

- `midiMessageType` — the integer enum
- Channels: panel channel vs. a modulator's own
- Raw range and where the value byte comes from
- Multi messages: NRPN, RPN, 14-bit CC (long form only)
- NRPN pitfalls: signed values, merged bursts, header latching
- SysEx formulas and tokens
- Value expressions
- Value maps
- Receiving MIDI

## `midiMessageType` — the integer enum

| N | Type | Wire bytes | Typical use |
|---|---|---|---|
| 0 | CC | `Bn cc vv` | Most parameters. |
| 1 | Aftertouch (poly) | `An nn vv` | Per-note pressure. |
| 2 | ChannelPressure | `Dn vv` | Channel aftertouch. |
| 3 | NoteOn | `9n nn vv` | Trigger a note / pad. |
| 4 | NoteOff | `8n nn vv` | Release a note. |
| 5 | SysEx | `F0 … F7` | Parameters via a formula (below). |
| 6 | Multi | several messages | NRPN, RPN, 14-bit CC (below). |
| 7 | ProgramChange | `Cn pp` | Select a program. |
| 8 | PitchWheel | `En ll mm` | 14-bit bend; range `0..16383`, centre 8192. |
| 9 | None | — | No MIDI (Lua-only or display control). |
| 10–14 | MidiClock / Continue / Stop / Start / ActiveSense | realtime | Clock / transport. |

`n` is the channel nibble (channel 1 → `0`). `midiMessageCtrlrNumber` is the CC / note / program
number (for Multi: the NRPN/RPN parameter number, 0..16383).

## Channels: panel channel vs. a modulator's own

| `midiMessageChannelOverride` | sends on | matches incoming MIDI on |
|---|---|---|
| `0` (default) | the panel's `panelMidiOutputChannelDevice` | the same channel |
| `1` | the modulator's `midiMessageChannel` | the same channel |

`panelMidiInputChannelDevice` does **not** decide which channel a modulator matches
(`CtrlrOwnedMidiMessage::getChannel`; measured: with input channel 2 and output channel 4, a
modulator without the override matched `B3`, channel 4). So a control meant for a second device
or part on its own channel needs `midiMessageChannelOverride="1"` **and** `midiMessageChannel`;
setting only the channel silently keeps the panel channel in both directions. Bytes that Lua builds
itself (`sendMidiMessageNow`) carry whatever status byte the code wrote.

## Raw range and where the value byte comes from

A control's raw value lies between `modulatorMin` and `modulatorMax`; match the max to the MIDI
resolution: 7-bit `0..127`, 14-bit (PitchWheel, NRPN, 14-bit CC) `0..16383`, switch `0..1`. A
slider also needs the same range on its component (`uiSliderMin` / `uiSliderMax`, see
`components.md`).

- **The value byte is the modulator's current value, never `midiMessageCtrlrValue`.** For CC,
  NoteOn, NoteOff, Aftertouch, ChannelPressure and PitchWheel, CtrlrX builds the message from
  `getValue()` (`CtrlrMidiMessage.cpp`). A `0..1` trigger button therefore sends velocity 0 or 1
  whatever `midiMessageCtrlrValue` says, and velocity 0 is a NoteOff. To send a fixed value byte,
  set `midiMessageType="9"` and send it from Lua.
- **A PitchWheel left at `0..127` uses 0.8 % of its range** (`ef 7f 00` at full travel). Set
  `modulatorMax="16383"` and `uiSliderMax="16383"`; raise `uiSliderMouseWheelInterval` too, or fine
  adjustment is unreachable by mouse.

## Multi messages: NRPN, RPN, 14-bit CC (long form only)

Set `midiMessageType="6"` and `midiMessageMultiList`. Messages are separated by `:`. Use the
**long form**, one message per part:

```
CC,ByteValue,<half>,<cc number>,<source>
```

- `<half>`: `MSB7bitValue` (bits 7–13) or `LSB7bitValue` (bits 0–6) of the source.
- `<source>`: `-2` = the modulator's `midiMessageCtrlrNumber` (the parameter number), `-1` = the
  modulator's value.

Measured forms (copy them; set `midiMessageCtrlrNumber` to the parameter number):

| Purpose | `midiMessageMultiList` | number 300, value 1000 sends |
|---|---|---|
| NRPN, 14-bit data | `CC,ByteValue,MSB7bitValue,99,-2:CC,ByteValue,LSB7bitValue,98,-2:CC,ByteValue,MSB7bitValue,6,-1:CC,ByteValue,LSB7bitValue,38,-1` | `b0 63 02, b0 62 2c, b0 06 07, b0 26 68` |
| NRPN, 7-bit data (`modulatorMax="127"`) | `CC,ByteValue,MSB7bitValue,99,-2:CC,ByteValue,LSB7bitValue,98,-2:CC,ByteValue,LSB7bitValue,6,-1` | value 100: `b0 63 02, b0 62 2c, b0 06 64` |
| RPN, 14-bit data | as NRPN with `101` / `100` instead of `99` / `98` | number 0, value 8192: `b0 65 00, b0 64 00, b0 06 40, b0 26 00` |
| 14-bit CC (CC 1 + CC 33) | `CC,ByteValue,MSB7bitValue,1,-1:CC,ByteValue,LSB7bitValue,33,-1` | `b0 01 07, b0 21 68` |

**Do not use the short form** (`CC,99,2 : CC,98,10 : CC,6,Direct`). In the current parser it
ignores the number in every part: all messages go out on the modulator's own number and the first
literal becomes the value (measured: number 5 → `b0 05 02, b0 05 2c, b0 05 7f, b0 05 7f`). A fixed
sequence of different CCs (a panic button) is therefore a Lua job:
`panel:sendMidiMessageNow(CtrlrMidiMessage("B0 7B 00"))`, one call per message.

Always probe a Multi modulator once and assert the exact bytes and their order.

## NRPN pitfalls: signed values, merged bursts, header latching

- **Signed parameters.** For a device that wants two's-complement 14-bit data, keep the
  modulator signed (`modulatorMin="-64"` …) and convert in `modulatorValueExpression`:
  `lt(modulatorValue,0,16384+modulatorValue,modulatorValue)`. `lt` takes four arguments
  (`lt(a, b, ifTrue, ifFalse)`).
- **Merged bursts.** With `panelMidiGlobalDelay="0"`, every buffer sent within the same
  millisecond gets the same start time, so several Multi modulators set in one Lua chunk interleave
  on the wire (`99 a, 99 b, 98 a, 98 b, 06 a, 06 b, …`). The device then writes the wrong
  parameters. Set `panelMidiGlobalDelay="2"` (each buffer starts 1 ms after the previous one; an
  NRPN takes ~3.8 ms on a DIN wire anyway). The same mechanism interleaves a `uiCombo`'s double
  send under the probe (`verify-with-probe.md`).
- **Header latching.** With `midiMessageLatchAndStream` on, CtrlrX skips the CC 99/98 header when
  the parameter is the same as the previous message's, so later writes are a bare CC 6/38 pair.
  The latch is panel-wide state: whatever sent last decides whether the header is resent.
- **NRPN is one-way.** Multi modulators are never updated by incoming MIDI (see "Receiving").

## SysEx formulas and tokens

Set `midiMessageType="5"` and `midiMessageSysExFormula`: space-separated two-character hex bytes,
with token codes where dynamic data goes (`CtrlrSysexProcessor::sysExIdentifyToken`):

| Code | Inserts |
|---|---|
| `xx` | the value as one 7-bit byte |
| `MS` / `LS` | value bits 7–13 / 0–6 |
| `ms` / `ls` | value high / low 4-bit nibble |
| `yy` / `0y` | MIDI channel (7-bit) / channel in the low nibble |
| `r1`–`r4` | Roland address/value byte splits |
| `q0`–`q3` / `Q0`–`Q3` | 16-bit value as 4-bit nibbles, LSB / MSB first |
| `tp` / `tb` | current program / bank |
| `k…` `o…` `p…` `n…` | a panel global variable (hex digit selects which) |
| `Xn` `zn` `wn` `On` `tc` | checksum over the `n` preceding bytes: XOR, 2's complement (Roland/Yamaha), sum, 1's complement, Technics |
| `ii` | ignore this byte when matching incoming SysEx |
| `u…` / `v…` | a byte computed by Lua / by a sub-formula |

Example: manufacturer `7D`, parameter `12` = the value, XOR checksum over the 3 bytes before it:

```
F0 7D 12 xx X3 F7
```

The Roland checksum `z<n>` wraps 128 to 0 correctly. These codes belong to SysEx formulas only;
they are not Multi-list tokens. For anything the tokens can't express, build the bytes in Lua and
send them (`lua-api.md`, "Sending MIDI"), and end every frame with `F7` yourself.

## Value expressions

| Attribute | Direction | Default |
|---|---|---|
| `modulatorValueExpression` | value → outgoing MIDI value | `modulatorValue` |
| `modulatorValueExpressionReverse` | incoming MIDI value → value | `midiValue` |
| `modulatorControllerExpression` | MIDI-learn input → value | `value` |

Variables: `modulatorValue`, `modulatorMappedValue`, `midiValue`, `midiNumber`, `vstIndex`,
`panel.<prop>`, `global.<n>`. Functions: `abs`, `ceil`, `floor`, `mod`, `fmod`, `pow`, `min`,
`max`, `eq`/`lt`/`lte`/`gt`/`gte` (four arguments: `a, b, ifTrue, ifFalse`), `setBit`/`isBitSet`/
`clearBit`/`getBitRangeAsInt`/`setBitRangeAsInt`, `setGlobal`, `+ - * /`.

```
modulatorValueExpression        = floor(modulatorValue / 2)
modulatorValueExpressionReverse = midiValue * 2
```

Expressions change the MIDI number only, never what the control **displays**. Per-value text on a
knob needs a `uiFixedSlider` + value map (`components.md`).

## Value maps

A value map gives a `uiCombo`, `uiFixedSlider` or `uiFixedImageSlider` named entries. The
modulator holds the **0-based index** of the entry, not a device value. Write it one entry per
line in `uiComboContent` / `uiFixedSliderContent` (newlines as `&#10;` in the XML):

```
Sine
Triangle
Saw
```

- `Label=raw` lines carry a device value per entry; still confirm with the probe which number
  your callback and the MIDI get before relying on it (`components.md`, "Combos").
- The map sets the range: `modulatorMax` becomes `entries - 1`. A blank line or a line starting
  with `__` is skipped and shifts every entry below it; use a visible placeholder label instead.
- A reverse lookup (device value → index) that misses a value silently picks index 0, and the next
  touch sends that wrong value to the device. Make the table total, or fail loudly in Lua.

## Receiving MIDI

Incoming single messages (CC, Note, Program Change, PitchWheel, SysEx by pattern) are matched to
modulators by type + channel (table above) + number, and the matching control follows the
hardware. The match table is built when the panel loads (`CtrlrPanel::bootstrapPanel`), also for a
panel saved in edit mode. Panel Lua can't rebuild or inspect it at runtime
(`panel:getInputComparator()` is not usable from Lua).

- **Multi (NRPN/RPN/14-bit) modulators never update from incoming MIDI**, although the code path
  exists. If the device must drive the panel, decode a SysEx dump or plain CCs in
  `luaPanelMidiReceived`.
- A part-received Multi message is discarded after 200 ms, so a probe test that injects the four
  CCs of an NRPN must use `"settleMs": 10` on each `injectDeviceMidi` step.
- A negative result ("no modulator moved") needs a positive control in the same run (an injected
  CC that *does* move one), or a broken test looks the same.
- Avoid feedback loops with the `source` argument of `luaModulatorValueChange` (2 = MIDI in), and
  keep `panelMidiThruD2D="0"` unless device→device thru is wanted: with it on, every device
  message is echoed straight back to the device.
- SysEx reaches `luaPanelMidiReceived` only once complete; there is no progress for a long dump.
