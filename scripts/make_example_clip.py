"""Writes examples/animation/clip.avi: a teal spinner with an amber head.

An 8-bit run-length-encoded AVI (the system animation control plays
uncompressed and run-length-encoded clips only). The background is the panel
color of the examples (#1B2033), so the clip melts into its card; the spinner
sits in the middle of a larger field of that color, so a control smaller than
the clip shows the middle of it at any screen scaling.

Usage: make_example_clip.py <output.avi>   (needs Pillow)
"""
import math
import struct
import sys

from PIL import Image, ImageChops, ImageDraw

out = sys.argv[1]
SIZE = 192
SCALE = 4
FRAMES = 24
DOTS = 12
PANEL = (0x1B, 0x20, 0x33)
TEAL = (0x3D, 0xD6, 0xC6)
AMBER = (0xFF, 0xB4, 0x54)
LEVELS = 16


def blend(a, b, t):
    return tuple(int(round(a[i] + (b[i] - a[i]) * t)) for i in range(3))


# Palette: 16 steps panel -> teal, 16 steps panel -> amber, rest panel.
palette = [blend(PANEL, TEAL, i / (LEVELS - 1)) for i in range(LEVELS)]
palette += [blend(PANEL, AMBER, i / (LEVELS - 1)) for i in range(LEVELS)]
palette += [PANEL] * (256 - len(palette))


def frame(step):
    big = SIZE * SCALE
    teal = Image.new("L", (big, big), 0)
    amber = Image.new("L", (big, big), 0)
    centre = big / 2
    radius = 33 * SCALE
    for k in range(DOTS):
        # k = 0 is the head; later dots trail behind it.
        angle = 2 * math.pi * (step / FRAMES - k / DOTS) - math.pi / 2
        x = centre + radius * math.cos(angle)
        y = centre + radius * math.sin(angle)
        level = max(0.18, 1 - k / (DOTS * 0.85))
        r = (7.5 - 3.0 * k / DOTS) * SCALE
        layer = Image.new("L", (big, big), 0)
        ImageDraw.Draw(layer).ellipse((x - r, y - r, x + r, y + r),
                                      fill=int(255 * level))
        target = amber if k == 0 else teal
        target.paste(ImageChops.lighter(target, layer))
    teal = teal.resize((SIZE, SIZE), Image.LANCZOS)
    amber = amber.resize((SIZE, SIZE), Image.LANCZOS)
    tp, ap = teal.load(), amber.load()
    rows = []
    for y in range(SIZE - 1, -1, -1):  # a DIB is stored bottom-up
        row = bytearray()
        for x in range(SIZE):
            t, a = tp[x, y], ap[x, y]
            if a > t:
                row.append(LEVELS + round(a / 255 * (LEVELS - 1)))
            else:
                row.append(round(t / 255 * (LEVELS - 1)))
        rows.append(bytes(row))
    return rows


def rle8(rows):
    """Run-length encodes bottom-up rows (BI_RLE8, encoded mode only)."""
    out = bytearray()
    for row in rows:
        x = 0
        while x < len(row):
            run = 1
            while x + run < len(row) and row[x + run] == row[x] and run < 255:
                run += 1
            out += bytes((run, row[x]))
            x += run
        out += bytes((0, 0))  # end of line
    out[-2:] = bytes((0, 1))  # end of bitmap replaces the last end of line
    return bytes(out)


frames = [rle8(frame(i)) for i in range(FRAMES)]
image_size = max(len(f) for f in frames)


def chunk(tag, data):
    pad = b"\0" if len(data) % 2 else b""
    return tag + struct.pack("<I", len(data)) + data + pad


def lst(kind, body):
    return chunk(b"LIST", kind + body)


usec = 50000  # 20 frames per second
avih = struct.pack("<IIIIIIIIII4I", usec, image_size * 20, 0, 0x10, FRAMES,
                   0, 1, image_size, SIZE, SIZE, 0, 0, 0, 0)
strh = struct.pack("<4s4sIHHIIIIIIIIhhhh", b"vids", b"RLE ", 0, 0, 0, 0, 1,
                   20, 0, FRAMES, image_size, 0xFFFFFFFF, 0, 0, 0, SIZE, SIZE)
bih = struct.pack("<IiiHHIIiiII", 40, SIZE, SIZE, 1, 8, 1, image_size, 0, 0,
                  256, 0)
quads = b"".join(bytes((b, g, r, 0)) for (r, g, b) in palette)
strf = bih + quads
hdrl = lst(b"hdrl", chunk(b"avih", avih) +
           lst(b"strl", chunk(b"strh", strh) + chunk(b"strf", strf)))
movi_body = b"".join(chunk(b"00dc", f) for f in frames)
movi = lst(b"movi", movi_body)
index = b""
offset = 4
for f in frames:
    index += struct.pack("<4sIII", b"00dc", 0x10, offset, len(f))
    offset += 8 + len(f)
avi = b"AVI " + hdrl + movi + chunk(b"idx1", index)
open(out, "wb").write(b"RIFF" + struct.pack("<I", len(avi)) + avi)
print(len(avi) + 8, "bytes")
