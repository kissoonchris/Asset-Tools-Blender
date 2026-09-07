# JSRF Native v26 manifest

## What v25 proved

v25 removed the previous missing `0x00178F40` call and continued much farther through real DirectSound setup.

The v25 run was then terminated by the old xboxrecomp watchdog at exactly 60 seconds even though the title was still making forward progress: immediately before the watchdog fired, JSRF completed a long page-by-page `MmGetPhysicalAddress` walk through the DirectSound allocation and wrote APU register `0x020D4`.

The same run exposed six unique unresolved indirect-call targets:

- `0x0013B180`
- `0x0013B1C0`
- `0x0013B230`
- `0x0013B2A0`
- `0x001BCAA3`
- `0x001BD274`

Retail disassembly independently proves these are real callback/worker entries. Three adjacent DirectSound callbacks (`0x0013B0A0`, `0x0013B0E0`, `0x0013B110`) are registered by the same retail setup function and are also boundary-proven.

## v26 changes

v26 is deliberately broader than the previous one-blocker releases so one Windows test can cover more ground:

1. **Batches all nine currently proven callback/worker entries** and pins their measured retail boundaries.
2. Adds a conservative `promote-unresolved` pass. A runtime-observed indirect target is automatically seeded only if it is inside a raw-backed JSRF code section **and** the retail XBE independently contains the address as a literal callback/function pointer.
3. `START JSRF TEST.bat` can automatically preserve diagnostics, promote safe new callback targets, rebuild, and continue up to **three passes in one user-initiated test**.
4. Changes the watchdog from a fixed wall-clock cutoff to a **no-progress timeout** when JSRF registers its existing indirect-call counter. The 60-second setting now means 60 seconds without indirect-call progress, so long live initialization is not killed merely for taking more than one minute.
5. Moves the xboxrecomp checkout to `%LOCALAPPDATA%\JSRF-Native-Cache`, avoids Git fetch/checkout when the exact pinned revision is already cached, and skips `pip install capstone` when Capstone already imports.
6. Remembers the selected extracted JSRF folder in `%LOCALAPPDATA%\JSRF-Native-Cache\game-path.txt`.
7. Preserves all prior JSRF correctness fixes, including lock-XADD flags, repeated CMPS/SCAS flags, DirectSound boundaries, APU/AC97 activation, the NV2A fence mirror, and the `0x00178F40` callable boundary.

No game return value, HRESULT, DirectSound result, gameplay state, or arbitrary loop exit is forced.

## Verification

- Python: `python -m py_compile scripts/jsrf_xboxrecomp.py` passed
- Python tests: **50 passed**
- Root CMake configure/build: passed
- CTest: **20/20 passed**
- v25 -> v26 patch: `git diff --check` passed
- v25 -> v26 patch: applies cleanly to a fresh v25 tree
- applied-patch tree: byte-for-byte content match with v26 tree (excluding build/cache metadata)
- real v25 UTF-16 runtime-log auto-promotion check: promoted exactly the six unresolved targets above
- Easy ZIP integrity: passed
- Source ZIP integrity: passed
- `START JSRF TEST.bat`: present at ZIP archive root
- package cache/build artifacts: none
- Easy/source archives: byte-identical

## Files

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v26.zip`
  - SHA-256 `13be27da867af63fd387923ef1dbd97de21d41bb18af35eebfa1ec9f7eec2643`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX25.zip`
  - SHA-256 `13be27da867af63fd387923ef1dbd97de21d41bb18af35eebfa1ec9f7eec2643`
- `JSRF-V25-to-V26.patch`
  - SHA-256 `6c6847e1458acb3276be297aac8874a7a8eeb4b0eb6c62c3489ee6f5a2443c25`
