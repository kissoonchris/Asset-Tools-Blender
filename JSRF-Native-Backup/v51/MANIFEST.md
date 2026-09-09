# JSRF Native v51 manifest

## Purpose

v50 fixes the deterministic retail vtable/SEH boundary crash at `0x0007B8D0`.
v51 keeps that correction and fixes the newly identified boot/logo background
colour error in the real NV2A pushbuffer executor.

## Windows evidence

The v49 runtime log shows JSRF rendering to a 640x480, 1280-byte-pitch,
2-bytes-per-pixel surface at `0x00084000`. It later submits distinct clear
values including `0x0000FFFF` and `0x000020E4` to that 16-bit surface.

The user reported that the foreground logo itself looked correct while the
Smilebit screen background became cyan.

## Root cause

Pinned xboxrecomp's `nv2a_pb_exec.c` incorrectly states that
`SET_COLOR_CLEAR_VALUE` is always A8R8G8B8. For every 16-bit render target it
therefore converts the 32-bit register value down to R5G6B5.

That is not NV2A behaviour. The clear register is interpreted according to the
active render-target format. xemu's NV2A implementation reads R5G6B5 clear
channels directly from the low 16 bits. JSRF's active surface-format words end
in colour code `0x3`, which is `LE_R5G6B5`.

Therefore retail `0x0000FFFF` is already packed R5G6B5 white. Reinterpreting it
as A8R8G8B8 means RGB `00,FF,FF`, producing exactly the observed cyan.

## v51 correction

`scripts/jsrf_xboxrecomp.py` now patches the pinned pushbuffer executor so a
16-bit target writes:

`(uint16_t)s_gpu.clear_color`

instead of quantizing the register as though it were A8R8G8B8.

No logo asset, retail clear value, title state, or framebuffer pixels are
invented. The original packed retail clear value is preserved.

## Regression coverage

`tests/test_v51_rgb565_clear_value.py` covers the exact pinned-source
transformation, removal of the incorrect ARGB-to-565 conversion, idempotence,
pipeline activation, and the v51 runtime banner.

The tests were observed failing against v50 before the production change, then
passing after the minimal patch.

## Flicker status

The user's flicker report remains tracked separately. This colour correction is
proven by the retail runtime values and the NV2A surface semantics; it does not
claim to fix presentation timing/flicker. The current presenter reads guest
framebuffer memory asynchronously, so that path will be isolated separately
once the next Windows run confirms how far v50/v51 advances.
