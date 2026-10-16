#!/usr/bin/env python3
"""Compile and exercise the small C reference models, not the entire game.
Uses a host C11 compiler via CC (default cc). No third-party Python dependency.
These are data/contract tests, NOT execution of original x86 code.
"""
from __future__ import annotations
import ctypes as C
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
ROOT=Path(__file__).resolve().parents[1]
class Entity(C.Structure):
    _fields_=[('flags',C.c_uint8),('param',C.c_uint8),('layer',C.c_uint8),('z',C.c_int8),
        ('frame',C.c_uint8),('x',C.c_uint16),('y',C.c_uint16),('width',C.c_uint8),('height',C.c_uint8),
        ('anchor_x',C.c_int8),('anchor_y',C.c_int8),('bank_offset',C.c_uint16),('sprite_offset',C.c_uint16),
        ('transparent',C.c_uint8),('handler_id',C.c_uint8),('state',C.c_uint8*4)]
def main():
    if os.name=='nt':
        raise SystemExit('This test launcher is validated for Linux/WSL; use it there. C sources are platform-neutral.')
    cc=shutil.which(os.environ.get('CC','cc'))
    if not cc:raise SystemExit('C compiler not found; set CC or install cc/clang/gcc.')
    exe=ROOT/'build/GOOSE.EXE'
    if not exe.is_file():raise SystemExit('Run python3 build.py first.')
    image=exe.read_bytes();ds=image[0x3c50:]
    spans=json.loads((ROOT/'analysis/data_spans.json').read_text())
    counts={}
    with tempfile.TemporaryDirectory(prefix='goose-c-models-') as tmp:
        lib=Path(tmp)/'goose_model.so'
        subprocess.run([cc,'-std=c11','-O2','-Wall','-Wextra','-Werror','-pedantic','-shared','-fPIC',
            str(ROOT/'port/goose_model.c'),'-o',str(lib)],check=True)
        api=C.CDLL(str(lib));ptr=C.POINTER(Entity)
        api.goose_entity_read.argtypes=[ptr,C.c_void_p,C.c_size_t];api.goose_entity_read.restype=C.c_bool
        api.goose_entity_write.argtypes=[C.c_void_p,C.c_size_t,ptr];api.goose_entity_write.restype=C.c_bool
        api.goose_entity_allocate.argtypes=[ptr,ptr];api.goose_entity_allocate.restype=C.c_int
        api.goose_entity_clear.argtypes=[ptr];api.goose_entity_clear.restype=None
        api.goose_apply_frame.argtypes=[ptr,C.c_void_p,C.c_size_t];api.goose_apply_frame.restype=C.c_bool
        api.goose_surface_dx.argtypes=[C.c_int16];api.goose_surface_dx.restype=C.c_int16
        api.goose_sar16.argtypes=[C.c_uint16,C.c_uint];api.goose_sar16.restype=C.c_uint16
        api.goose_tunnel_angle_force.argtypes=[C.c_uint8];api.goose_tunnel_angle_force.restype=C.c_int16
        # Round-trip every recovered template and 4096 deterministic arbitrary records.
        records=[ds[s['ds_start']:s['ds_end']] for s in spans if s['description'].startswith('Entity template:')]
        records += [hashlib.sha256(str(i).encode()).digest()[:23] for i in range(4096)]
        for raw in records:
            ent=Entity();dst=C.create_string_buffer(23)
            assert api.goose_entity_read(C.byref(ent),raw,23)
            assert api.goose_entity_write(dst,23,C.byref(ent))
            assert dst.raw==raw
        counts['entity_roundtrips']=len(records)
        # Every frame in each extracted FrameSet, using independent expected field offsets.
        nframes=0
        for s in spans:
            if not s['description'].startswith('FrameSet '):continue
            desc=ds[s['ds_start']:s['ds_end']];n=desc[0]
            for i in range(n):
                ent=Entity();ent.frame=i
                assert api.goose_apply_frame(C.byref(ent),desc,len(desc))
                assert (ent.width,ent.height,ent.anchor_x&255,ent.anchor_y&255)==tuple(desc[1+k*n+i] for k in range(4))
                assert (ent.bank_offset,ent.sprite_offset)==struct.unpack_from('<HH',desc,1+4*n+4*i)
                nframes+=1
        counts['frame_applications']=nframes
        pool=(Entity*48)();source=Entity();source.flags=3;source.x=77;source.state[2]=99
        for i in range(48):pool[i].flags=1;pool[i].x=i+100
        assert api.goose_entity_allocate(pool,C.byref(source))==-1
        pool[17].flags=2
        assert api.goose_entity_allocate(pool,C.byref(source))==17
        assert pool[17].x==77 and pool[17].state[2]==99
        before=[pool[i].x for i in range(48)]
        api.goose_entity_clear(pool)
        assert all(pool[i].flags==0 and pool[i].x==before[i] for i in range(48))
        counts['pool_contract_scenarios']=3
        # Full 16-bit input space: expected arithmetic shift uses Python floor division.
        for bits in range(65536):
            signed=bits if bits<32768 else bits-65536
            for shift in (0,1,2,7,8,15,16,17):
                assert api.goose_sar16(bits,shift)==((signed//(1<<shift))&65535)
            v=(bits+3)&65535;v=v if v<32768 else v-65536
            assert api.goose_surface_dx(signed)==v//4
        counts['sar_cases']=65536*8;counts['surface_dx_cases']=65536
        for a in range(256):
            # Reproduce signed flag from the 8-bit SUB, including unusual inputs.
            value=(a-5)&255
            expected=0 if value>=128 else -(min(value,56)//2)-3
            assert api.goose_tunnel_angle_force(a)==expected
        counts['tunnel_force_cases']=256
        ent=Entity();short=b'\x00'
        assert not api.goose_entity_read(C.byref(ent),short,1)
        assert not api.goose_apply_frame(C.byref(ent),short,1)
        counts['invalid_buffer_checks']=2
    report={'status':'PASS','scope':'C helper/data contract tests; NOT original x86 execution or whole-game equivalence',
        'compiler':subprocess.check_output([cc,'--version'],text=True).splitlines()[0],'counts':counts}
    out=ROOT/'build/c_model_tests.json';out.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__':main()
