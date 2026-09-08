# JSRF Native v28 manifest

## What the v27 user run proved

The v27 runtime reached and connected the retail NV2A vector-3 ISR, then entered a sustained interrupt/DPC insertion storm. The captured run ended at 275,187 total kernel calls, of which 271,971 were ordinal 119 (`KeInsertQueueDpc`) from retail return address `0x00193CFD` with ESP `0x007FFFCC`. The same run did eventually reach ordinal 145 (`KeSetEvent`) from `0x00193E62`, proving the retail DPC/event path can complete.

## Root cause

v27 moved deferred DPC execution to the kernel async worker but still called the connected guest ISR synchronously from `jsrf_nv2a_vblank_tick()`, which itself runs on the NV2A acknowledge-poll thread. The retail DPC writes the PGRAPH W1C acknowledge register, but the host code that observes that register and clears the master pending bit could not run while the same thread was blocked inside the guest ISR. The ISR therefore continued to observe the interrupt as pending and repeatedly called `KeInsertQueueDpc`.

## v28 correction

1. The connected guest ISR is launched on a separate host worker thread.
2. The NV2A poll thread remains free to observe the retail DPC's PGRAPH W1C acknowledge while the ISR is active.
3. Only one vblank interrupt raise is allowed in flight at once.
4. A still-pending source is not re-raised before the guest acknowledge is observed.
5. A failed host ISR-dispatch launch withdraws the synthetic pending bit instead of leaving it asserted.
6. The host still does not signal JSRF's embedded event directly; the retail ISR/DPC/KeSetEvent/KeWaitForSingleObject path remains authoritative.
7. The cached xboxrecomp working tree is force-restored to the pinned upstream commit before each generation pass. This prevents v27's tracked source patches from surviving in `%LOCALAPPDATA%\JSRF-Native-Cache` and bypassing the v28 transformation when both versions pin the same upstream revision.

Pinned xboxrecomp revision remains:

`8146ca2741e56e9d16aec1e7b04d8fb8ccd52ba6`

## Verification

- New v28 regression test was first observed failing against v27 behavior, then passing after the fix.
- Python suite: **72 passed**.
- Native CMake/CTest suite: **20/20 passed**.
- Easy-test ZIP contains `START JSRF TEST.bat`, the v28 runtime patch, the regression test, and the v28 root-cause note.
- A Windows JSRF runtime run is still required to verify the real title gets past this interrupt boundary.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v28.zip`
  - SHA-256: `18cc207b49cf84a32e0c666a887c6ad8ceb18e38ab4c1c33d2742de3fa2eee3e`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX27.zip`
  - SHA-256: `18cc207b49cf84a32e0c666a887c6ad8ceb18e38ab4c1c33d2742de3fa2eee3e`
- `JSRF-V27-to-V28.patch`
  - SHA-256: `315077b6b4c8101b8fabd8cecfbd2c0201a7a12922fb02d68be21cc3b1c2e434`

Persistent copies are also stored in the ChatGPT Library `/JSRF` workspace under those names.