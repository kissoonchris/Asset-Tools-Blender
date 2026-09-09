# JSRF Native v40 manifest

## Purpose

v40 fixes the next stable post-SEGA static-recompilation CFG split exposed by
the user's v39 Windows run. The visible SEGA frame no longer disappears, but
retail execution corrupts its guest stack while entering the adjacent mark
resource loader.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v39 Windows evidence

- The genuine `Presented by SEGA` framebuffer remains visible.
- Indirect-call #72899 exposes unresolved target `0x0002D1F8`.
- The enclosing `sub_0002D080` path returns exactly `0x100` bytes below its
  expected guest ESP.
- The same `-0x100` displacement propagates into `0x00025310`, `0x00011070`,
  `0x000123E0`, and `0x00013A80`.
- Corrupted state then causes repeated null indirect calls from `0x0001194A`
  and `0x00011FEA`.
- NV2A `DMA_PUT == DMA_GET == 0x0007AD2C` after the corruption, explaining why
  the last valid SEGA frame remains on screen.

## Independent boundary evidence

The public JSRF-Decompilation symbol table names:

- `0x0002D080` — `readMarkDefault`
- `0x0002DBE0` — `readMarkPressOrTex`

Thus the observed `0x0002D1F8` destination lies inside `readMarkDefault`, and
must not be treated as a standalone function.

## v40 correction

`JSRF_RETAIL_FUNCTION_BOUNDARIES` now also pins:

`0x0002D080 .. 0x0002DBE0`

The pinner removes generic internal starts in that interval. The adjacent v39
boundary beginning at `0x0002DBE0` remains separate.

Generated-code verification additionally requires `loc_0002D1F8` to be inside
`sub_0002D080` before the Windows executable may launch.

No retail gameplay, resource result, title-state decision, framebuffer content,
GPU command, or kernel result is replaced or bypassed.

## Regression coverage

`tests/test_v40_mark_default_switch_boundary.py` was written first and failed
4/4 against v39. After the v40 correction it passes 4/4 and covers:

1. the exact retail function interval;
2. removal of the observed internal false start;
3. preservation of the adjacent v39 function;
4. generated-code rejection if `0x0002D1F8` is not internal to
   `sub_0002D080`.

The v39 fixture was updated only where its synthetic `0x0002DB00` start is now
known to lie inside the newly proven v40 function interval.

## Fresh verification

- Focused v39 + v40 regression: **8/8 passed**
- Full Python suite: **108 passed**
- Python compile check: **passed**
- Fresh native CMake/Ninja configure/build: **passed**
- Native CTest suite: **20/20 passed**

Windows runtime confirmation is still required before claiming the SEGA-screen
stall is cleared on the user's PC.
