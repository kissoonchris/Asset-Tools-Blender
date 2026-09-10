# v53: post-logo callback recovery and NV2A flip-state bring-up

## What v52 proved

Restoring `0x521B0` was not the end of the callback-recovery chain. The v52 easy-test's automatic pass logic found `0x52350`, rebuilt, then found `0x425D0` and rebuilt again. Each target came from the running retail image rather than speculative static promotion.

With both entries present, pass 3 no longer reports unresolved indirect targets or a crash. The title reaches state 12 and continues doing substantial work. The GPU report reaches 7,185 clears, 7,264 draws, 16,799 rasterised triangles, and 7,217 textured batches in the captured run. Texture usage includes the 640x480 linear framebuffer texture and two 512x512 DXT resources.

## Callback 0x52350

The callback table around `0x001F9888` places `0x521B0` at `0x001F98A4` and `0x52350` at `0x001F98A8`. Retail disassembly makes `0x52350..0x52460` one padding-delimited function with two plain RET exits. v53 permanently pins it instead of relying on runtime promotion.

## Vtable method 0x425D0

Pass 2 reaches indirect target `0x425D0` from `0x31AEB`; the target also appears as the literal vtable slot at `0x001CA5D0`. Retail disassembly makes `0x425D0..0x42750` one function with one `ret 0x0C` exit. v53 pins the body and verifies that cleanup contract after codegen.

The pass-2 fault inside `0x6E970` occurred only before this method was promoted; pass 3 runs without the unresolved target and without the crash. The later bad pointer read is therefore not patched directly.

## Remaining black-screen lead

Pass 3 keeps decoding and rasterising real pushbuffer work. The captured GPU state remains a 640x480 16-bit surface while the pushbuffer inventory contains many NV097 buffer-flip commands. The pinned xboxrecomp revision predates executor support for `SET_FLIP_READ`, `SET_FLIP_WRITE`, `SET_FLIP_MODULO`, `FLIP_INCREMENT_WRITE`, and `FLIP_STALL`.

Later upstream xboxrecomp tracks read/write/modulo indices, advances write modulo the configured count, and completes software `FLIP_STALL` by synchronizing read to write. xemu independently models the same PGRAPH read/write index semantics and performs a surface update at flip stall.

v53 ports that upstream state behavior to the pinned toolkit and adds `[GPUSURFACE]` / `[GPUFLIP]` telemetry. The framebuffer-window selection remains unchanged until runtime evidence proves which presentation address/index is required.
