# JSRF Native Checkpoint 08 Hotfix 14 / Easy Test v15

## v14 result

The v14 Windows run no longer reproduced the `0x00160080` stack corruption or access violation. It survived until xboxrecomp's 20-second diagnostic watchdog stopped it with code 3 after 461 indirect calls.

The watchdog's recent indirect targets were `0x00160080`, `0x00177FE0`, `0x0015FD40`, `0x00160EA0`, `0x00160D80`, `0x00160CE0`, kernel critical-section enter/leave, then `0x0015FF10` and another critical-section enter. Retail disassembly identifies `0x0015FF10 -> 0x00166760` as DirectInput8 creation (version 0x800, DirectInput8 CLSID/IID), with the direct chain `0x00166760 -> 0x001666E0 -> 0x00166660`. The watchdog stack also contains the retail heap allocator `0x001497DC`.

The 20-second watchdog is only a diagnostic timer; this result alone does not prove a deadlock. The kernel log budget ended before the final lock call, so v15 is a diagnostics-only probe rather than a speculative behavior change.

## v15 changes

- Default `RECOMP_WATCHDOG_SECS`: 20 -> 60.
- Default `RECOMP_KERNEL_LOG_BUDGET`: 5000.
- Default `RECOMP_CS_TRACE_CRT`: `all`.
- Default `RECOMP_TRACE_ARGS`: 8.
- Added `recomp/jsrf_trace_functions.json` covering the DirectInput8 creation chain, its vtable methods, and heap allocator.
- `tools.recomp` now receives `--trace-functions` for that probe.
- No JSRF gameplay, input semantics, synchronization semantics, or retail function bodies are replaced.

## Verification

- Full Python suite: 23/23 passed.
- CTest: 20/20 passed.
- `python -m py_compile scripts/jsrf_xboxrecomp.py`: passed.
- ZIP integrity: passed; all required v15 diagnostic files present.

## Artifacts

- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v15.zip`
  - SHA-256: `f6361d3841ba0215f5952aca631df8381071525e6c2868303549d55edb2de490`
  - Library ID: `libfile_358c5ad2f74481918dc4da2ca262429c`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX14.zip`
  - SHA-256: `f6361d3841ba0215f5952aca631df8381071525e6c2868303549d55edb2de490`
  - Library ID: `libfile_9bc8d3d93dd88191a40a0989459f90c9`
- `/JSRF/JSRF-V14-to-V15.patch`
  - SHA-256: `b9a3176b22b0856194198ce8e0a1fd1482b5f228e3e4aca4936a72ad25f147dd`
  - Library ID: `libfile_737c6cf43a688191b91c86bffe454134`
