# JSRF Native v66 checkpoint

## Windows evidence from v65

- WX request `0x001403B0` phase-1 returns `EAX=0` repeatedly.
- CRI wrapper `0x0013DD70` phase-1 propagates the same zero.
- WX object `0x00273780` has accepted/request count 25 and a 51,200-byte async read.
- Host read completes 51,200/51,200 bytes successfully and the destination contains the ADX header.
- Source result remains zero and producer-ring reservation is cancelled.
- The user-closed framebuffer exits with code 5; this is not a crash.

## v66 change

v66 does not force the expected request result. It captures the untouched xboxrecomp-generated functions and exact retail XBE bytes for `0x001403B0..0x001405B0` into `JSRF_WXREQ_CODEGEN_LOG.txt`, which the one-click runner appends automatically to `JSRF_TEST_LOG.txt`. The phase-1 probe also recovers the WX object from preserved stack argument 1 so object state is visible at the generated RET.

## Verification

- TDD red phase: 4 expected failures.
- Focused v64/v65/v66 tests: 12 passed.
- Full Python suite: 249 passed.
- Python py_compile: passed.
- `recomp_manual.c` C11 syntax: passed.
- Native Release build: passed.
- CTest: 20/20 passed.
- Re-extracted final ZIP repeated the same verification: 249 Python tests and 20/20 CTest.
- Final package: 198 files, `START JSRF TEST.bat` present, no retail `default.xbe` or build/cache payload.
- v65->v66 patch reconstructs the final clean source tree byte-for-byte: 198/198 files.

## Artifacts

- `JSRF-Native-v66-CLEAN-PROJECT.zip`
  - SHA-256: `f7f163b61ee869ae3e6c827b9f1e1c086a273b987f91e24cd2190334ab33ecc0`
- `JSRF-v65-CLEAN-to-v66.patch`
  - SHA-256: `67d8cebd7cda1a3218597bcc3c007005e4c1b799183b946157fbd223c8394226`
