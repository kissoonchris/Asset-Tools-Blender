# JSRF Native v70 checkpoint

## v69 Windows evidence

- v67's WX request correction remains good: retail `0x001403B0` returns `0x19` (25 sectors), and the CRI wrapper propagates the same positive result.
- The host completes the full 51,200-byte `title.adx` read successfully.
- Before the read the first WX object has `+0x148=1`; after the read that read-request field is cleared to zero, so a worker path did service the request.
- The object nevertheless remains state 2 at sector position 0 with request count 25 and transfer length 51,200, while the source ring remains at zero filled bytes until the retail ADXT watchdog reaches its real 1201-count failure path.
- The v69 `[XECISRV]` probe at `0x00140130` has no runtime hits. Inspection of the untouched generated code corrects the earlier label: `0x00140130` is a timing/wait helper used by a polling path, not the object completion server.
- The framebuffer was closed by the user (exit code 5); no new stack-guard crash was observed.

## v70 change

v70 is diagnostic-only and removes the guessed completion address.

- Adds `write_jsrf_xapi_read_caller_report()` before generated-source instrumentation.
- Scans untouched generated functions and automatically discovers every translated caller of XAPI `ReadFile` at `0x00145F8B`, excluding `ReadFile` itself.
- Saves each discovered caller's exact retail bytes and untouched generated C to `JSRF_READ_CALLER_CODEGEN_LOG.txt`.
- Adds sparse read-only `[READCALLER]` entry/return probes to those exact discovered callers.
- The probe records the first WX object's state and position, embedded IO_STATUS_BLOCK/OVERLAPPED status and Information fields at `+0x12C/+0x130`, transfer offset/length, and `+0x148/+0x14C` read/update fields.
- The one-click test automatically appends the new evidence to `JSRF_TEST_LOG.txt`.
- No stream position, completion flag, WX state, ring fill, file result, title state, or graphics state is forced.

## Verification

- TDD red phase: four expected v70 failures.
- Focused v68/v69/v70 tests: 12 passed after implementation/version-generalization.
- Full Python suite: 266 passed.
- Python py_compile: passed.
- `recomp_manual.c` C11 syntax: passed.
- Native Release build: passed.
- CTest: 20/20 passed.
- Re-extracted final ZIP repeated the verification: 266 Python tests and 20/20 CTest.
- Final package: 208 files, `START JSRF TEST.bat` present, no retail `default.xbe`, generated evidence logs, Python caches, or build/cache payload.
- v69->v70 patch reconstructs the final clean tree byte-for-byte: 208/208 files.

## Artifacts

- `JSRF-Native-v70-CLEAN-PROJECT.zip`
  - SHA-256: `7734e73a8b3ec7b9e660db2f7560d9888c488828e8dd29a7060b019f37d519c4`
- `JSRF-v69-CLEAN-to-v70.patch`
  - SHA-256: `ce1563bf81f3153e6a26ddc555223733258f045c12773cc9a677ef5ef6d3a764`
- `JSRF-v69-CLEAN-to-v70.patch.gz`
  - SHA-256: `ad62e46ae50b7a5f533728c9feddfbea3cdb337201d55b05c0bfd3957f639423`
- `JSRF-v69-CLEAN-to-v70.patch.gz.b64`
  - SHA-256: `cf4a9a90b82391fc1f63be799bd6872f4d85f514988ed9d4723b320f65aae898`
