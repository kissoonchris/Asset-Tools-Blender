# JSRF Native v74 Checkpoint

## Status
v74 is a diagnostic checkpoint following the complete v73 Windows run.

The previous audio and cleanup fixes are confirmed to move retail execution forward. The main ADXT ring reaches `filled=51164` and `read_pos=51200`, so the old zero-filled/zero-position title ADX stall is no longer the active blocker.

The next deterministic failure is retail `0x00011070` (`recursiveExecDefault`) traversing a GameObj edge into contiguous-memory address `0x81688000`. That object's first dword is `0xFFFF0000`, and the subsequent vtable-slot read faults at `0xFFFF0004`. v74 does not skip or repair the object. It adds read-only `[GOBJ]` probes at entry, before vtable dispatch, after the `+0x28` child load, and before the `+0x30` sibling transition to capture the exact source object/link on the next Windows run.

The v73 one-click promoter also independently accepted retail callback entry `0x0013B750`; v74 makes that runtime-proven target a permanent seed so the next test reaches the GameObj boundary directly.

## Verification
- focused v74 regression: 4/4 passed after red phase
- full Python suite: 282/282 passed
- `scripts/jsrf_xboxrecomp.py` py_compile: passed
- `recomp/src/recomp_manual.c` C11 syntax: passed
- source-tree Release build: passed
- source-tree CTest: 20/20 passed
- exact final ZIP re-extracted: 282/282 Python + syntax passed
- exact final ZIP Release build: passed
- exact final ZIP CTest: 20/20 passed
- clean distributable: 217 files
- root `START JSRF TEST.bat`: present
- retail `default.xbe`: absent
- build/Python caches: absent
- fresh v73 + patch -> v74: 217 files, missing 0, extra 0, changed 0

## Artifacts
- `JSRF-Native-v74-CLEAN-PROJECT.zip`
  - SHA256 `cf6cbcd64d0781f36f9dbd02f3c04f72710decd321f97d004ee9eed8b225d1aa`
- `JSRF-v73-CLEAN-to-v74.patch`
  - SHA256 `be7c42712880e701e22a339972189ca9b7832708752d5f67292d885d22d53dd4`
- compressed patch gzip SHA256 `24babc8db17415f6814e54c5be416093d7190adfe0fe043ef726df431f16c8fd`
- compressed patch base64 SHA256 `e402a5a66d1d5c6fc0f83880a8aa728566d30567cb31321a2a4ca541cf515f9a`
