#!/usr/bin/env python3
"""panel_tool.py — utilities for authoring/inspecting CtrlrX panel files.

Stdlib only (python3). Reads every panel format — .panel (XML), .panelz (gzipped XML),
.bpanelz (gzipped binary ValueTree) and .bpanel (raw binary ValueTree) — and presents all
of them as the same XML element tree, so every subcommand works on any of them. Writing is
still XML-only: see the note at the end. Subcommands:

  summary  FILE              Compact, token-cheap overview of a panel (use this instead of
                             reading the raw XML, which is huge).
  lint     FILE              Well-formedness + structural checks (unique names, dangling Lua
                             references, unresolved luaMethodSourcePath, missing uiType,
                             duplicate vstIndex) + warnings for MIDI and Lua-binding traps that
                             load fine but misbehave, Lua syntax CtrlrX doesn't compile (goto
                             before 5.6.36, //, bitwise operators) and literal SysEx without F7.
                             Lua comments and strings are ignored. Exit 0 = no errors (read
                             the warnings); exit 1 = errors, one per line.
  escape   [FILE|-]          XML-escape raw Lua so it can go into a luaMethodCode attribute.
                             Reads stdin if FILE is '-' or omitted. Bytes are kept verbatim
                             (CR -> &#13;). Only for text you paste into the XML yourself: an
                             XML library (ElementTree, lxml) escapes on write, so escaping
                             first double-escapes (&amp;quot;).
  dump     FILE SELECTOR     Print one element's XML (one attribute per line) so it can be
                             used as a copy-and-edit template instead of reading the raw file.
                             SELECTOR: 'editor' (the <uiPanelEditor>, incl. layer defs),
                             'panel' (root open tag attributes only), 'modulator:NAME'
                             (a modulator by name), or 'modulators' (list names + uiType).
  xml      FILE OUT.panel    Write any panel format as an XML .panel you can edit (attribute
                             order kept). The way to edit a .bpanelz / .panelz / .bpanel: never
                             save decompressed bytes as .panel, they are not XML. Reports how
                             many <resource> nodes (images, fonts) the file has; re-attach
                             those in CtrlrX before exporting a .bpanelz again.

Note: .bpanelz is not gzipped XML — it is a gzipped *binary* JUCE ValueTree, and it also
embeds the panel's resources (images, fonts) as base64 in <resource> nodes, which the XML
export drops. This tool can therefore read a .bpanelz but not write one: to produce a
.bpanelz with its resources intact, use the CtrlrX app (File -> Export).
"""
import base64
import os
import re
import struct
import sys
import xml.etree.ElementTree as ET
import zlib
from xml.sax.saxutils import escape as _xml_escape

# midiMessageType integer enum -> name (Source/MIDI/CtrlrMidiMessage.h)
MIDI_TYPES = {
    0: "CC", 1: "Aftertouch", 2: "ChannelPressure", 3: "NoteOn", 4: "NoteOff",
    5: "SysEx", 6: "Multi", 7: "ProgramChange", 8: "PitchWheel", 9: "None",
    10: "MidiClock", 11: "MidiClockContinue", 12: "MidiClockStop",
    13: "MidiClockStart", 14: "ActiveSense",
}

NONE_VALUES = {"-- None", "", "None"}


# --- reading the binary ValueTree form (.bpanelz / .bpanel) ----------------------------
#
# CtrlrPanel::openBinPanel gunzips the file and calls ValueTree::readFromStream. The wire
# format is defined by ValueTree::readFromStream (juce_ValueTree.cpp) and var::readFromStream
# (juce_Variant.cpp):
#
#   tree  := string tag, compressedInt numProps, {string name, var value} * numProps,
#            compressedInt numChildren, tree * numChildren
#   var   := compressedInt numBytes, byte marker, payload of numBytes-1
#   marker: 1=int32 2=true 3=false 4=double 5=string 6=int64 7=array 8=binary 9=undefined
#
# Strings are NUL-terminated UTF-8; a compressed int is a length byte (low 7 bits = number of
# little-endian bytes that follow, bit 7 = negative).

