#!/usr/bin/env python3
"""Pack every original sprite and map tile into one PNG atlas plus an index.

Source pixels stay packed 4bpp in the .X banks. The atlas gives each sprite
a rectangle, anchor, and transparent color in a form the game can draw.
"""
from __future__ import annotations

import json
import re
import struct
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INC = ROOT / "src" / "data" / "01_strings_templates_tables.inc"
BANKS = ("BIRD1", "BIRD2", "BLOX", "NEWBIRD", "PODZ1", "INTRO")
EGA = (
    (0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170),
    (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
    (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255),
    (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255),
)
ATLAS_W = 1024
PADDING = 1

NUMBER = re.compile(r"-?0x[0-9A-Fa-f]+|-?\d+")


def signed8(value: int) -> int:
    value &= 255
    return value if value < 128 else value - 256


def numbers(line: str) -> list[int]:
    body = line.split("#", 1)[0]
    if ".byte" not in body and ".word" not in body:
        return []
    return [int(token, 0) for token in NUMBER.findall(body)]


def decode_bank(name: str) -> list[int]:
    data = (ROOT / "assets" / f"{name}.X").read_bytes()
    if len(data) != 32000:
        raise SystemExit(f"{name}.X is {len(data)} bytes")
    pixels = [0] * (320 * 200)
    for y in range(200):
        row = data[y * 160:(y + 1) * 160]
        for x in range(320):
            byte = row[x >> 1]
            pixels[y * 320 + x] = (byte >> 4) if (x & 1) == 0 else (byte & 15)
    return pixels


def write_png(path: Path, width: int, height: int, rgba: bytes) -> None:
    def chunk(tag: bytes, payload: bytes) -> bytes:
        return struct.pack(">I", len(payload)) + tag + payload + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF)

    rows = bytearray()
    stride = width * 4
    for y in range(height):
        rows.append(0)
        rows.extend(rgba[y * stride:(y + 1) * stride])
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(rows), 9))
    png += chunk(b"IEND", b"")
    path.write_bytes(png)


def collect(lines: list[str]) -> list[dict]:
    sprites: list[dict] = []

    def add(name: str, bank: int, offset: int, width: int, height: int, ax: int, ay: int, key: int) -> None:
        if width <= 0 or height <= 0:
            return
        index = bank // 2
        if index < 0 or index >= len(BANKS):
            raise SystemExit(f"{name} bank {bank} is outside the six files")
        sprites.append({
            "name": name,
            "bank": index,
            "source": BANKS[index],
            "offset": offset & 0xFFFF,
            "w": width,
            "h": height,
            "anchor_x": signed8(ax),
            "anchor_y": signed8(ay),
            "key": key & 15,
        })

    for i, line in enumerate(lines):
        frame = re.search(r"FrameSet ([0-9A-Fa-f]+): (\d+) frames", line)
        if frame:
            ident, count = frame.group(1).upper(), int(frame.group(2))
            nums: list[int] = []
            j = i + 1
            while j < len(lines) and not lines[j].lstrip().startswith("/*"):
                nums.extend(numbers(lines[j]))
                j += 1
            nums = nums[1:]
            need = count * 6
            if len(nums) < need:
                raise SystemExit(f"FrameSet {ident} has {len(nums)} numbers, expected {need}")
            widths = nums[0:count]
            heights = nums[count:count * 2]
            ax = nums[count * 2:count * 3]
            ay = nums[count * 3:count * 4]
            pairs = nums[count * 4:need]
            prefix = "ship" if ident == "0D49" else f"fs_{ident}"
            for n in range(count):
                add(f"{prefix}_{n:02d}", pairs[n * 2], pairs[n * 2 + 1], widths[n], heights[n], ax[n], ay[n], 2)
            continue
        template = re.search(r"Entity template: ([A-Za-z0-9_]+)", line)
        if template:
            groups = []
            j = i + 1
            while j < len(lines) and not lines[j].lstrip().startswith("/*"):
                found = numbers(lines[j])
                if found:
                    groups.append(found)
                j += 1
            if len(groups) < 5:
                continue
            width, height, ax, ay = groups[2][:4]
            bank, offset = groups[3][:2]
            key = groups[4][0]
            add(template.group(1), bank, offset, width, height, ax, ay, key)

    heights = block(lines, "DS:051D", 42)
    offsets = block(lines, "DS:05EF", 42)
    for n in range(42):
        bank = 4 if n < 0x26 else 2
        add(f"tile_{n:02d}", bank, offsets[n], 24, heights[n], 0, 0, 0)
    return sprites


