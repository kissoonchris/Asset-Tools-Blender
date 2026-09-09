# JSRF Native v43 manifest

## Purpose

v43 targets the first stable title-side loop exposed by the user's v42 Windows
run. v42 no longer crashes, loses indirect targets, or duplicates the framebuffer
window when closed. Instead, the retail process remains alive and repeatedly
cycles through its cache-completion state while the GPU continues submitting and
consuming work.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v42 Windows evidence

- no `[CRASH]`, `[STACKGUARD]`, unresolved indirect target, or skipped non-code
  indirect call appears in the measured runtime;
- explicit framebuffer close exits cleanly with diagnostic code 5;
- the GPU continues advancing/rasterising for the duration of the run;
- after the title reaches its cache phase, filesystem traffic repeatedly probes
  `Z:\Media\Cache\JSRF_CACHE_COMPLETEXX.CMP`;
- the retail source-table names `D:\Media\Cache\DmCacheXX.tbl` never appear in
  the v42 runtime log.

This means the next useful boundary is the retail cache/resource-dispatch state,
not another broad GPU or kernel wait.

## Retail cache callback correction

Direct disassembly of the supported retail XBE identifies three adjacent
resource-update callbacks installed in the literal callback array at
`0x001EC0F0`:

- category 30: `0x00026170 .. 0x00026200`, pointer at `0x001EC168`;
- category 31: `0x000262B0 .. 0x00026390`, pointer at `0x001EC16C`;
- category 32: `0x000263C0 .. 0x000264A0`, pointer at `0x001EC170`.

Category 30 contains an embedded computed jump table. Categories 31 and 32 each
contain multiple early plain-RET exits around a 0x104-byte local frame. v43 pins
all three complete retail bodies so generic recovery cannot truncate an update
callback at an internal state return.

Generated-code verification requires the retail plain-RET exit counts:

- `0x26170`: 4 exits;
- `0x262B0`: 3 exits;
- `0x263C0`: 3 exits.

## Read-only cache diagnostics

Because a Windows run is still needed to prove which cache state is active,
v43 also adds throttled, read-only probes at:

- `0x00025040` — cache manager state machine;
- `0x00025310` — shared resource dispatcher update;
- `0x00025390` — shared resource dispatcher start;
- `0x00025DD0` — asynchronous D:/Z: source/cache loader;
- `0x000262B0` — category-31 DmCache table updater;
- `0x000264D0` — category-31 initializer.

The probes emit `[CACHESTATE]`, `[CACHEDISPATCH]`, and `[CACHETABLE]` records.
They read guest registers/memory only and do not alter game state.

The beginner runner also inventories the exact retail source table files
`Media\Cache\DmCache00..08.tbl` and `Cache00..08.tbl` before launching. The
result is captured automatically as `[CACHEPREFLIGHT]` lines in the normal test
log, so the user does not need to inspect the extracted game manually.

## Regression coverage

`tests/test_v43_cache_state_callbacks.py` was written before implementation and
failed 7/7 against the v42 tree. After the v43 changes it passes 7/7 and covers:

1. exact cache-update callback intervals and data-table evidence;
2. removal of internal false starts while preserving adjacent methods;
3. generated cache-state probe insertion at all six retail landmarks;
4. rejection of missing generated entry labels;
5. presence of the read-only runtime probe implementation;
6. pipeline activation of the v43 probe and generated-code contract;
7. automatic source-table preflight in the Windows runner.

## Fresh local verification

- focused v43 regression: **7/7 passed**;
- full Python suite: **132 passed**;
- Python compile check: **passed**;
- `recomp_manual.c` C syntax check: **passed**;
- fresh native CMake/Ninja configure/build: **passed**;
- native CTest suite: **20/20 passed**.

Windows runtime confirmation is still required. v43 does not claim the SEGA/cache
loop is cleared until the user's PC run demonstrates it; if the boundary pin is
not sufficient, the new cache-state records are intended to identify the exact
remaining retail or host-side failure in one run.
