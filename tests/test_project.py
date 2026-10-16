#!/usr/bin/env python3
"""Dependency-free structural and byte-exact tests for readable ASM reconstruction."""
from __future__ import annotations
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys
import tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
    subprocess.run([sys.executable,str(ROOT/'build.py')],check=True)
    exe=(ROOT/'build/GOOSE.EXE').read_bytes();ds=exe[0x3c50:]
    insns=json.loads((ROOT/'analysis/instructions.json').read_text())
    funcs=json.loads((ROOT/'analysis/functions.json').read_text())
    levels=json.loads((ROOT/'analysis/levels.json').read_text())
    summary=json.loads((ROOT/'analysis/reconstruction_summary.json').read_text())
    sources={str(p.relative_to(ROOT)):p.read_text(encoding='utf-8').splitlines()
             for p in (ROOT/'src').rglob('*') if p.is_file()}
    seen=set()
    for q in insns:
        a=q['cs_offset'];assert a not in seen;seen.add(a)
        text=sources[q['source_file']][q['source_line']-1]
        assert f'# CS:{a:04X}' in text,(a,q['source_file'],q['source_line'])
        assert text.split('# CS:',1)[0].strip()==q['assembly']
        assert exe[q['file_offset']:q['file_offset']+q['size']]==bytes.fromhex(q['bytes'])
    assert len(seen)==6022
    assert len(funcs)==204 and len({x['name'] for x in funcs})==204
    for f in funcs:
        assert sources[f['source_file']][f['source_line']-1].strip()==f['name']+':'
        assert f['purpose'] and f['inputs'] and f['effects']
        assert not f['name'].startswith('sub_')
    allcode='\n'.join('\n'.join(v) for k,v in sources.items() if k.startswith('src/code/'))
    assert '[si+D_msg_insufficient_ram]' not in allcode
    assert '[di+D_msg_insufficient_ram]' not in allcode
    assert '.incbin' not in '\n'.join('\n'.join(v) for v in sources.values()).lower()
    assert not any(ds[0x1b8a:0x1fda])
    assert 0x1fda-0x1b8a==48*23 and ds[0x1fda:0x1fdc]==b'\x80\x00'
    # Re-encode every original token, not a newly optimized RLE stream.
    rle_count=0;placement_count=0
    for level in levels:
        tokens=bytes(level['tile_rle']);p=level['tile_stream_ds']
        assert ds[p:p+len(tokens)]==tokens
        decoded=[]
        for token in tokens:
            tile=token&63;count=(token>>6)+1
            assert 0<=tile<42 and 1<=count<=4
            assert ((count-1)<<6)|tile==token
            decoded.extend([tile]*count)
        assert len(decoded)==768
        assert [decoded[i:i+12] for i in range(0,768,12)]==level['stored_rows']
        objects=bytearray()
        for x in level['objects']:
            assert 0<=x['column_index']<10 and 0<=x['source_row']<64
            assert x['stored_row']==63-x['source_row']
            objects.extend((x['kind'],x['column_index'],x['source_row']))
        objects.append(255);p=level['object_stream_ds']
        assert ds[p:p+len(objects)]==objects
        rle_count+=len(tokens);placement_count+=len(level['objects'])
    handlers=json.loads((ROOT/'analysis/handler_table.json').read_text())
    for h in handlers:
        ofs=struct.unpack_from('<H',exe,512+0x105a+2*(h['id']-1))[0]
        assert ofs==h['cs_offset'] and h['symbol'] in {f['name'] for f in funcs}
    subprocess.run([sys.executable,str(ROOT/'tools/export_flat.py')],check=True)
    as_tool=shutil.which('as');objcopy=shutil.which('objcopy')
    if not as_tool or not objcopy:raise RuntimeError('GNU as/objcopy not found')
    with tempfile.TemporaryDirectory(prefix='goose-flat-') as td:
        p=Path(td);shutil.copy2(ROOT/'dist/GOOSE.flat.asm',p/'GOOSE.asm')
        # Only the flat source is present in this folder. No EXE/data/includes are needed.
        subprocess.run([as_tool,'--32','-o',str(p/'goose.o'),str(p/'GOOSE.asm')],cwd=p,check=True)
        subprocess.run([objcopy,'-O','binary','-j','.text',str(p/'goose.o'),str(p/'GOOSE.EXE')],cwd=p,check=True)
        assert (p/'GOOSE.EXE').read_bytes()==exe
        readelf=shutil.which('readelf')
        reloc_report=None
        if readelf:
            reloc_report=subprocess.check_output([readelf,'-r',str(p/'goose.o')],text=True)
            assert 'There are no relocations' in reloc_report
    result={'status':'PASS','binary_sha256':hashlib.sha256(exe).hexdigest(),
        'binary_size':len(exe),'instruction_source_links_checked':len(insns),
        'entry_contracts_checked':len(funcs),'handler_entries_checked':len(handlers),
        'level_parts_checked':len(levels),'rle_tokens_roundtripped':rle_count,
        'placements_roundtripped':placement_count,'flat_build_in_isolated_directory':'PASS',
        'object_relocations':reloc_report.strip() if reloc_report else 'readelf unavailable; not separately checked',
        'scope':'structural/source-map/byte identity tests; no whole-game execution'}
    (ROOT/'build/project_tests.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))
if __name__=='__main__':main()
