import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
levels = json.loads((root / "analysis" / "levels.json").read_text(encoding="utf-8"))
if len(levels) != 24:
    raise SystemExit("expected 24 map parts")

lines = ['#include "maps.h"', ""]
for index, item in enumerate(levels):
    rows = item["stored_rows"]
    if len(rows) != 64 or any(len(row) != 12 for row in rows):
        raise SystemExit(f"bad tiles in part {index}")
    flat = [tile for row in rows for tile in row]
    lines.append(f"static const uint8_t tiles_{index:02d}[768] = {{")
    for offset in range(0, 768, 16):
        chunk = ", ".join(str(tile) for tile in flat[offset:offset + 16])
        lines.append("    " + chunk + ("," if offset + 16 < 768 else ""))
    lines.append("};")
    lines.append(f"static const GooseMapObject objects_{index:02d}[] = {{")
    for obj in item["objects"]:
        lines.append("    {%d, %d, %d}," % (obj["kind"], obj["column_index"], obj["stored_row"]))
    lines.append("};")

lines.append("const GooseMap goose_maps[24] = {")
for index, item in enumerate(levels):
    lines.append(
        "    {tiles_%02d, objects_%02d, %d}," % (index, index, len(item["objects"]))
    )
lines.append("};")
(root / "port" / "maps.c").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
print("parts", len(levels), "objects", sum(len(item["objects"]) for item in levels))
