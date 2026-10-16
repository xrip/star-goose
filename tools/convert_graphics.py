#!/usr/bin/env python3
"""Turn the original 32000-byte .X banks into PNG.

Sprite banks are packed 4bpp, stride 160, high nibble first.
INTRO.X is four EGA planes of 8000 bytes, written intensity, red, green, blue.
"""
from __future__ import annotations

import struct
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "assets"
OUTPUT = ROOT / "assets" / "png"

EGA = (
    (0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170),
    (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
    (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255),
    (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255),
)
WIDTH = 320
HEIGHT = 200
PACKED_STRIDE = 160
PLANE_BYTES = 8000


def write_png(path: Path, pixels: bytes) -> None:
    def chunk(tag: bytes, data: bytes) -> bytes:
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    rows = bytearray()
    stride = WIDTH * 3
    for y in range(HEIGHT):
        rows.append(0)
        rows.extend(pixels[y * stride:(y + 1) * stride])
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", WIDTH, HEIGHT, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(rows), 9))
    png += chunk(b"IEND", b"")
    path.write_bytes(png)


def paint(indices: list[int]) -> bytes:
    out = bytearray(WIDTH * HEIGHT * 3)
    for i, color in enumerate(indices):
        out[i * 3:i * 3 + 3] = bytes(EGA[color & 15])
    return bytes(out)


def decode_packed(data: bytes) -> bytes:
    indices = [0] * (WIDTH * HEIGHT)
    for y in range(HEIGHT):
        row = data[y * PACKED_STRIDE:(y + 1) * PACKED_STRIDE]
        for x in range(WIDTH):
            byte = row[x >> 1]
            indices[y * WIDTH + x] = (byte >> 4) if (x & 1) == 0 else (byte & 15)
    return paint(indices)


def decode_intro(data: bytes) -> bytes:
    planes = [data[i * PLANE_BYTES:(i + 1) * PLANE_BYTES] for i in range(4)]
    indices = [0] * (WIDTH * HEIGHT)
    for y in range(HEIGHT):
        for x in range(WIDTH):
            byte_index = y * 40 + (x >> 3)
            bit = 7 - (x & 7)
            color = 0
            for plane, shift in enumerate((3, 2, 1, 0)):
                if planes[plane][byte_index] & (1 << bit):
                    color |= 1 << shift
            indices[y * WIDTH + x] = color
    return paint(indices)


def main() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for name in ("BIRD1", "BIRD2", "BLOX", "NEWBIRD", "PODZ1"):
        data = (SOURCE / f"{name}.X").read_bytes()
        if len(data) != 32000:
            raise SystemExit(f"{name}.X is {len(data)} bytes, expected 32000")
        write_png(OUTPUT / f"{name}.png", decode_packed(data))
        print(f"{name}.png")
    intro = (SOURCE / "INTRO.X").read_bytes()
    if len(intro) != 32000:
        raise SystemExit(f"INTRO.X is {len(intro)} bytes, expected 32000")
    write_png(OUTPUT / "INTRO.png", decode_intro(intro))
    print("INTRO.png")


if __name__ == "__main__":
    main()