VAR_INT, VAR_TRUE, VAR_FALSE, VAR_DOUBLE = 1, 2, 3, 4
VAR_STRING, VAR_INT64, VAR_ARRAY, VAR_BINARY = 5, 6, 7, 8


class _Stream:
    """Just enough of juce::InputStream to walk a serialised ValueTree."""

    __slots__ = ("data", "pos")

    def __init__(self, data):
        self.data = data
        self.pos = 0

    def take(self, n):
        if n < 0 or self.pos + n > len(self.data):
            raise ValueError(f"truncated ValueTree: want {n} bytes at offset {self.pos}")
        out = self.data[self.pos:self.pos + n]
        self.pos += n
        return out

    def byte(self):
        return self.take(1)[0]

    def string(self):
        end = self.data.find(b"\0", self.pos)
        if end < 0:
            raise ValueError(f"truncated ValueTree: unterminated string at {self.pos}")
        raw = self.data[self.pos:end]
        self.pos = end + 1
        return raw.decode("utf-8", "replace")

    def compressed_int(self):
        size_byte = self.byte()
        if size_byte == 0:
            return 0
        n = size_byte & 0x7F
        if n > 4:
            raise ValueError(f"corrupt compressed int ({n} bytes) at offset {self.pos}")
        value = struct.unpack("<i", self.take(n).ljust(4, b"\0"))[0]
        return -value if size_byte >> 7 else value


def _var_to_text(value):
    """Render a juce::var the way ValueTree::createXml would write it into an attribute."""
    if value is None:
        return ""
    if isinstance(value, bool):
        return "1" if value else "0"
    if isinstance(value, int):
        return str(value)
    if isinstance(value, float):
        return str(int(value)) if value.is_integer() else repr(value)
    if isinstance(value, (bytes, bytearray)):
        return base64.b64encode(bytes(value)).decode("ascii")
    if isinstance(value, list):
        return " ".join(_var_to_text(v) for v in value)
    return str(value)


def _read_var(s):
    num_bytes = s.compressed_int()
    if num_bytes <= 0:
        return None
    marker, payload = s.byte(), num_bytes - 1
    if marker == VAR_INT:
        return struct.unpack("<i", s.take(4))[0]
    if marker == VAR_TRUE:
        return True
    if marker == VAR_FALSE:
        return False
    if marker == VAR_DOUBLE:
        return struct.unpack("<d", s.take(8))[0]
    if marker == VAR_STRING:
        return s.take(payload).rstrip(b"\0").decode("utf-8", "replace")
    if marker == VAR_INT64:
        return struct.unpack("<q", s.take(8))[0]
    if marker == VAR_ARRAY:
        return [_read_var(s) for _ in range(s.compressed_int())]
    if marker == VAR_BINARY:
        return s.take(payload)
    s.take(payload)  # undefined, and anything a future JUCE adds
    return None


def _read_tree(s):
    """Read one ValueTree node straight into an ET.Element, so every other command
    in this tool can stay pure ElementTree regardless of the file's on-disk format."""
    tag = s.string()
    if not tag:
        return None
    elem = ET.Element(tag)
    num_props = s.compressed_int()
    if num_props < 0:
        raise ValueError(f"corrupt ValueTree: negative property count {num_props}")
    for _ in range(num_props):
        name = s.string()
        value = _read_var(s)
        if name:
            elem.set(name, _var_to_text(value))
    for _ in range(s.compressed_int()):
        child = _read_tree(s)
        if child is None:
            break
        elem.append(child)
    return elem


def _inflate(data):
    """Decompress gzip / zlib / raw-deflate; tolerate a truncated tail."""
    for wbits in (47, 15, -15):
        try:
            return zlib.decompress(data, wbits)
        except zlib.error:
            pass
    for wbits in (47, 15, -15):
        try:
            partial = zlib.decompressobj(wbits).decompress(data)
        except zlib.error:
            continue
        if partial:
            return partial
    raise ValueError("file looks compressed but is not a valid gzip/zlib stream")


