# JSRF Native v67 checkpoint

## Root cause proven by v66

The WX request routine `0x001403B0` receives a valid 25-sector title ADX request and the host completes the full 51,200-byte read, but the generated function returns zero. The v66 retail/codegen capture shows the failure is not file I/O: xboxrecomp loses EFLAGS provenance at result-producing CFG merges around `0x001404DB` and `0x001404F1`.

Retail reaches the second merge after `and edi,0x800007ff` (or the negative normalization ending in `inc edi`). With the observed zero file offset, ZF=1, so `je 0x00140508` must be taken. Generated C instead emits a branch against fallback `_flags`, initialized to zero, and falls into the error-return path.

## v67 change

- Added `patch_xboxrecomp_result_zf_merge_state()`.
- Applied immediately after the existing v62 CMP/TEST merge correction.
- Preserves a representative result-based flag state only when the merged block immediately consumes ZF with JE/JZ/JNE/JNZ and all incoming setters write the same register at the same width.
- Different destinations, memory destinations, unknown states, and non-ZF consumers remain conservative/unknown.
- No JSRF WX return value, sector count, file result, source/ring/ADXT/title/graphics state is hardcoded.

## Verification

- TDD red phase: 4 expected failures.
- Focused v67 tests: 5 passed.
- Full Python suite: 254 passed.
- Python py_compile: passed.
- `recomp_manual.c` C11 syntax: passed.
- Native Release build: passed.
- CTest: 20/20 passed.
- Re-extracted final ZIP repeated the same verification: 254 Python tests and 20/20 CTest.
- Final package: 200 files, `START JSRF TEST.bat` present, no retail `default.xbe` or build/cache payload.
- v66->v67 patch reconstructs the final clean source tree byte-for-byte: 200/200 files.

## Artifacts

- `JSRF-Native-v67-CLEAN-PROJECT.zip`
  - SHA-256: `3ec593a04ea389c3c765a9f71ae3b702087f173429e2a82b0236a5222ce18f7b`
- `JSRF-v66-CLEAN-to-v67.patch`
  - SHA-256: `7aa9ca0c3649a3e3b10e29803ed6699f96cc2eedfe956be6ff365986df55a926`
