#!/usr/bin/env python3
"""Expand .include directives. Does not read EXE, manifests, or instruction bytes."""
from __future__ import annotations
import argparse
from pathlib import Path
import re
ROOT = Path(__file__).resolve().parents[1]
INCLUDE = re.compile(r'^\s*\.include\s+"([^\"]+)"(?:\s*#.*)?$')

def expand(path: Path, stack: tuple[Path, ...] = ()) -> str:
    path = path.resolve()
    if path in stack:
        raise ValueError(f"Cyclic include: {path}")
    if not path.is_relative_to(ROOT):
        raise ValueError(f"Include outside project: {path}")
    result = []
    for line in path.read_text(encoding='utf-8').splitlines():
        match = INCLUDE.fullmatch(line)
        if match:
            child = ROOT / match.group(1)
            result.append(f"/* BEGIN {match.group(1)} */")
            result.append(expand(child, stack + (path,)))
            result.append(f"/* END {match.group(1)} */")
        else:
            result.append(line)
    return '\n'.join(result) + '\n'

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'dist/GOOSE.flat.asm')
    args = parser.parse_args()
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(expand(ROOT / 'src/GOOSE.asm'), encoding='utf-8')
    print(args.out)

if __name__ == '__main__':
    main()
