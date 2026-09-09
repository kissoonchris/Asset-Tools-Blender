# JSRF Native v37 manifest

## Purpose

v37 fixes the first stable guest-stack corruption exposed by the user's v36 Windows run: retail switch routine `0x000304F0` lost its `0x104`-byte local frame because generic CFG recovery split internal jump-table target `0x0003050F` out of the owning function.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v36 Windows evidence

v36 made a major forward step:

- the old `0x0007E180` return-address failure is absent
- no `[CRASH]` or `[STACKGUARD]` is emitted in the supplied runtime log
- retail D3D reaches `AvSetDisplayMode` with framebuffer `0x001B2000`, pitch 1280
- the runtime remains active past 771,000 kernel calls

The first new stable failure sequence is:

1. computed control flow inside `0x000304F0` reaches retail case `0x0003050F`
2. generated dispatch reports `0x0003050F` unresolved
3. `0x000304F0` returns with ESP `0x00F7FEE0 -> 0x00F7FDDC`
4. the exact `0x104` deficit propagates through `0x00025310`, `0x00011070`, `0x000123E0`, and `0x00013A80`

## Retail boundary

Retail `0x000304F0` starts with `sub esp,0x104`, dispatches through the four-entry table at `0x00030618`, and has two common epilogues that each execute `add esp,0x104; ret`.

The jump table targets are `0x0003050F`, `0x00030592`, `0x000305B9`, and `0x00030601`. The next function begins at `0x00030630`. Data table entry `0x001EC0F0` independently stores literal pointer `0x000304F0`.

v37 pins `[0x000304F0, 0x00030630)` so internal case labels cannot become standalone function starts.

## Regression coverage

`tests/test_v37_switch_cfg_boundary.py` was written first and failed **4/4** against v36. After implementation it passes **4/4**.

The Windows generation pipeline now refuses to run unless generated `sub_000304F0` contains both retail plain-RET epilogues.

## Fresh local verification

- Focused v37 regression: **4 passed**
- Full Python suite: **99 passed**
- Python compile check: **passed**
- Fresh native CMake configure/build: **passed**
- Native CTest suite: **20/20 passed**
- Windows runtime verification is still required; this manifest does not claim the v36 stack leak is fixed until the user's v37 run confirms it.