def block(lines: list[str], marker: str, count: int) -> list[int]:
    nums: list[int] = []
    seen = False
    for line in lines:
        if not seen and marker in line:
            seen = True
            continue
        if not seen:
            continue
        if line.lstrip().startswith("/*") and nums:
            break
        nums.extend(numbers(line))
        if len(nums) >= count:
            return nums[:count]
    raise SystemExit(f"could not read {count} values at {marker}")


def pack(sprites: list[dict]) -> tuple[int, int]:
    order = sorted(range(len(sprites)), key=lambda i: sprites[i]["h"], reverse=True)
    x = y = row_h = 0
    placed: dict[tuple, tuple[int, int]] = {}
    for index in order:
        sprite = sprites[index]
        identity = (sprite["bank"], sprite["offset"], sprite["w"], sprite["h"], sprite["key"])
        if identity in placed:
            sprite["x"], sprite["y"] = placed[identity]
            continue
        if x > 0 and x + sprite["w"] > ATLAS_W:
            y += row_h + PADDING
            x = 0
            row_h = 0
        if sprite["w"] > ATLAS_W:
            raise SystemExit(f"{sprite['name']} is wider than the atlas")
        sprite["x"], sprite["y"] = x, y
        placed[identity] = (x, y)
        x += sprite["w"] + PADDING
        row_h = max(row_h, sprite["h"])
    return ATLAS_W, y + row_h


def paint(sprites: list[dict], banks: list[list[int]], width: int, height: int) -> bytes:
    image = bytearray(width * height * 4)
    seen: set[tuple] = set()
    for sprite in sprites:
        slot = (sprite["x"], sprite["y"], sprite["w"], sprite["h"])
        if slot in seen:
            continue
        seen.add(slot)
        source = banks[sprite["bank"]]
        origin_x = (sprite["offset"] % 160) * 2
        origin_y = sprite["offset"] // 160
        for dy in range(sprite["h"]):
            for dx in range(sprite["w"]):
                sx = origin_x + dx
                sy = origin_y + dy
                if sx < 0 or sy < 0 or sx >= 320 or sy >= 200:
                    continue
                color = source[sy * 320 + sx]
                if color == sprite["key"]:
                    continue
                at = ((sprite["y"] + dy) * width + sprite["x"] + dx) * 4
                image[at:at + 4] = bytes((*EGA[color], 255))
    return bytes(image)


def write_c(sprites: list[dict]) -> None:
    def row(sprite: dict) -> str:
        return "    {%d, %d, %d, %d, %d, %d}," % (
            sprite["x"], sprite["y"], sprite["w"], sprite["h"], sprite["anchor_x"], sprite["anchor_y"])

    tiles = [s for s in sprites if s["name"].startswith("tile_")]
    ship = [s for s in sprites if s["name"].startswith("ship_")]
    tiles.sort(key=lambda s: s["name"])
    ship.sort(key=lambda s: s["name"])
    lines = [
        '#include "atlas.h"',
        f"const AtlasSprite atlas_tiles[{len(tiles)}] = {{",
        * (row(s) for s in tiles),
        "};",
        f"const AtlasSprite atlas_ship[{len(ship)}] = {{",
        * (row(s) for s in ship),
        "};",
        f"const AtlasSprite atlas_sprites[{len(sprites)}] = {{",
        * (row(s) for s in sprites),
        "};",
        "const char *const atlas_sprite_names[] = {",
    ]
    for sprite in sprites:
        lines.append(f'    "{sprite["name"]}",')
    lines.append("};")
    lines.append(f"const int atlas_sprite_count = {len(sprites)};")
    (ROOT / "port" / "atlas_data.c").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> None:
    lines = INC.read_text(encoding="utf-8").splitlines()
    sprites = collect(lines)
    width, height = pack(sprites)
    banks = [decode_bank(name) for name in BANKS]
    out = ROOT / "assets" / "atlas"
    out.mkdir(parents=True, exist_ok=True)
    write_png(out / "goose.png", width, height, paint(sprites, banks, width, height))
    catalog = {
        "image": "goose.png",
        "width": width,
        "height": height,
        "sprites": [
            {key: sprite[key] for key in (
                "name", "x", "y", "w", "h", "anchor_x", "anchor_y", "source", "offset", "key")}
            for sprite in sprites
        ],
    }
    (out / "goose.json").write_text(json.dumps(catalog, indent=2) + "\n", encoding="utf-8")
    write_c(sprites)
    unique = len({(s["x"], s["y"], s["w"], s["h"]) for s in sprites})
    print(f"{len(sprites)} names, {unique} rectangles, {width}x{height}")


if __name__ == "__main__":
    main()
