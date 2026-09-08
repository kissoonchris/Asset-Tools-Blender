# JSRF Native v32 manifest

## Purpose

v32 carries the v31 kernel-thunk concurrency correction forward and addresses the next Windows blocker: the retail DirectSound GP DSP command-doorbell handshake.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v31 Windows result

The v31 run advanced into the millions of kernel calls without the v30 worker stack guard firing and without the older TLS/`fs:[4]` access violation. The remaining main-thread stall was traced into the retail DSOUND GP command sender.

Retail code writes command `3` to `GP command block + 0x810` and spins until the hardware DSP clears that dword to zero. The GP block is the first 0x2000-byte, 0x1000-aligned contiguous allocation made by the DSOUND initialization path. The v31 runtime recorded that allocation at Xbox VA `0x8037B000`, proving the JSRF command doorbell is `0x8037B810`.

## v32 correction

The pinned xboxrecomp APU already has a generic passthrough-DSP completion model: `RECOMP_APU_DSP_ACK=<guest address>`. Its APU frame thread clears a configured non-zero command dword to zero, matching the completion observation made by the retail CPU code while the DSP56300 cores remain stubbed.

JSRF's Windows runner now defaults that setting to the retail-derived address:

`RECOMP_APU_DSP_ACK=0x8037B810`

An externally supplied value is preserved, so diagnostics can still override it.

No JSRF function, DirectSound return value, allocator, or spin loop is replaced. The retail title still publishes the command and waits; the host APU model performs the missing hardware acknowledgement.

## Regression coverage

The v32 regression was written before the implementation. It failed against v31 because the JSRF runner did not configure any DSP acknowledgement address, then passed after the single runner change.

## Fresh verification

- focused v32 regression: **1 passed**;
- Python syntax compilation: **passed**;
- full Python suite: **84 passed**;
- fresh native CMake/Ninja build: **passed**;
- native CTest suite: **20/20 passed**.

Windows JSRF runtime verification is still required before claiming the retail DirectSound initialization now proceeds beyond this command.

## Artifacts

- Easy test ZIP SHA-256: `7813b0691dee74d65d070a3436b5c5395cca33728fb2b24e88872c98f887a414`
- Source ZIP SHA-256: `7813b0691dee74d65d070a3436b5c5395cca33728fb2b24e88872c98f887a414`
- v31-to-v32 patch SHA-256: `eb46a04b51ef4a9c1816de7dbe8f5822ca605410b3c107d33b7e1974da3f1013`

Full archives and patch are stored in the ChatGPT `/JSRF/` Library.