def parse_bytes(data, path="<data>"):
    """Parse any panel format into an ElementTree root element."""
    if data[:2] == b"\x1f\x8b" or data[:1] == b"\x78":  # .panelz / .bpanelz
        data = _inflate(data)
    if data.lstrip()[:1] == b"<":  # plain or decompressed XML
        # some .panelz end in a NUL after </panel>; CtrlrX ignores it, ElementTree doesn't
        return ET.fromstring(data.lstrip().lstrip(b"\xef\xbb\xbf").rstrip(b"\0"))
    try:
        root = _read_tree(_Stream(data))
    except ValueError as exc:
        raise ValueError(f"{path}: not XML, and the binary ValueTree is unreadable ({exc})")
    if root is None:
        raise ValueError(f"{path}: empty ValueTree")
    if root.tag != "panel":
        raise ValueError(
            f"{path}: parsed as a binary ValueTree but the root is <{root.tag}>, not <panel>")
    return root


def parse(path):
    with open(path, "rb") as f:
        return parse_bytes(f.read(), path)


def _num(s, default=None):
    try:
        return int(float(s))
    except (TypeError, ValueError):
        return default


# MIDI types whose value byte comes from the modulator value, not midiMessageCtrlrValue.
# (CtrlrMidiMessage.cpp:1004-1019 -> ...(ch, getNumber(), getValue()))
VALUE_FROM_MODULATOR = {0, 1, 2, 3, 4, 8}
MIDI_NONE = 9
PITCHWHEEL = 8


def midi_traps(mod, comp, midi):
    """Wiring mistakes that lint cleanly and load fine, but emit wrong MIDI.

    Each was observed in a real hand-edited panel and confirmed with CtrlrPanelProbe.
    """
    out = []
    name = mod.get("name")
    mtype = _num(midi.get("midiMessageType"), MIDI_NONE)
    ui = comp.get("uiType") or ""
    mmax = _num(mod.get("modulatorMax"))

    # 1. The one-line uiButtonContent trap. The valueMap is uiButtonContent split on
    #    newlines; a single entry means increment() never changes the value, so
    #    luaModulatorValueChange never fires and a NoteOn emits velocity 0.
    if ui in ("uiButton", "uiToggleButton"):
        content = comp.get("uiButtonContent")
        if content is not None and len([l for l in content.split("\n") if l != ""]) < 2:
            wired = mod.get("luaModulatorValueChange") not in NONE_VALUES
            has_mouse = (comp.get("componentLuaMouseUp") not in NONE_VALUES
                         or comp.get("componentLuaMouseDown") not in NONE_VALUES)
            if mtype != MIDI_NONE:
                out.append(f"'{name}': uiButtonContent has one line, so its value never "
                           f"changes and its MIDI always sends value 0 (velocity 0 = NoteOff). "
                           f"Use componentLuaMouseDown/Up, or give it two lines")
            elif wired and not has_mouse:
                out.append(f"'{name}': luaModulatorValueChange will never fire - "
                           f"uiButtonContent has one line, so the value never changes. "
                           f"Use componentLuaMouseUp instead")

    # 2. PitchWheel is 14-bit; a 0..127 range silently emits only the bottom 0.8%.
    if mtype == PITCHWHEEL and mmax is not None and mmax <= 127:
        out.append(f"'{name}': PitchWheel with modulatorMax={mmax} wastes 14-bit range "
                   f"(full travel emits bend 0..{mmax} of 16383); set 16383")

    # 3. midiMessageCtrlrValue looks like a fixed value byte but is ignored on these types.
    cval = _num(midi.get("midiMessageCtrlrValue"), 0)
    if mtype in VALUE_FROM_MODULATOR and cval:
        out.append(f"'{name}': midiMessageCtrlrValue={cval} is ignored - the value byte "
                   f"comes from the modulator value. Send from Lua for a fixed byte")

    # 4. Slider visible range is a separate property from the modulator range.
    if ui == "uiSlider" and mmax is not None:
        smax = _num(comp.get("uiSliderMax"))
        if smax is not None and smax != mmax:
            out.append(f"'{name}': uiSliderMax={smax} != modulatorMax={mmax}; "
                       f"the knob will clamp to the component range")
    return out


def lua_callbacks(elem):
    """Attribute (name, method) pairs that point at a Lua method (not 'None')."""
    out = []
    for k, v in elem.attrib.items():
        if k.startswith("lua") and v not in NONE_VALUES:
            out.append((k, v))
    return out


