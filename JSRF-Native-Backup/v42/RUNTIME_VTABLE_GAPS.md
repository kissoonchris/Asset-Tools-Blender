# JSRF v42 — post-SEGA runtime vtable gaps

## Measured v41 behavior

The v41 Windows run keeps executing after the SEGA presentation frame. The GPU
pushbuffer repeatedly advances and the title polls its own cache-completion
files. The first concrete missing code entries in that path are two indirect
calls:

- `0x0003E230`, call #12576, return site `0x0002FE4D`;
- `0x00069F90`, call #12580, return site `0x0003003F`.

They are not inferred from a later crash. They are direct runtime branch targets.

## Retail evidence for `0x0003E230`

The exact supported retail XBE stores `0x0003E230` in vtable slot
`0x001CA494`. Disassembly shows a separate prologue at `0x3E230`, one
`ret 0x0C` at `0x3E3B3`, padding after the return, and the next function at
`0x3E3C0`.

Therefore the pinned interval is:

`0x0003E230 .. 0x0003E3C0`

## Retail evidence for `0x00069F90`

The same XBE stores `0x00069F90` in vtable slot `0x001CC130`. Disassembly shows
its own prologue at `0x69F90`, `ret 0x0C` exits at `0x6A042` and `0x6A0FA`,
padding after the second return, and the next function at `0x6A100`.

Therefore the pinned interval is:

`0x00069F90 .. 0x0006A100`

## Why the visible stall is not treated as a GPU deadlock

During the same long-running state, `DMA_PUT` and `DMA_GET` repeatedly move
forward and sometimes catch up. The title also continues opening/checking
`Media\Cache\JSRF_CACHE_COMPLETE*.CMP`. That makes the static SEGA visual a
symptom of title-side progress not reaching its next state, rather than proof of
a permanently blocked GPU queue.

v42 therefore restores the two measured missing retail methods first. It does
not force cache completion, alter a title-state result, or fabricate a frame.

## Framebuffer close race

The diagnostic framebuffer window had a host-only race: `WM_CLOSE` cleared the
same `s_fb_running` flag that `xbox_FramebufferWindowStart()` uses as its start
guard. A display update could observe zero before the old worker fully exited
and spawn a replacement window. v42 adds a separate user-close latch and turns
an explicit close into diagnostic exit code 5 so the runner can preserve logs
and stop cleanly.
