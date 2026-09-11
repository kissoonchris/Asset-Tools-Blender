# JSRF Native v69 checkpoint

## v68 Windows evidence

- v67 request correction remains good: WX `0x001403B0` returns `0x19` (25 sectors), and the CRI wrapper propagates it.
- The host completes the full 51,200-byte title ADX read.
- v68 `[WXSTAT]` proves the status getter is not inventing the stall: `0x001405B0` repeatedly returns state `2` from the WX object itself.
- The first WX object remains state 2 with sector position 0, request count 25, transfer length 51,200, and observed +0x148/+0x14C bookkeeping fields both zero after the read.
- The producer ring stays at zero filled bytes until the retail ADXT watchdog reaches its real 1201-count error path.
- The v68 run did not reproduce the prior stack-guard exit; it continued into the retail fatal/error path and was then closed by the user.

## v69 change

v69 is diagnostic-only and adds no forced guest state.

- Adds `write_jsrf_completion_codegen_report()` before generated-source instrumentation.
- Captures exact retail bytes plus untouched generated functions for:
  - XECI/WX internals `0x00140000..0x001403B0`
  - XAPI ReadFile `0x00145F80..0x00146080`
- Adds sparse read-only `[XECISRV]` entry/exit probes to generated `sub_00140130`.
- The probe records the first WX object's state, buffer, sector size, total sectors, current position, request count, transfer offset/length, and +0x148/+0x14C read/update bookkeeping fields.
- `JSRF_COMPLETION_CODEGEN_LOG.txt` is appended automatically to the one-click aggregate log.

## Verification

- TDD red phase: 4 expected failures.
- Focused v69 tests: 4 passed.
- Full Python suite: 262 passed.
- Python py_compile: passed.
- `recomp_manual.c` C11 syntax: passed.
- Native Release build: passed.
- CTest: 20/20 passed.
- Re-extracted final ZIP repeated the same verification: 262 Python tests and 20/20 CTest.
- Final package: 206 files, `START JSRF TEST.bat` present, no retail `default.xbe` or build/cache/generated-log payload.
- v68->v69 patch reconstructs the final clean source tree byte-for-byte: 206/206 files.

## Artifacts

- `JSRF-Native-v69-CLEAN-PROJECT.zip`
  - SHA-256: `735ac519eb63d5c577a35d15eeeda5848b135ea3dd9ace6935f6115c76b504fa`
- `JSRF-v68-CLEAN-to-v69.patch`
  - SHA-256: `bceda5fff38c07b35c203846924cdc502c7f7f98bfba493c99bb48119c5560f7`
- `JSRF-v68-CLEAN-to-v69.patch.gz`
  - SHA-256: `34d9fea8519aa9ed29cc3dedd3edc7f5ea35c337d0b16e9182c33e18fb77a3d3`
- `JSRF-v68-CLEAN-to-v69.patch.gz.b64`
  - SHA-256: `4f559d2d32967ab1392a0438e78c13b0567fc6a84835ebc892586bba6eeb08b6`
