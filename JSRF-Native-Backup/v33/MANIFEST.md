# JSRF Native v33 manifest

## Purpose

v33 addresses the next Windows blocker exposed by v32: the retail DirectSound
GP DSP command-doorbell wait.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v32 Windows result

The v32 run reached the retail DirectSound scatter/gather setup without the old
worker stack/TLS crash. JSRF's first 0x2000-byte GP command block is the measured
contiguous allocation at `0x8037B000`, so the retail command/status dword used by
`sub_001A1769` is `0x8037B810`.

The APU parsed `RECOMP_APU_DSP_ACK=0x8037B810`, but no later DSP acknowledgement
was logged. The main thread finished the GP scatter/gather table, wrote
`GPSMAXSGE = 0xCE`, and then stopped progressing while background kernel threads
continued.

## v33 correction

The existing xboxrecomp acknowledgement was coupled to `mcpx_apu_dsp_frame()`.
v33 patches the pinned APU so configured DSP doorbells are serviced by a small
asynchronous Windows host worker, matching the fact that the hardware GP DSP
runs independently of the CPU/audio-frame call chain.

The worker:

- uses the existing environment-supplied address list;
- atomically observes and clears non-zero command dwords;
- does not hardcode the JSRF address in reusable xboxrecomp code;
- starts only when `RECOMP_APU_DSP_ACK` is configured;
- stops and joins during APU shutdown;
- leaves the old per-frame acknowledgement as the non-Windows fallback.

No JSRF game function, DirectSound return value, allocator, or spin loop is
replaced.

## Regression coverage

The v33 regression was written first. It failed because no independent DSP
worker existed, then passed after the single APU-model correction. The test also
uses the pinned APU shutdown shape and verifies the patch is idempotent and
actually called by the JSRF pipeline.

## Fresh verification

- focused v33 regression: **1 passed**;
- Python syntax compilation: **passed**;
- full Python suite: **85 passed**;
- fresh native CMake/Ninja build: **passed**;
- native CTest suite: **20/20 passed**.

Windows JSRF runtime verification is still required before claiming the retail
DirectSound command wait is cleared.

## Next Windows evidence

Extract `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v33.zip` into a new folder and
double-click `START JSRF TEST.bat`. If a visible JSRF window appears, leave it
running. Otherwise return the generated runtime/test/source logs.
