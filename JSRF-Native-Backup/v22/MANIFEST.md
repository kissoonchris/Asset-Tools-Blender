# JSRF Native Backup v22

Date: 2026-09-07
Base: v21 (`jsrf-native-backup-v21-stage`)

## v21 Windows root cause

v21 did create xboxrecomp's upstream MCPX APU object, but the two xboxrecomp
runtime switches were not visible to the runtime:

- the log contains `[APU] MCPX APU initialized (standalone)`;
- it does **not** contain the memory-layout lines for the APU MMIO trap or the
  AC'97 codec-ready bit;
- it contains no `[APUMMIO]` trace lines;
- retail DirectSound consequently still returns `0x88780078` and startup later
  follows the normal XAPI reboot/exit path.

The source ordering was already correct. The configuration transport was not.
v21 used `SetEnvironmentVariableA()` while pinned xboxrecomp reads both switches
with C `getenv()`. On MSVC/UCRT, `getenv()` reads the CRT `_environ` table;
`_putenv_s()` is the matching setter for that table.

## v22 correction

v22 replaces only the two late Win32 environment writes with CRT environment
writes before `xbox_MemoryLayoutInit()`:

- `_putenv_s("RECOMP_AC97_READY", "1")`
- `_putenv_s("RECOMP_APU_TRACE", "1")`

Boot now fails explicitly if either setter fails.

This does not fabricate DirectSound success and does not replace guest audio or
gameplay code. It only makes the already-selected upstream xboxrecomp AC'97/APU
hardware path receive its configuration through the same CRT environment table
that xboxrecomp reads.

The v17 lock-XADD EFLAGS fix, v18/v19 NV2A fence mirror, v21 MCPX APU ownership,
and APU MMIO VEH routing remain present.

## TDD

A new regression test first failed against v21 because `_putenv_s` was absent
and `SetEnvironmentVariableA` was still present. After the correction, the
focused environment tests pass.

## Verification

Fresh verification from an extraction of the final v22 ZIP:

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — PASS
- `python -m pytest -q tests` — 37 passed
- root CMake configure/build — PASS
- root CTest — 20/20 passed
- v21→v22 patch `git diff --check` — PASS
- v21→v22 patch applies cleanly to a fresh v21 source tree — PASS
- Easy-test ZIP integrity — PASS
- Source ZIP integrity — PASS
- final ZIP contract confirms:
  - `START JSRF TEST.bat` at archive root
  - v17 lock-XADD patch still invoked
  - JSRF NV2A fence mirror still registered
  - `_putenv_s` used for both xboxrecomp `getenv()` switches
  - stale `SetEnvironmentVariableA("RECOMP_AC97_READY", ...)` absent
  - MCPX APU initialization present
  - APU MMIO VEH delegation present
  - no pytest/pycache/build artifacts packaged

This does not claim the Windows title now reaches the persistent main loop. The
next Windows run is required to validate that the AC'97/APU path is now truly
active and to expose the next real hardware/guest blocker if one remains.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v22.zip`
  SHA-256: `937b06678bbebe3954a212cd8fa123be8117e4c78caceb935d15f514f8946a66`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX21.zip`
  SHA-256: `937b06678bbebe3954a212cd8fa123be8117e4c78caceb935d15f514f8946a66`
- `JSRF-V21-to-V22.patch`
  SHA-256: `5a99d7ec9c7f2758a28292bbbd013d0463ab462198681e1b90c9b075c386f4eb`

Full ZIP archives are saved in the ChatGPT `/JSRF/` Files Library. GitHub stores
this manifest and the complete v21→v22 text patch as redundant source backup.
