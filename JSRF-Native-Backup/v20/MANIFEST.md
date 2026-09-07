# JSRF Native Backup v20

Date: 2026-09-07
Base: v19
GitHub backup branch: `jsrf-native-backup-v20-stage`

## What the v19 Windows run proved

v19 retained both earlier correctness fixes and no longer watchdogs:

- the v17 `lock xadd` EFLAGS correction prevents the retail COM Release double-free / allocator free-list cycle;
- the JSRF NV2A fence mirror allows retail D3D wait `0x00191440` to reach post-wait `0x001914FC`;
- the title later requests `HalReturnToFirmware(2)` and the host exits normally with code 0.

Retail control-flow analysis identifies the title-requested reboot as the normal XAPI path after the retail game main (`0x0006F9E0`) returns. The main application constructor stores the return from `sub_0005F350` at object `+0x10`; setup `0x00012C10` and run `0x00013F80` both reject a negative value. A non-negative value enters the persistent game loop. The next root-cause question is therefore which subsystem in `sub_0005F350` returns a negative startup status.

## v20 change

v20 is diagnostic only. It extends `recomp/jsrf_trace_functions.json` to trace the retail game-main/startup chain and the direct status-producing calls inside `sub_0005F350`:

- `0x0006F9E0`, `0x00012210`, `0x0005F350`, `0x00012C10`, `0x00013F80`
- `0x0014CAA0`, `0x001BB690`, `0x0014CAF0`, `0x001BB750`
- `0x0005F2C0`, `0x0005F250`, `0x00156DF0`
- `0x001680D0`, `0x00116EC0`
- `0x0014ADA3`, `0x00147532`

No guest state is changed and no gameplay/D3D/audio behavior is replaced. The repeated `NtFreeVirtualMemory` failures are deliberately left untouched until the startup trace proves whether they are causal.

Research note added: `research/JSRF_V19_GAME_MAIN_RETURN.md`.

## Regression coverage

A v20 regression test was written first. It failed against the v19 trace configuration because the new startup chain was absent, then passed after the trace list was extended.

## Fresh verification from the final v20 ZIP

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — PASS
- `python -m pytest -q tests` — **32 passed**
- fresh CMake configure/build — PASS
- `ctest --test-dir build_verify --output-on-failure` — **20/20 passed**
- Easy-test ZIP integrity — PASS
- Source ZIP integrity — PASS
- final ZIP contract check — PASS:
  - `START JSRF TEST.bat` at archive root
  - v17 lock-XADD patch retained and invoked
  - v18/v19 D3D wait instrumentation retained
  - all 16 new v20 startup trace targets present

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v20.zip`
  - SHA-256 `7c67838f5eaae59c16a67d8c04638bb9991c142adf0678f548102667015026c0`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX19.zip`
  - SHA-256 `7c67838f5eaae59c16a67d8c04638bb9991c142adf0678f548102667015026c0`
- `JSRF-V19-to-V20.patch`
  - SHA-256 `e02862b4f5a4d5df7a07c84cdae7e796a3f1fab3f07cb23e0793855609b80a8f`
