#!/usr/bin/env python3
"""Read pixels out of a CtrlrPanelProbe PNG (screenshot / paint "file"), standard library only.

Command line:

    python3 png_sample.py FILE X Y                       # prints "rrggbbaa" for pixel (X, Y)
    python3 png_sample.py FILE X Y --expect 2060a0 [--tol 2]
                                                         # exit 0 if it matches, 1 if not
    (exit 2: unreadable file, unsupported PNG, or (X, Y) outside the image)

As a module:

    from png_sample import load, pixel, close
    img = load("shot.png")
    assert close(pixel(img, 120, 40), (0x20, 0x60, 0xa0, 0xff), tol=2)

For "is this area the colour I set" checks: sample a solid fill away from text, edges and
anti-aliasing, and compare with a small tolerance. Not for comparing whole images -- fonts and
anti-aliasing differ between platforms (see Doc/PanelProbe.md).

Decodes what the probe writes: 8-bit, non-interlaced truecolour (RGB) or truecolour + alpha (RGBA);
also 8-bit greyscale (+ alpha). Anything else raises ValueError rather than guessing.
"""

import argparse
import struct
import sys
import zlib

_SIGNATURE = b"\x89PNG\r\n\x1a\n"
_CHANNELS = {0: 1, 2: 3, 4: 2, 6: 4}  # PNG colour type -> samples per pixel


class Png:
    """A decoded image: `width`, `height`, `channels`, and `rows` (one bytes object per row)."""

    def __init__(self, width, height, colour_type, rows):
        self.width = width
        self.height = height
        self.colour_type = colour_type
        self.channels = _CHANNELS[colour_type]
        self.rows = rows


def _paeth(a, b, c):
    p = a + b - c
    pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    return b if pb <= pc else c


def _unfilter(data, width, height, bpp):
    stride = width * bpp
    rows, prev, pos = [], bytearray(stride), 0
    for _ in range(height):
        ftype = data[pos]
        line = bytearray(data[pos + 1:pos + 1 + stride])
        pos += 1 + stride
        if ftype == 1:
            for i in range(bpp, stride):
                line[i] = (line[i] + line[i - bpp]) & 0xFF
        elif ftype == 2:
            for i in range(stride):
                line[i] = (line[i] + prev[i]) & 0xFF
        elif ftype == 3:
            for i in range(stride):
                left = line[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + ((left + prev[i]) >> 1)) & 0xFF
        elif ftype == 4:
            for i in range(stride):
                left = line[i - bpp] if i >= bpp else 0
                up_left = prev[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + _paeth(left, prev[i], up_left)) & 0xFF
        elif ftype != 0:
            raise ValueError(f"bad PNG filter type {ftype}")
        rows.append(bytes(line))
        prev = line
    return rows


def load(path):
    """Decode a PNG file into a Png."""
    with open(path, "rb") as f:
        blob = f.read()
    if blob[:8] != _SIGNATURE:
        raise ValueError(f"{path}: not a PNG")
    pos, header, idat = 8, None, []
    while pos < len(blob):
        (length,) = struct.unpack(">I", blob[pos:pos + 4])
        ctype = blob[pos + 4:pos + 8]
        body = blob[pos + 8:pos + 8 + length]
        pos += 12 + length
        if ctype == b"IHDR":
            header = struct.unpack(">IIBBBBB", body)
        elif ctype == b"IDAT":
            idat.append(body)
        elif ctype == b"IEND":
            break
    if header is None:
        raise ValueError(f"{path}: no IHDR chunk")
    width, height, depth, colour_type, _, _, interlace = header
    if depth != 8 or colour_type not in _CHANNELS or interlace != 0:
        raise ValueError(f"{path}: unsupported PNG (bit depth {depth}, colour type {colour_type}, "
                         f"interlace {interlace}); only 8-bit non-interlaced grey/RGB(A) is decoded")
    rows = _unfilter(zlib.decompress(b"".join(idat)), width, height, _CHANNELS[colour_type])
    return Png(width, height, colour_type, rows)


def pixel(img, x, y):
    """The pixel at (x, y) as (r, g, b, a); images without alpha report a = 255."""
    if not (0 <= x < img.width and 0 <= y < img.height):
        raise IndexError(f"({x}, {y}) is outside the {img.width} x {img.height} image")
    n = img.channels
    s = img.rows[y][x * n:(x + 1) * n]
    if n == 4:
        return tuple(s)
    if n == 3:
        return (s[0], s[1], s[2], 255)
    if n == 2:
        return (s[0], s[0], s[0], s[1])
    return (s[0], s[0], s[0], 255)


def close(a, b, tol=2):
    """True when every channel of colours a and b differs by at most tol. A 3-tuple is opaque."""
    a = tuple(a) + (255,) * (4 - len(a))
    b = tuple(b) + (255,) * (4 - len(b))
    return all(abs(p - q) <= tol for p, q in zip(a, b))


def _parse_colour(text):
    h = text.lower()
    h = h[2:] if h.startswith("0x") else h.lstrip("#")
    if len(h) not in (6, 8) or any(c not in "0123456789abcdef" for c in h):
        raise argparse.ArgumentTypeError(f"colour must be rrggbb or rrggbbaa hex, got {text!r}")
    return tuple(int(h[i:i + 2], 16) for i in range(0, len(h), 2))


def main(argv=None):
    ap = argparse.ArgumentParser(
        description="Print or check one pixel of a CtrlrPanelProbe PNG (screenshot or paint "
                    "file). Sample solid fills away from text, edges and anti-aliasing.")
    ap.add_argument("file", help="PNG written by the probe's screenshot or paint action")
    ap.add_argument("x", type=int, help="column, 0 = left edge of the image")
    ap.add_argument("y", type=int, help="row, 0 = top edge of the image")
    ap.add_argument("--expect", type=_parse_colour, metavar="RRGGBB[AA]",
                    help="expected colour; alpha defaults to ff. Exit 1 if the pixel differs")
    ap.add_argument("--tol", type=int, default=2,
                    help="per-channel tolerance for --expect (default 2)")
    args = ap.parse_args(argv)
    try:
        got = pixel(load(args.file), args.x, args.y)
    except (OSError, ValueError, IndexError, zlib.error) as e:
        print(f"error: {e}", file=sys.stderr)
        return 2
    text = "".join(f"{c:02x}" for c in got)
    if args.expect is None:
        print(text)
        return 0
    want = "".join(f"{c:02x}" for c in tuple(args.expect) + (255,) * (4 - len(args.expect)))
    if close(got, args.expect, args.tol):
        print(f"OK {text} (expected {want}, tol {args.tol})")
        return 0
    print(f"MISMATCH at ({args.x}, {args.y}): got {text}, expected {want} (tol {args.tol})")
    return 1


if __name__ == "__main__":
    sys.exit(main())
