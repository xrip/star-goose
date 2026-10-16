#!/usr/bin/env python3
"""Compile and exercise the arithmetic helpers the port actually calls.
Uses a host C11 compiler via CC (default cc). No third-party Python dependency.
These are data/contract tests, NOT execution of original x86 code.
"""
from __future__ import annotations
import ctypes as C
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
    if os.name=='nt':
        raise SystemExit('This test launcher is validated for Linux/WSL; use it there. C sources are platform-neutral.')
    cc=shutil.which(os.environ.get('CC','cc'))
    if not cc:raise SystemExit('C compiler not found; set CC or install cc/clang/gcc.')
    with tempfile.TemporaryDirectory(prefix='goose-c-models-') as tmp:
        lib=Path(tmp)/'goose_model.so'
        subprocess.run([cc,'-std=c11','-O2','-Wall','-Wextra','-Werror','-pedantic','-shared','-fPIC',
            str(ROOT/'port/goose_model.c'),'-o',str(lib)],check=True)
        api=C.CDLL(str(lib))
        api.goose_surface_dx.argtypes=[C.c_int16];api.goose_surface_dx.restype=C.c_int16
        api.goose_sar16.argtypes=[C.c_uint16,C.c_uint];api.goose_sar16.restype=C.c_uint16
        api.goose_tunnel_angle_force.argtypes=[C.c_uint8];api.goose_tunnel_angle_force.restype=C.c_int16
        counts={}
        for bits in range(65536):
            signed=bits if bits<32768 else bits-65536
            for shift in (0,1,2,7,8,15,16,17):
                assert api.goose_sar16(bits,shift)==((signed//(1<<shift))&65535)
            v=(bits+3)&65535;v=v if v<32768 else v-65536
            assert api.goose_surface_dx(signed)==v//4
        counts['sar_cases']=65536*8;counts['surface_dx_cases']=65536
        for a in range(256):
            value=(a-5)&255
            expected=0 if value>=128 else -(min(value,56)//2)-3
            assert api.goose_tunnel_angle_force(a)==expected
        counts['tunnel_force_cases']=256
    report={'status':'PASS','scope':'C arithmetic helpers; NOT original x86 execution or whole-game equivalence',
        'compiler':subprocess.check_output([cc,'--version'],text=True).splitlines()[0],'counts':counts}
    out=ROOT/'build/c_model_tests.json';out.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__':main()
