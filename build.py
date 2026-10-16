#!/usr/bin/env python3
"""Assemble the modular readable DOS source and verify it against the supplied manifest.

Build dependencies: Python 3.10+ and x86 GNU binutils (as, objcopy).
No access to the original EXE, internet, linker, C compiler or Python packages
is needed. --reference additionally performs a direct byte-by-byte comparison.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parent


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def get_tool(name: str) -> str:
    """Resolve an executable name or an explicit path without using a shell."""
    found = shutil.which(name)
    if not found:
        raise RuntimeError(
            f"Tool not found: {name!r}. Install x86 GNU binutils, or pass "
            "--assembler /path/to/as and --objcopy /path/to/objcopy."
        )
    return found


def reference_bytes(path: Path) -> bytes:
    if not path.is_file():
        raise RuntimeError(f"Reference does not exist: {path}")
    if zipfile.is_zipfile(path):
        # Read the member; do not extract or execute any archive content.
        with zipfile.ZipFile(path) as archive:
            names = [name for name in archive.namelist()
                     if name.replace('\\', '/').rsplit('/', 1)[-1].upper() == 'GOOSE.EXE']
            if len(names) != 1:
                raise RuntimeError(f"Expected exactly one GOOSE.EXE in {path}, found {len(names)}")
            return archive.read(names[0])
    return path.read_bytes()


def verify(exe: Path, reference: Path | None = None) -> dict:
    data = exe.read_bytes()
    manifest = json.loads((ROOT / 'reference.json').read_text(encoding='utf-8'))
    instructions = json.loads((ROOT / 'analysis/instructions.json').read_text(encoding='utf-8'))
    expected = manifest['exe']
    problems: list[str] = []
    actual_sha = sha256(data)
    if len(data) != expected['size']:
        problems.append(f"Size mismatch: expected {expected['size']}, found {len(data)}")
    if actual_sha != expected['sha256']:
        problems.append('SHA-256 mismatch')

    instruction_mismatches = []
    for insn in instructions:
        start = insn['file_offset']
        wanted = bytes.fromhex(insn['bytes'])
        actual = data[start:start + len(wanted)]
        if actual != wanted:
            instruction_mismatches.append({
                'cs_offset': f"0x{insn['cs_offset']:04X}",
                'expected': wanted.hex(), 'actual': actual.hex(),
            })
    if instruction_mismatches:
        problems.append(f"{len(instruction_mismatches)} instruction encodings differ")

    mz = None
    if len(data) < 46 or data[:2] != b'MZ':
        problems.append('Missing/truncated MZ header')
    else:
        words = struct.unpack_from('<14H', data)
        declared_size = (words[2] - 1) * 512 + (words[1] or 512)
        header_bytes = words[4] * 16
        relocs = []
        if words[12] + 4 * words[3] > len(data):
            problems.append('Relocation table extends past end of file')
        else:
            for i in range(words[3]):
                off, seg = struct.unpack_from('<HH', data, words[12] + i * 4)
                relocs.append({'offset': off, 'segment': seg})
        mz = {
            'header_bytes': header_bytes, 'declared_file_size': declared_size,
            'cs': words[11], 'ip': words[10], 'ss': words[7], 'sp': words[8],
            'checksum_field': words[9], 'relocations': relocs,
        }
        wanted_relocs = [{'offset': off, 'segment': 0}
                         for off in (0x11, 0x4d1, 0x5d8, 0xb59)]
        if relocs != wanted_relocs:
            problems.append('MZ relocation table differs')
        if declared_size != len(data) or header_bytes != 512:
            problems.append('MZ file/header size fields are inconsistent')
        if (words[11], words[10], words[7], words[8]) != (0, 16, 0xcfc, 0x100):
            problems.append('Initial CS:IP / SS:SP differs')
        if words[9] != 0xe069:
            problems.append('Original MZ checksum field was changed')
        for relocation in wanted_relocs:
            pos = 512 + relocation['offset']
            if data[pos:pos+2] != b'\xa5\x03':
                problems.append(f"Relocation operand at file offset {pos:#x} differs")

    assets = {}
    # Check pristine bundled assets, not writable runtime copies in build/.
    for name, expected_asset in manifest['assets'].items():
        path = ROOT / 'assets' / name
        if not path.is_file():
            assets[name] = {'match': False, 'error': 'missing'}
            problems.append(f'Missing bundled asset: {name}')
            continue
        raw = path.read_bytes()
        match = len(raw) == expected_asset['size'] and sha256(raw) == expected_asset['sha256']
        assets[name] = {'size': len(raw), 'sha256': sha256(raw), 'match': match}
        if not match:
            problems.append(f'Bundled asset differs: {name}')

    comparison = None
    if reference is not None:
        original = reference_bytes(reference)
        differences = sum(a != b for a, b in zip(original, data)) + abs(len(original) - len(data))
        comparison = {
            'reference_name': reference.name,
            'reference_size': len(original), 'reference_sha256': sha256(original),
            'differing_bytes': differences, 'equal': original == data,
        }
        if differences:
            problems.append(f'Direct comparison: {differences} differing bytes')

    return {
        'status': 'PASS' if not problems else 'FAIL',
        'actual_size': len(data), 'expected_size': expected['size'],
        'actual_sha256': actual_sha, 'expected_sha256': expected['sha256'],
        'instructions_checked': len(instructions),
        'instruction_encoding_mismatch_count': len(instruction_mismatches),
        'instruction_encoding_mismatch_examples': instruction_mismatches[:20],
        'mz': mz, 'bundled_assets': assets, 'direct_comparison': comparison,
        'problems': problems,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--assembler', default=os.environ.get('GOOSE_AS', 'as'))
    parser.add_argument('--objcopy', default=os.environ.get('GOOSE_OBJCOPY', 'objcopy'))
    parser.add_argument('--reference', type=Path, help='Original GOOSE.EXE or goose.zip (optional)')
    parser.add_argument('--out', type=Path, default=ROOT / 'build', help='Output directory')
    parser.add_argument('--verify-only', action='store_true', help='Verify an existing output; do not assemble')
    parser.add_argument('--no-verify', action='store_true', help='Build an intentionally modified source without claiming a byte-exact match')
    args = parser.parse_args()
    output = args.out.resolve()
    output.mkdir(parents=True, exist_ok=True)
    exe = output / 'GOOSE.EXE'
    versions = {}
    if not args.verify_only:
        assembler, objcopy = get_tool(args.assembler), get_tool(args.objcopy)
        for name, command in [('assembler', assembler), ('objcopy', objcopy)]:
            versions[name] = subprocess.check_output([command, '--version'], text=True).splitlines()[0]
        obj = output / 'goose.o'
        subprocess.run([assembler, '--32', '-o', str(obj), str(ROOT / 'src/GOOSE.asm')],
                       cwd=ROOT, check=True)
        subprocess.run([objcopy, '-O', 'binary', '-j', '.text', str(obj), str(exe)],
                       cwd=ROOT, check=True)
        # Do not overwrite an existing runtime HISCORES or user-modified asset.
        for asset in (ROOT / 'assets').iterdir():
            target = output / asset.name
            if asset.is_file() and not target.exists():
                shutil.copyfile(asset, target)
    if not exe.is_file():
        raise RuntimeError(f'No output executable found: {exe}')
    if args.no_verify:
        print(f'BUILT, NOT VERIFIED: {exe}')
        print('This mode does not assert byte-exactness or correct behavior after source edits.')
        return 0

    report = verify(exe, args.reference.resolve() if args.reference else None)
    report['toolchain'] = versions
    report['source_files_sha256'] = {str(p.relative_to(ROOT)): sha256(p.read_bytes())
                                     for p in sorted((ROOT / 'src').rglob('*')) if p.is_file()}
    report_file = output / 'verification.json'
    report_file.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(f"{report['status']}: {exe.name} ({report['actual_size']} bytes)")
    print(f"SHA256: {report['actual_sha256']}")
    print(f"Instruction encodings: {report['instructions_checked']} checked, "
          f"{report['instruction_encoding_mismatch_count']} mismatches")
    if report['direct_comparison']:
        print(f"Direct reference comparison: {report['direct_comparison']['differing_bytes']} differing bytes")
    print(f'Report: {report_file}')
    for problem in report['problems']:
        print(f'ERROR: {problem}', file=sys.stderr)
    return 0 if report['status'] == 'PASS' else 1


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.CalledProcessError,
            ValueError, zipfile.BadZipFile) as error:
        print(f'ERROR: {error}', file=sys.stderr)
        raise SystemExit(2)