def cmd_summary(path):
    root = parse(path)
    print(f"panel: {root.get('name','?')}  "
          f"(v{root.get('panelVersionMajor','?')}.{root.get('panelVersionMinor','?')}, "
          f"device={root.get('panelDevice','?')})")

    # panel-level lua hooks
    pcb = lua_callbacks(root)
    if pcb:
        print("panel Lua hooks: " + ", ".join(f"{k}={v}" for k, v in pcb))

    # lua methods defined
    methods = [m.get("luaMethodName") or "?" for m in root.iter("luaMethod")]
    if methods:
        print(f"lua methods ({len(methods)}): " + ", ".join(methods))

    mods = root.findall("modulator")
    print(f"modulators ({len(mods)}):")
    for m in mods:
        name = m.get("name", "?")
        comp = m.find("component")
        ui = comp.get("uiType", "?") if comp is not None else "(no component)"
        rect = comp.get("componentRectangle", "") if comp is not None else ""
        midi = m.find("midi")
        if midi is not None:
            tnum = _num(midi.get("midiMessageType"), -1)
            t = MIDI_TYPES.get(tnum, midi.get("midiMessageType") or "?")
            ch = _num(midi.get("midiMessageChannel"))
            num = _num(midi.get("midiMessageCtrlrNumber"))
            midi_s = f"{t} ch{ch} #{num}"
            if t == "Multi" and midi.get("midiMessageMultiList"):
                midi_s += f"  multi=[{midi.get('midiMessageMultiList')}]"
            if t == "SysEx" and midi.get("midiMessageSysExFormula"):
                midi_s += f"  sysex=[{midi.get('midiMessageSysExFormula')}]"
        else:
            midi_s = "(no midi)"
        rng = f"{_num(m.get('modulatorMin'))}..{_num(m.get('modulatorMax'))}"
        static = " static" if m.get("modulatorIsStatic") == "1" else ""
        vst = m.get("vstIndex")
        line = f"  - {name}  [{ui}{static}]  {midi_s}  range {rng}"
        if vst not in (None, "-1"):
            line += f"  vst#{vst}"
        if rect:
            line += f"  @({rect})"
        print(line)
        for k, v in lua_callbacks(m):
            print(f"      lua: {k} -> {v}")
        if comp is not None:
            for k, v in lua_callbacks(comp):
                print(f"      lua(component): {k} -> {v}")
    return 0


# Silent-failure traps in CtrlrX's luabind bindings — see references/lua-api.md.
# Each is a runtime bug that lint and panel-load both pass happily.
_MB_SIZED_CTOR = re.compile(r"\bMemoryBlock\(\s*(?![\){])")
_SETSIZE_1ARG = re.compile(r":setSize\(\s*[^,()]+\)")
_GETNAME_TEXT = re.compile(r"getName\(\)\s*(\.\.|==|~=)|(\.\.|==|~=)\s*\w+:getName\(\)")
# Syntax CtrlrX's Lua doesn't compile. CtrlrX runs the Lua 5.1 language: plain Lua 5.1 up to
# 5.6.35, LuaJIT 2.1 from 5.6.36 (which adds goto and the `bit` library, not 5.3 operators).
_POST_51 = [
    (re.compile(r"\bgoto\b|::[A-Za-z_]\w*::"),
     "goto / ::label:: compiles only on CtrlrX 5.6.36+ (LuaJIT); older CtrlrX (Lua 5.1) "
     "rejects the method"),
    (re.compile(r"//"),
     "integer division // is Lua 5.3: no CtrlrX compiles it; use math.floor(a / b)"),
    (re.compile(r"<<|>>|&|\|"),
     "bitwise operators are Lua 5.3: no CtrlrX compiles them; use bit.band/bor/lshift/rshift "
     "(CtrlrX 5.6.36+) or arithmetic"),
]
# A MIDI message built from a literal: a byte table or a hex string.
_MIDI_TABLE = re.compile(r"\b(?:CtrlrMidiMessage|sendMidiMessageNow)\s*\(\s*\{([^{}]*)\}")
_MIDI_HEX = re.compile(r"\b(?:CtrlrMidiMessage|sendMidiMessageNow)\s*\(\s*(['\"])([0-9A-Fa-f\s,]+)\1")
_LONG_BRACKET = re.compile(r"\[(=*)\[")


