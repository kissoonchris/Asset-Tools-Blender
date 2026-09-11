# JSRF Native v71 checkpoint

## v70 Windows evidence

- v67's WX request correction remains good: retail `0x001403B0` and the CRI wrapper return `0x19` (25 sectors).
- The host completes the full 51,200-byte `title.adx` read successfully.
- The first WX object nevertheless remains state 2 at sector position 0 and the source ring remains unfilled until the retail ADXT watchdog reaches its legitimate 1201-count failure path.
- v70's generic `[READCALLER]` probe emitted its 128-record budget on unrelated startup file traffic before the title ADX reached the relevant read caller, so it did not capture the title-specific completion transition.
- Untouched generated code identifies the exact async chain: WX worker `0x001407E0` calls async helper `0x00146375` with completion callback `0x001401B0`.
- Retail callback `0x001401B0` sets the WX state byte to 3 on success and clears `+0x14C`.
- The framebuffer was closed by the user (exit code 5); no stack-guard crash was observed.

## v71 change

v71 is diagnostic-only; no guest completion state is forced.

- Adds `write_jsrf_wx_async_completion_report()` before generated-source instrumentation.
- Captures exact retail bytes and untouched generated C for `0x001401B0`, `0x001407E0`, and `0x00146375` in `JSRF_WX_ASYNC_CODEGEN_LOG.txt`.
- Adds read-only `[WXASYNC]` entry/return probes only to those three exact functions.
- Probes emit only while the known title WX object is active (`state==2`, read/update flags set, 51,200-byte transfer active, or callback context resolves to the object).
- Uses independent per-site/per-phase counters, so unrelated startup I/O cannot consume the title evidence budget.
- Records WX state/position, embedded status/OVERLAPPED fields, transfer fields, read/update flags, six guest stack arguments, and callback-object resolution.
- The one-click test appends `WX ASYNC COMPLETION CODEGEN EVIDENCE` automatically.

## Verification

- TDD red phase: 4 expected focused failures before implementation.
- Focused v71 tests: 4/4 passed.
- Full Python suite: 270 passed.
- Python py_compile: passed.
- `recomp_manual.c` C11 syntax: passed.
- Native Release build: passed.
- CTest: 20/20 passed.
- Re-extracted final ZIP repeated: 270 Python tests, syntax checks, Release build, 20/20 CTest.
- Final package: 210 files, root `START JSRF TEST.bat`, no retail `default.xbe`, build output, generated evidence logs, or Python caches.
- v70->v71 patch reconstructs the final clean tree byte-for-byte: 210/210 files.

## Artifacts

- `JSRF-Native-v71-CLEAN-PROJECT.zip`
  - SHA-256: `4dfc21e42101496a975c8dd79b2d69a2655df9ac4316b46a51d1e6327b0ea7c2`
- `JSRF-v70-CLEAN-to-v71.patch`
  - SHA-256: `eb1829268b184fc519709d43c9625125d4fcf89c32586f418ead394de955b8bd`
- `JSRF-v70-CLEAN-to-v71.patch.gz`
  - SHA-256: `8a398c9d85a6c9115a923675cd00a443e1508587abdb6a5ba5be587e866f0677`
- `JSRF-v70-CLEAN-to-v71.patch.gz.b64`
  - SHA-256: `c4919a5c6cea91654e08d9815fe38631298b7ce45aa3a4c7b68a24ecff18b3fd`
