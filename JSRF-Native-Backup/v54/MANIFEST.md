# JSRF Native v54

## What the supplied v53 run proves

The returned Windows run identifies itself as `JSRF Native v53`, reaches the
retail display path, stays alive in title state 12, and continues processing a
large real NV2A command stream until the framebuffer window is closed by the
user. The new screenshot is therefore more useful than the earlier all-black
result: pixels are reaching the host window, but the visible result is a black
frame with large opaque white/blocky pieces instead of correctly composited
post-logo UI.

The final v53 GPU report records the active 640x480, 1280-byte-pitch surface at
`0x00084000`, 7,298 draws, 17,097 rasterised triangles and 7,223 textured
batches. Only 75 batches are rejected as non-screen-space, so the visible
failure is not explained by all geometry being skipped.

The same pushbuffer inventory shows state the software rasterizer did not honor:

- `0x012C` FLIP_INCREMENT_WRITE: 2,424 writes
- `0x0130` FLIP_STALL: 2,424 writes
- `0x0300` SET_ALPHA_TEST_ENABLE: 4,853 writes
- `0x0304` SET_BLEND_ENABLE: 9,646 writes
- `0x0344` SET_BLEND_FUNC_SFACTOR: 2,417 writes
- `0x0348` SET_BLEND_FUNC_DFACTOR: 2,417 writes

The old survey table also mislabeled several of these registers, including
`0x0130`, `0x033C`, `0x0340`, and `0x0350`.

## Root graphics gap

The pinned xboxrecomp software executor samples texture ARGB values but its
`put_pixel` path writes them directly to the destination surface. On JSRF's
16-bit R5G6B5 target that conversion drops alpha completely. Transparent or
partially transparent UI/logo texels therefore become opaque RGB pixels. That
matches the kind of solid blocks visible in the supplied v53 screenshot and is
independently consistent with the thousands of retail alpha/blend state writes.

The pinned xboxrecomp revision is also exactly one upstream commit behind the
project's generic five-method NV097 flip-state implementation. v54 ports that
upstream behavior rather than creating JSRF-specific flip rules.

## v54 changes

1. Handle the five standard NV097 flip methods in the software executor:
   `SET_FLIP_READ`, `SET_FLIP_WRITE`, `SET_FLIP_MODULO`,
   `FLIP_INCREMENT_WRITE`, and `FLIP_STALL`. Write advances modulo the retail
   configured buffer count; without physical scanout, FLIP_STALL completes by
   making the read index catch the write index, matching upstream xboxrecomp.
2. Track the standard NV097 alpha-test and blend registers used by JSRF.
3. Apply alpha test before a software-rasterized pixel is committed.
4. Apply fixed-function source/destination blending to 32-bit and R5G6B5
   destinations. The implementation supports the NV097 zero/one, source/dest
   color, source/dest alpha, inverse, source-alpha-saturate, and constant
   color/alpha factors plus ADD, SUBTRACT, REVERSE_SUBTRACT, MIN and MAX.
5. Preserve destination RGB from R5G6B5 by expanding it before blending and
   repacking the result afterward.
6. Add bounded `[GPUBLEND]` state snapshots and `[GPUFLIP]` flip snapshots so a
   returned Windows log still exposes the exact retail state combinations.
7. Correct the pushbuffer survey names for the five flip registers and the
   alpha/blend registers around `0x033C..0x0350`.
8. Retain v53's permanent resource callbacks, all earlier ABI/runtime fixes,
   and the v51 packed-RGB565 clear-value correction.

No title state, texture content, framebuffer address, blend parameter, or menu
transition is fabricated. Unsupported/unknown alpha state passes rather than
rejecting pixels, and an unknown blend equation falls back to fixed-function
ADD instead of adding a JSRF-only special case.

## Scope of this test

v54 is still using xboxrecomp's deliberately limited software rasterizer. It
does not claim full NV2A vertex-program, register-combiner, depth/stencil, or
3D scene fidelity. The immediate test is whether honoring the already-observed
alpha/blend/flip state removes or materially reduces the opaque white/blocky
post-logo result.

Extract the v54 package into a fresh folder and double-click
`START JSRF TEST.bat`. Run through the logos and leave the next screen visible
long enough to judge it. If it is still wrong, take a screenshot, then close the
framebuffer window and return the newly generated test/runtime logs. The runtime
banner must say `JSRF Native v54`.

## Local verification

The v54 regression covers alpha/blend patching, upstream-compatible flip state,
corrected scanner names, patch idempotence, and executable alpha/blend/R5G6B5
math. Full source/package verification is recorded in `V54-VALIDATION.txt`.

Windows runtime behavior is intentionally not claimed here. The supplied v53
run establishes the input evidence; the v54 Windows run is the validation of the
new graphics behavior.
