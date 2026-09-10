# JSRF Native v53 backup

v53 permanently restores the two additional retail indirect-call targets exposed by the v52 Windows auto-pass chain and ports later-upstream xboxrecomp NV097 flip-state handling into the pinned JSRF toolkit.

## Runtime evidence carried forward

- v52 restored `0x000521B0..0x00052350`.
- v52 auto-pass then exposed adjacent callback `0x00052350..0x00052460` through the same retail state dispatcher.
- the next auto-pass exposed vtable method `0x000425D0..0x00042750` from indirect caller `0x00031AEB` with literal pointer evidence at `0x001CA5D0`.
- after both were runtime-promoted, pass 3 had no unresolved indirect calls and no crash while the title continued submitting thousands of GPU draws.

## v53 changes

- permanent boundary `0x00052350..0x00052460`, pointer evidence `0x001F98A8`, two plain RET exits required after codegen;
- permanent boundary `0x000425D0..0x00042750`, pointer evidence `0x001CA5D0`, one `ret 0x0C` exit required after codegen;
- later-upstream xboxrecomp NV097 flip state ported into the pinned revision: `SET_FLIP_READ`, `SET_FLIP_WRITE`, `SET_FLIP_MODULO`, `FLIP_INCREMENT_WRITE`, and `FLIP_STALL`;
- read-only `[GPUSURFACE]` and `[GPUFLIP]` diagnostics added to isolate the remaining black post-logo output/flicker;
- no forced menu/loading state and no speculative framebuffer-selection change.

## Verification

Fresh extraction of the final v53 package passed 180/180 Python tests and 20/20 native CTest tests, plus Python compile, fresh CMake/Ninja build, ZIP integrity, root-launcher, retail-payload scan, flip-patch C syntax fixture, and patch-apply verification.