def lua_strip(code, keep_strings=False):
    """The code with comments (and, unless keep_strings, string contents) blanked out.

    Blanked characters become spaces and newlines are kept, so positions don't move. Enough of
    Lua's lexer for linting: short strings with escapes, long brackets [[ ]] / [==[ ]==], and
    -- / --[[ ]] comments."""
    out, i, n = [], 0, len(code)

    def blank(s):
        return re.sub(r"[^\n]", " ", s)

    def long_end(m):
        close = "]" + m.group(1) + "]"
        end = code.find(close, m.end())
        return (n, n) if end < 0 else (end, end + len(close))

    while i < n:
        c = code[i]
        if code.startswith("--", i):
            m = _LONG_BRACKET.match(code, i + 2)
            if m:
                end = long_end(m)[1]
            else:
                end = code.find("\n", i)
                end = n if end < 0 else end
            out.append(blank(code[i:end]))
            i = end
            continue
        m = _LONG_BRACKET.match(code, i)
        if m or c in "'\"":
            if m:
                body_start, (body_end, end) = m.end(), long_end(m)
            else:
                j = i + 1
                while j < n and code[j] != c and code[j] != "\n":
                    j += 2 if code[j] == "\\" else 1
                end = min(j + 1, n)
                body_start, body_end = i + 1, max(i + 1, end - 1)
            if keep_strings:
                out.append(code[i:end])
            else:
                out.append(code[i:body_start] + blank(code[body_start:body_end])
                           + code[body_end:end])
            i = end
            continue
        out.append(c)
        i += 1
    return "".join(out)


def _byte_value(tok):
    tok = tok.strip()
    try:
        return int(tok, 16) if tok.lower().startswith("0x") else int(tok)
    except ValueError:
        return None  # a variable or an expression


def unterminated_sysex(code):
    """Literal SysEx messages (byte tables or hex strings) that start with F0 but don't end
    with F7. Pass code with comments stripped and strings kept."""
    bad = []
    for m in _MIDI_TABLE.finditer(code):
        items = [t for t in m.group(1).split(",") if t.strip()]
        if items and _byte_value(items[0]) == 0xF0 and _byte_value(items[-1]) != 0xF7:
            bad.append("{" + " ".join(m.group(1).split()) + "}")
    for m in _MIDI_HEX.finditer(code):
        hexs = m.group(2).replace(",", " ").split()
        if hexs and hexs[0].lower() == "f0" and hexs[-1].lower() != "f7":
            bad.append('"' + " ".join(hexs) + '"')
    return bad


def lua_binding_traps(code):
    """Warn about Lua that passes lint and loads (or looks like it would) but misbehaves.
    Comments and string contents are ignored, so a trap mentioned in a comment doesn't fire."""
    out = []
    with_strings = lua_strip(code, keep_strings=True)
    code = lua_strip(code)
    for rx, what in _POST_51:
        if rx.search(code):
            out.append(what)
    for frame in unterminated_sysex(with_strings):
        out.append(f"SysEx {frame[:60]} does not end with F7 -- nothing appends it, and real "
                   "MIDI drivers drop an unterminated frame")
    if _MB_SIZED_CTOR.search(code):
        out.append("MemoryBlock(x) with an argument -- if x is a number luabind picks the "
                   "table constructor and you get an EMPTY block; use MemoryBlock(n, true), "
                   "or pass a table. If this block is about to be loadFileAsData'd, plain "
                   "MemoryBlock() is correct -- that call APPENDS, so a sized block doubles")
    if _SETSIZE_1ARG.search(code):
        out.append("setSize(n) takes no default args in luabind -- call setSize(n, true)")
    if _GETNAME_TEXT.search(code):
        out.append("getName() returns a juce String userdata: '==' is always false and '..' "
                   "ABORTS the process; use getVstIndex(), or getName():toUTF8():getAddress()")
    return out


