# JSRF Native v25 manifest

## What v24 proved

v24 fixed the DirectSound interface comparison. The startup calls that were failing in v23 now return success and JSRF continues much farther into its real initialization path.

The first new missing call is retail address `0x00178F40`. The v24 runtime reaches this address through an indirect/vtable call from retail function `0x00176F70`, but the static recompilation did not generate a callable entry for it. The runtime therefore skipped it. Soon afterward the title faults while processing downstream state in DSOUND function `0x001A19D6`.

Retail disassembly proves `0x00178F40` is a standalone function:
- entry: `0x00178F40`
- final return: `0x001791B8` (`ret 0x14`)
- exclusive end / padding start: `0x001791BB`
- observed indirect call site: `0x0017702F`
- observed caller function: `0x00176F70`

## v25 change

v25:
- seeds `0x00178F40` as a real runtime function;
- pins its retail-proven boundary to `0x00178F40..0x001791BB`;
- records caller evidence `0x00176F70`;
- traces the function on the next Windows run;
- adds regression tests for the seed and exact boundary.

No game return value, HRESULT, DirectSound result, or gameplay state is forced.

## Verification

- Python tests: 43 passed
- CTest: 20/20 passed
- CMake build: passed
- `START JSRF TEST.bat`: present at archive root
- ZIP integrity: passed
- Easy/source archives are byte-identical

## Files

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v25.zip`
  - SHA-256 `0109e00f33f4a579f45e6560b740e9353ffc8b5cc31bbc13e956a885c536996e`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX24.zip`
  - SHA-256 `0109e00f33f4a579f45e6560b740e9353ffc8b5cc31bbc13e956a885c536996e`
- `JSRF-V24-to-V25.patch`
  - SHA-256 `3cc5b3046ebb3bbb4614cd0909f260f605718eaae115d0ca7d98fe9860fcc863`

## Expected next evidence

The next run should no longer report `unresolved target 0x00178F40`. The trace should show entry/exit for `sub_00178F40`, after which the next genuine blocker can be identified from the new runtime log.
