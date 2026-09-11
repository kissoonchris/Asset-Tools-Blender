# JSRF Native v68 checkpoint

## v67 Windows evidence

- v67 fixed the previous WX request return bug: `0x001403B0` now returns `0x19` (25 sectors), and CRI wrapper `0x0013DD70` propagates the same positive result.
- The host completes the full 51,200-byte title ADX read successfully.
- The producer ring remains reserved with `free=0` but never gains filled bytes; ADXT later reaches its legitimate watchdog timeout. The primary blocker has therefore moved to WX asynchronous status/completion.
- After the timeout cleanup begins, generated `0x0013B610` corrupts ESP/callee-saved registers and the stack guard exits on the following caller iteration. This is secondary to the audio-completion stall.

## v68 change

v68 is diagnostic-only and does not alter guest behavior. It captures untouched generated C plus exact retail bytes for:

- WX status/completion: `0x001405B0..0x001406D0`
- cleanup callee: `0x0013B610..0x0013B810`
- cleanup caller: `0x0013BDF0..0x0013BE70`

It also adds sparse read-only `[WXSTAT]` entry/exit probes to `0x001405B0`, recording the live return register, original object argument, state, request/progress, and async bookkeeping fields. The one-click test automatically appends this evidence to `JSRF_TEST_LOG.txt`.

## Verification

- TDD red phase: 4 expected failures.
- Focused v68 tests: 4 passed.
- Full Python suite: 258 passed.
- Python py_compile: passed.
- `recomp_manual.c` C11 syntax: passed.
- Native Release build: passed.
- CTest: 20/20 passed.
- Re-extracted final ZIP repeated the same verification: 258 Python tests and 20/20 CTest.
- Final package: 203 files, `START JSRF TEST.bat` present, no retail `default.xbe` or build/cache payload.
- v67->v68 patch reconstructs the final clean source tree byte-for-byte: 203/203 files.

## Artifacts

- `JSRF-Native-v68-CLEAN-PROJECT.zip`
  - SHA-256: `eb7dab64e215170fcf0e63a69d2adbfc02d01b9e89b4df3ba5552cd91a38bb46`
- `JSRF-v67-CLEAN-to-v68.patch`
  - SHA-256: `c714427c06b2625f3fe7869ccf325c9c77a156c2a85e1dd328db2859903b817f`
- `JSRF-v67-CLEAN-to-v68.patch.gz`
  - SHA-256: `d00425562b470deaf48f68b22f27d58bebeea03d9597718d5179bd2ce05ab7a9`
- `JSRF-v67-CLEAN-to-v68.patch.gz.b64`
  - SHA-256: `2e829191a24fda6bcd108f5c8ab2eaf34c9f8ee7c96e16ad6cff88b83fbec0ca`
