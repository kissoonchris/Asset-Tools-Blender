# JSRF Native v73 Checkpoint

## Status
v73 fixes a generic xboxrecomp indirect-call stack recovery bug exposed by the first true v72 Windows run.

The v72 deferred file-I/O APC change is confirmed to work: the 51,200-byte title ADX read returns from the XAPI async helper and WX worker with `wants_update=1`, then the queued APC drains afterward. This moved execution beyond the previous completion-ordering blocker.

The next deterministic failure was cleanup routine `0x0013B610`. Its invalid vtable calls use generated `RECOMP_ICALL_SAFE`, whose failure path restores ESP to the pre-argument value. Retail code at those call sites then explicitly performs caller cleanup (`add esp,0x10` and `add esp,0x0C`). On failed indirect calls this double-cleans 16 + 12 = 28 bytes, exactly matching the observed ABI excess (`0x011EAF88` expected normal return `0x011EAF8C`, observed `0x011EAFA8`). Repeated cleanup eventually pushes beyond the worker-stack top.

v73 adds a conservative generic translator correction: generated indirect calls that are immediately followed by explicit caller cleanup equal to their argument bytes use ordinary `RECOMP_ICALL` failure semantics instead of stdcall-style `RECOMP_ICALL_SAFE`. Sites without matching explicit caller cleanup remain SAFE. No JSRF addresses or object-state values are hardcoded.

## Verification
- Python regressions: 278 passed
- `scripts/jsrf_xboxrecomp.py` py_compile: passed
- `recomp/src/recomp_manual.c` C11 syntax: passed
- Native Release build: passed
- CTest: 20/20 passed
- Clean package: 215 files
- Root `START JSRF TEST.bat`: present
- Retail `default.xbe`: absent
- Clean package build/cache/Python cache payload: absent
- Fresh extraction repeated Python/syntax/Release/CTest gates successfully
- v72 -> v73 patch reconstruction: 215/215 files, missing 0, extra 0, changed 0 (byte-for-byte exact)

## Artifacts
- `JSRF-Native-v73-CLEAN-PROJECT.zip`
  - size: 391439 bytes
  - SHA256: `e94bcb2905faf95da3aee655fca4e172d7c445e8c06fd1a24e5475f2b146b3d7`
- `JSRF-v72-CLEAN-to-v73.patch`
  - size: 27283 bytes
  - SHA256: `6343984658252dbbb9ffff86f1345d153e121c3a7771c454730b1d1be9f33694`

## Persistent Library
- `/JSRF/JSRF-Native-v73-CLEAN-PROJECT.zip`
  - library id: `libfile_ea2a495c4e348191b0f71fd7487a125b`
- `/JSRF/JSRF-v72-CLEAN-to-v73.patch`
  - library id: `libfile_af18adb4ef5c8191b4731af53d7097fa`

## GitHub lineage
v72 did not receive a GitHub checkpoint branch. v73 therefore branches from the latest existing checkpoint, `jsrf-native-backup-v71-stage`, while this checkpoint records the complete v72 -> v73 patch and v73 evidence.