def Path_read_text(p):
    with open(p, "r", encoding="utf-8") as fh:
        return fh.read()


def cmd_lint(path):
    errors, warnings = [], []
    try:
        root = parse(path)
    except ET.ParseError as e:
        print(f"FAIL: XML is not well-formed: {e}")
        return 1
    except OSError as e:
        print(f"FAIL: cannot read {path}: {e}")
        return 1

    defined = {m.get("luaMethodName") for m in root.iter("luaMethod")}

    # check each defined method's inner function name matches its luaMethodName.
    # skip names that aren't valid Lua identifiers (e.g. library blobs like 'debugger.lua').
    for m in root.iter("luaMethod"):
        nm = m.get("luaMethodName")
        code = m.get("luaMethodCode", "")
        if m.get("luaMethodSource") == "1":
            # codeInFile: the code lives in <panel path minus extension>/lua/<luaMethodSourcePath>.
            # A missing file is NOT reported at load -- the panel loads with no complaint and the
            # method's globals are simply nil -- so check it here.
            rel = m.get("luaMethodSourcePath")
            if not rel:
                errors.append(f"method '{nm}': luaMethodSource='1' but no luaMethodSourcePath")
                continue
            if code:
                warnings.append(f"method '{nm}': has both luaMethodSourcePath and luaMethodCode; "
                                "CtrlrX reads the FILE and ignores the property")
            src = (rel if os.path.isabs(rel)
                   else os.path.join(os.path.splitext(str(path))[0], "lua", rel))
            try:
                code = Path_read_text(src)
            except OSError:
                errors.append(f"method '{nm}': luaMethodSourcePath '{rel}' does not resolve to a "
                              f"readable file ({src}) -- the panel will load silently and the "
                              "method's globals will be nil")
                continue
        if nm and code and nm.isidentifier() and (
                f"{nm} = function" not in code and f"function {nm}" not in code):
            warnings.append(f"method '{nm}': inner function name may not match luaMethodName")
        for trap in lua_binding_traps(code):
            warnings.append(f"method '{nm}': {trap}")

    seen_names, seen_vst = set(), {}
    mods = root.findall("modulator")
    for i, m in enumerate(mods):
        name = m.get("name")
        if not name:
            errors.append(f"modulator #{i} has no 'name'")
        elif name in seen_names:
            errors.append(f"duplicate modulator name: '{name}'")
        else:
            seen_names.add(name)

        comp = m.find("component")
        uiType = comp.get("uiType") if comp is not None else None
        if comp is None:
            errors.append(f"modulator '{name}' has no <component>")
        elif not uiType:
            errors.append(f"modulator '{name}' component has no uiType")
        
        midi = m.find("midi")
        if midi is None:
            if uiType != "uiLabel":
                warnings.append(f"modulator '{name}' has no <midi> child")
        elif comp is not None:
            for trap in midi_traps(m, comp, midi):
                warnings.append(trap)

        vst = m.get("vstIndex")
        if vst not in (None, "-1") and m.get("modulatorVstExported") == "1":
            if vst in seen_vst:
                errors.append(f"duplicate vstIndex {vst}: '{seen_vst[vst]}' and '{name}'")
            else:
                seen_vst[vst] = name

    # dangling Lua references: scan only the elements that *hold* callbacks
    # (panel root, modulators, their midi/component children) — never the
    # <luaManager> subtree, whose luaMethod* attributes are definitions, not references.
    holders = [root]
    for m in mods:
        holders.append(m)
        holders.extend(c for c in (m.find("component"), m.find("midi")) if c is not None)
    for elem in holders:
        for k, v in lua_callbacks(elem):
            if v not in defined:
                errors.append(f"<{elem.tag}> {k}='{v}' references method '{v}' not in <luaManager>")

    for w in warnings:
        print(f"WARN: {w}")
    for e in errors:
        print(f"ERROR: {e}")
    if errors:
        print(f"FAIL: {len(errors)} error(s), {len(warnings)} warning(s).")
        return 1
    print(f"OK: well-formed, {len(mods)} modulator(s), {len(defined)} lua method(s), "
          f"{len(warnings)} warning(s).")
    return 0


