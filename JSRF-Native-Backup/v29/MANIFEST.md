# JSRF Native v29 manifest

## Primary change

v29 moves JSRF onto xboxrecomp's newly upstreamed persistent timer-thread vertical blank implementation instead of v28's one-new-Win32-thread-per-vblank adapter.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

Upstream commit title: `Raise the vertical blank a title's D3D8 has been waiting on`.

## Why this version exists

The v28 Windows run reached substantially broader kernel activity than v27, but then crashed in the retail TLS/LastError helper. The affected worker began with a valid TIB (`fsbase=0x011F0000`, `fs:[4]=0x011F0054`) and crashed later with the same TIB base but `fs:[4]=0`.

v28 also revealed a concrete host-side defect: every vblank used a newly-created host thread. Because the ISR helper caches its guest TIB in host TLS, every one of those one-shot threads allocated another 148-byte guest TIB/TLS block. The runtime log consequently climbed into tens of thousands of 148-byte heap allocations.

The new upstream implementation raises vblank from the already-existing timer thread. That thread has one guest stack/TIB and runs the ISR before draining its queued DPCs. v29 enables that upstream path with `RECOMP_VBLANK=1` and does not install the old per-frame host-thread delivery code.

## Compatibility adapter

JSRF's existing host `main.c` still calls:

- `xbox_Nv2aRegisterVBlankInterrupt(...)`
- `xbox_KernelAsyncStart()`

On the new upstream revision these are kept as a thin adapter. Registration is informational because upstream owns vector 3/PCRTC/PMC behavior, while `xbox_KernelAsyncStart()` starts the persistent timer thread before retail code begins.

## Verification

- v29 regression test failed against v28 before the implementation and passes after it.
- Python suite: **76 passed**.
- Native CMake/CTest suite: **20/20 passed**.
- Windows JSRF runtime verification is still required; this manifest does not claim the `fs:[4]` crash is fixed until the user's v29 run proves it.
