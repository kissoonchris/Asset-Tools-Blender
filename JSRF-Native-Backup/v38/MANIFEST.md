# JSRF Native v38 manifest

## Purpose

v38 is the first **visual bring-up** package. The user's v37 Windows run confirms the v37 switch-frame correction: the old `0x000304F0` `0x104`-byte stack leak is gone, no crash/stack guard is emitted, retail `AvSetDisplayMode` is reached, media continues loading, and the main loop remains alive past 822,000 kernel calls.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v37 Windows evidence

- `AvSetDisplayMode` is called with mode `0x0801010D`, format `0x11`, pitch 1280 and framebuffer `0x001B2000`.
- The prior `0x000304F0` ABI/stack violation is absent.
- No `[CRASH]` or `[STACKGUARD]` appears in the supplied runtime log.
- JSRF continues loading retail title, mission, misc and sound data after display setup.
- The main loop remains at a stable guest ESP and the runtime reaches at least 822,246 kernel calls.

## Root cause of the missing window

xboxrecomp already contains two real guest-side visual paths:

1. `RECOMP_FB_WINDOW` scans the framebuffer selected by retail `AvSetDisplayMode` into a Win32 window.
2. `RECOMP_PB_EXEC` consumes the title's NV2A pushbuffer and writes supported clears/2D geometry/textures into guest framebuffer memory.

Both paths are deliberately opt-in upstream. v37 enabled neither, so JSRF could reach display setup and continue running without any host window or executed pushbuffer pixels.

## v38 correction

The easy Windows run now defaults these existing xboxrecomp paths on:

- `RECOMP_FB_WINDOW=1`
- `RECOMP_PB_EXEC=1`
- `RECOMP_PB_SCAN=1`
- `RECOMP_PB_EXEC_VERBOSE=1`
- `RECOMP_PB_UNHANDLED_ALL=1`

`RECOMP_RASTER_TEST` is intentionally **not** enabled. Any pixels shown must come from the retail JSRF framebuffer/pushbuffer, not from a synthetic test triangle.

The scan/verbose flags are diagnostic only. If the first window is black or incomplete, the runtime log will inventory the real NV2A methods JSRF submitted so the next graphics gap can be fixed from evidence.

## Regression coverage

`tests/test_v38_visual_output_defaults.py` was written first and failed against v37 because the visual paths were not enabled. After the v38 change it passes.

## Fresh local verification

- Focused v38 regression: **1 passed**
- Full Python suite: **100 passed**
- Python compile check: **passed**
- Fresh native CMake configure/build: **passed**
- Native CTest suite: **20/20 passed**
- Windows runtime verification is still required; this manifest does not claim that the first visible JSRF frame is correct until the user's v38 run confirms it.