def attr_escape(text):
    """Escape a string so it is safe inside an XML attribute value.

    xml.sax escapes & < > ; attributes additionally need quotes and whitespace encoded,
    and CtrlrX itself writes newlines as &#10; (see panelAuthorDesc in any panel).
    Binary panels hand us raw, never-escaped strings, so dump output must go through this
    to stay pasteable into a .panel.
    """
    out = _xml_escape(text, {'"': "&quot;"})
    # Keep every byte: a CR (CRLF files, lone CRs) round-trips as &#13;, not as a newline.
    return (out.replace("\t", "&#9;").replace("\r", "&#13;").replace("\n", "&#10;"))


def cmd_escape(path):
    # newline="" / the raw buffer: no newline translation, so CRLF and lone CR survive.
    if path in (None, "-"):
        raw = sys.stdin.buffer.read().decode("utf-8")
    else:
        with open(path, "r", encoding="utf-8", newline="") as f:
            raw = f.read()
    sys.stdout.write(attr_escape(raw))
    return 0


def _pretty_element(elem, indent=0):
    """Serialize one element with one attribute per line; recurse into children.
    Used to surface a known-good element as a copy-template without re-reading the
    whole (huge) panel. Attribute order is preserved as parsed."""
    pad = "  " * indent
    lines = [f"{pad}<{elem.tag}"]
    for k, v in elem.attrib.items():
        lines.append(f'{pad}    {k}="{attr_escape(v)}"')
    kids = list(elem)
    if kids:
        lines.append(f"{pad}  >")
        for c in kids:
            lines.extend(_pretty_element(c, indent + 1))
        lines.append(f"{pad}</{elem.tag}>")
    else:
        lines.append(f"{pad}  />")
    return lines


def cmd_dump(path, selector):
    root = parse(path)
    if selector == "panel":
        print(f"<panel")
        for k, v in root.attrib.items():
            print(f'    {k}="{attr_escape(v)}"')
        print(">")
        return 0
    if selector == "editor":
        ed = root.find("uiPanelEditor")
        if ed is None:
            print("no <uiPanelEditor> in panel")
            return 1
        print("\n".join(_pretty_element(ed)))
        return 0
    if selector == "modulators":
        for m in root.findall("modulator"):
            comp = m.find("component")
            ui = comp.get("uiType", "?") if comp is not None else "?"
            print(f"{m.get('name','?')}\t{ui}")
        return 0
    if selector.startswith("modulator:"):
        want = selector.split(":", 1)[1]
        for m in root.findall("modulator"):
            if m.get("name") == want:
                print("\n".join(_pretty_element(m)))
                return 0
        print(f"no modulator named '{want}'")
        return 1
    print(f"unknown selector '{selector}' (use editor|panel|modulators|modulator:NAME)")
    return 2


def cmd_xml(path, out):
    root = parse(path)
    if os.path.abspath(out) == os.path.abspath(path):
        raise ValueError(f"{out}: refusing to overwrite the input; write to a new file")
    ET.ElementTree(root).write(out, encoding="UTF-8", xml_declaration=True)
    n_mod = len(root.findall("modulator"))
    n_res = len(list(root.iter("resource")))
    print(f"wrote {out}: XML, {n_mod} modulator(s), {n_res} resource node(s)")
    return 0


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    cmd, args = argv[1], argv[2:]
    if cmd in ("-h", "--help", "help"):
        print(__doc__)
        return 0
    try:
        if cmd == "summary":
            return cmd_summary(args[0])
        if cmd == "lint":
            return cmd_lint(args[0])
        if cmd == "escape":
            return cmd_escape(args[0] if args else None)
        if cmd == "dump":
            return cmd_dump(args[0], args[1])
        if cmd == "xml":
            return cmd_xml(args[0], args[1])
    except IndexError:
        print(f"missing argument for '{cmd}'\n\n{__doc__}")
        return 2
    except ET.ParseError as e:
        print(f"error: {args[0] if args else 'input'} is not well-formed XML: {e}")
        return 1
    except (ValueError, OSError) as e:
        print(f"error: {e}")
        return 1
    print(f"unknown command: {cmd}\n\n{__doc__}")
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
