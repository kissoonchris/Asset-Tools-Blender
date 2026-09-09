# JSRF v38 — first real framebuffer/pushbuffer visual bring-up

## What v37 proved

The v37 Windows run removes the previous `0x000304F0` switch-frame stack leak. The main thread now keeps a stable ESP through `sub_00013F80`, retail D3D reaches `AvSetDisplayMode`, and the title continues loading real game media. There is no crash or stack guard in the supplied run.

## Why there was still no window

The pinned xboxrecomp runtime already has a Win32 framebuffer scanout (`RECOMP_FB_WINDOW`) and a partial NV2A pushbuffer executor (`RECOMP_PB_EXEC`). Both are opt-in by design. The v37 easy-test environment enabled vblank and JSRF's measured APU handshake, but never enabled either visual path.

That means the title can successfully select a display framebuffer and continue its main loop while the host does not scan that framebuffer out and does not execute the NV2A commands that would populate it.

## v38 experiment

Enable only the existing real guest visual paths in the easy Windows runtime environment:

- framebuffer window
- pushbuffer executor
- pushbuffer survey/verbose diagnostics
- complete unhandled-method report

Do **not** enable `RECOMP_RASTER_TEST`, because a synthetic triangle would answer the wrong question. The first visual must be produced by JSRF's own retail command stream.

## Expected outcome

Best case: a Win32 framebuffer window opens after JSRF calls `AvSetDisplayMode`, and supported JSRF clears/UI/texture draws become visible.

If the window is black or incomplete, the new `[GPU]` / `[PB]` diagnostics identify the next unimplemented NV2A method or surface-format path without another blind graphics guess.
