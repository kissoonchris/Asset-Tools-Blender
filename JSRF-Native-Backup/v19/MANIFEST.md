# JSRF Native Backup v19

Date: 2026-09-07

## Why v19 exists

The v18 Windows package accidentally regressed to the pre-v17 source baseline.
It contained the new D3D/NV2A fence work but omitted the already-proven v17
`lock xadd` EFLAGS correction. The user's v18 run therefore reproduced the old
retail heap free-list corruption at `0x00149B28` before reaching the D3D wait.

v19 is rebuilt from the known-good v17 package, then applies the v18 D3D fence
changes on top. It therefore preserves both fixes in one source tree.

## v19 source contract

- exact retail JSRF XBE SHA-256:
  `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`
- retail entry point: `0x00148023`
- pinned xboxrecomp: `4d337526dc4ab892483aad0246aee9d1c33d1b81`
- v17 root fix retained:
  `patch_xboxrecomp_lock_xadd_flags()` runs before generation
- v18 D3D work retained:
  JSRF NV2A fence mirror at device global `0x0019DCE0`, PUT `+0x30`,
  completion pointer `+0x34`
- read-only D3D wait probe retained
- retail allocator behavior is not bypassed
- no gameplay replacement logic was added

## Regression coverage

`tests/test_v18_preserves_v17_root_fix.py` was written first and run against the
bad v18 tree. It failed because `patch_xboxrecomp_lock_xadd_flags` was absent.
The same test passes in v19 after composing v18 on top of v17.

## Fresh verification from the final v19 ZIP

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — PASS
- `python -m pytest -q tests` — 31 passed
- CMake configure/build — PASS
- `ctest --test-dir build_verify --output-on-failure` — 20/20 passed
- ZIP integrity — PASS for both archives
- final ZIP contract check confirms:
  - `START JSRF TEST.bat` exists at archive root
  - lock-XADD patch exists and is invoked
  - D3D wait instrumentation exists
  - JSRF NV2A fence registration exists

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v19.zip`
  SHA-256: `4136ddaab5729d23a76e0d98a327a40c4ae4a2fba7968c982f2f71374f1842b4`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX18.zip`
  SHA-256: `4136ddaab5729d23a76e0d98a327a40c4ae4a2fba7968c982f2f71374f1842b4`
- `JSRF-V18-to-V19.patch`
  SHA-256: `d5ecdcc033b5d7389912215fc2aa2c6b4cc584f5d6a77390372640229d64a994`

GitHub stores this manifest plus the v18→v19 text patch as redundant backup.
The tested full ZIP archives are also saved in the `/JSRF/` Files Library.
