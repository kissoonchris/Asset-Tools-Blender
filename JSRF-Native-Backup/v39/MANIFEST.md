# JSRF Native v39 manifest

## Purpose

v39 fixes the first stable execution corruption observed **after the first real
JSRF visual**. v38 displays the retail `Presented by SEGA` framebuffer and then
turns black. The runtime trace identifies a new switch-table CFG split in retail
routine `0x0002DBE0`.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v38 Windows evidence

- The Win32 framebuffer window opens at 640x480.
- Retail `AvSetDisplayMode` selects format `0x11`, pitch 1280 and framebuffer
  `0x001B2000`.
- The user visibly sees the genuine `Presented by SEGA` frame.
- The NV2A executor reports 240 clears, 242 draws, 549 rasterised triangles and
  241 textured batches in the measured title sequence.
- The first stable post-logo ABI break is `sub_0002DBE0`.
- Internal indirect target `0x0002E0CD` is surfaced as unresolved and the owning
  call returns `0x10C` bytes below its expected guest ESP.
- The later `0x00011070`/null-vcall failure is downstream corruption, not the
  root cause.

## Retail boundary proof

Direct disassembly of the supported retail XBE proves:

- `0x0002DBE0` begins with a `0x100`-byte local stack allocation;
- it uses an 11-entry computed jump table at `0x0002E13C`;
- `0x0002E0CD` is one of those internal case destinations;
- the body has eight plain-RET epilogues;
- `0x0002E170` is the next real function start;
- retail data VA `0x001EC154` contains literal pointer `0x0002DBE0`.

## v39 correction

`JSRF_RETAIL_FUNCTION_BOUNDARIES` now pins:

`0x0002DBE0 .. 0x0002E170`

The pinner removes all internal jump-table case starts inside that interval.
Generated-code verification additionally requires all eight retail plain-RET
exits before the Windows runtime may launch.

No retail gameplay, title-state decision, framebuffer content, or GPU command
is replaced or bypassed.

## Regression coverage

`tests/test_v39_title_state_switch_boundary.py` was written first. Against v38
it failed **4/4**, covering the missing boundary, internal case removal and the
generated-code RET contract. After the v39 implementation the focused suite
passes **4/4**.

## Fresh packaged verification

- Focused v39 regression: **4/4 passed**
- Full Python suite: **104 passed**
- Python compile check: **passed**
- Fresh native CMake configure/build: **passed**
- Native CTest suite: **20/20 passed**
- ZIP integrity: **passed**
- `START JSRF TEST.bat` is present at the archive root

Windows runtime confirmation remains required; this manifest does not claim the
post-SEGA blackout is fixed until the user's PC run proves it.
