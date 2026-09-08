# JSRF Native v30 manifest

## Purpose

v30 is a diagnostic checkpoint for the remaining worker TLS/TIB corruption seen in the v29 Windows run. It does **not** add padding above worker stacks and does not claim the underlying stack drift is fixed.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v29 evidence carried forward

The crashing worker began with guest stack top `0x011EAFF0`, TIB `0x011EB000`, and valid `fs:[4] = 0x011EB054`. At the later failure the TIB base was unchanged but `fs:[4]` was zero, while TIB header addresses contained values shaped like retail return addresses. The failure in `sub_00146429` is therefore downstream evidence; the useful next boundary is the first write into the region above the worker stack.

## v30 change

`scripts/jsrf_xboxrecomp.py` now installs an idempotent diagnostic stack guard into the pinned xboxrecomp checkout before code generation:

- translated `PUSH32` writes are checked before the write;
- translated ESP/EBP-relative memory stores are checked before the write;
- the check is active only for spawned guest workers and the region immediately above their recorded stack top;
- the first crossing prints `[STACKGUARD]`, guest stack/TIB state, and the generated C source file/line, then exits with diagnostic code 4 before corruption occurs.

The beginner PowerShell runner recognizes code 4, preserves the runtime/source logs, and does not run the unrelated mirror-protection pass afterward.

No retail gameplay, D3D, audio, allocator, or TLS behavior is replaced by this checkpoint.

## Verification

- v30 regression test was written first and failed against v29 because the guard did not exist.
- focused v30 regression suite: **4 passed** after implementation.
- full Python suite: **80 passed**.
- Python syntax compilation: **passed**.
- native CMake/CTest suite: **20/20 passed**.
- Windows JSRF runtime verification is still required to identify the first offending lifted instruction.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v30.zip` — SHA-256 `617ab01e6f161e64ef4c5f1d45ca85eb48ad7fe43ed1567c0b7e636b1edbbd6b`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX29.zip` — SHA-256 `617ab01e6f161e64ef4c5f1d45ca85eb48ad7fe43ed1567c0b7e636b1edbbd6b`
- `JSRF-V29-to-V30.patch` — SHA-256 `3a3336e940313980e4b90b1b78ba2aa35812ca15071b9108f2f9d5e0ca72997e`
- `JSRF-V30-MANIFEST.md` — SHA-256 `76955a34810d6616fd54c44db99d8356c50325b9d349b72cd9c17093e0a5002e`

The full ZIP archives and patch are stored in the ChatGPT `/JSRF/` Files Library.

## Next Windows evidence

Extract the v30 easy-test ZIP into a new folder and double-click `START JSRF TEST.bat`. If the guard fires, send the generated `JSRF_TEST_LOG.txt`; the source context should now point at the first stack/TIB crossing rather than the later TLS crash.
