# JSRF Native Checkpoint 08 Hotfix 13 / Easy Test v14

## Root cause fixed

The v13 Windows run showed `0x00160080` restoring its callee-saved registers but returning with guest ESP unchanged. The next event was the bad indirect target `0x00000002` at return address `0x0015FC99`.

Exact retail JSRF bytes prove the third and final return of `0x00160080` begins at `0x00160105` and is the three-byte instruction `C2 0C 00` (`ret 0x0C`), covering `0x00160105..0x00160107`. v13 incorrectly pinned the exclusive function end at `0x00160106`, chopping this return instruction after its first byte. The correct exclusive end is `0x00160108`; byte `0x00160108` is alignment NOP `0x90`.

That lost return leaves guest ESP exactly 16 bytes too low (4-byte synthetic return address plus 12 bytes of callee-cleaned arguments), which explains why caller `0x0015FC40` subsequently misreads saved stack data as a COM object and reaches target `0x00000002`.

v14 changes the pinned interval to `0x00160080..0x00160108` and strengthens generated-code verification so `sub_00160080` must retain all three retail `ret 0x0C` exits. No JSRF behavior is replaced.

## Verification

- `tests/test_jsrf_xboxrecomp.py`: 17/17 passed
- `tests/test_easy_test_powershell.py`: 4/4 passed
- full Python suite: 21/21 passed
- CTest: 20/20 passed
- `python -m py_compile scripts/jsrf_xboxrecomp.py`: passed
- exact retail-byte check: `0x00160105 = C2 0C 00`, `0x00160108 = 90`: passed
- ZIP integrity / required-content audit: passed

## Artifacts

- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v14.zip`
  - SHA-256: `92933fd44c344739f6759e51923364468e40cac8a39bec93f408a2be8821f3c7`
  - Library ID: `libfile_9b1040a8f3f48191a5e63dac57b5e70c`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX13.zip`
  - SHA-256: `92933fd44c344739f6759e51923364468e40cac8a39bec93f408a2be8821f3c7`
  - Library ID: `libfile_5cdd44aff8d08191975cc2c81997754a`
- `/JSRF/JSRF-V13-to-V14.patch`
  - SHA-256: `f7e9c2a89f744b8e31d77abacb8d2a43f677f570f5a4b7ee3c975f0914cc9c53`
  - Library ID: `libfile_9c8f01fbe8b48191a1069a9c9ba09459`
