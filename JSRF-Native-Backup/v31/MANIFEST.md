# JSRF Native v31 manifest

## Purpose

v31 fixes the concrete xboxrecomp concurrency bug exposed by the v30 worker-stack guard. It does not change retail JSRF gameplay, D3D, audio, allocator, or TLS behavior.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

The pinned revision is intentionally unchanged from v29/v30 so this Windows test isolates one runtime correction. The current upstream tree still declares the same kernel-dispatch selector as process-global, so moving revisions alone would not remove this race.

## What v30 proved

The v30 diagnostic stopped before the old TIB/TLS corruption occurred:

- worker stack top: `0x011EAFF0`
- worker TIB: `0x011EB000`
- valid `fs:[4]`: `0x011EB054`
- first out-of-stack write: `0x011EAFF4`
- generated source: `recomp_0004.c:119002`
- retail site: the `push ecx` before call `0x00147DD2` in worker loop `0x0013B1C0`

Tracing backward showed the worker's guest ESP had already been over-popped. The decisive call was retail `0x00147D12`: its caller entered at `0x011EAF64` and returned at `0x011EAF74`, 8 bytes higher than its retail calling convention permits. Its saved ESI was then restored from the caller return address `0x0013B0B1`, proving the stack was already misaligned before the later worker-loop push.

## Root cause

xboxrecomp already keeps each guest thread's registers, ESP, TIB/FS base, and related execution state in `RECOMP_TLS`, but the kernel-thunk selection handoff was still shared by every host thread:

```c
static int g_kernel_dispatch_slot = -1;
```

`recomp_lookup_kernel()` writes this slot and returns the common `kernel_thunk_dispatch()` function. The dispatcher reads the shared slot later and uses that slot not only to choose the bridge but also to decide how many stdcall argument bytes to add to the calling thread's guest ESP.

With multiple real guest workers active, another host thread can overwrite the selector between lookup and dispatch. The first thread can therefore execute or clean up using a different kernel ordinal's slot. The v30 +8-byte over-pop is exactly the kind of corruption this race produces.

## v31 correction

`scripts/jsrf_xboxrecomp.py` now patches the pinned runtime before code generation so the selector is per host/guest thread:

```c
static RECOMP_TLS int g_kernel_dispatch_slot = -1;
```

The change is idempotent and is applied before JSRF code generation. The v30 first-write stack guard remains enabled in v31 so the next Windows run can prove whether this was the only stack-crossing source or immediately expose an independent one.

## Regression coverage

A v31 regression test was written first. Against the unmodified v30 driver it failed because the kernel-dispatch TLS patch did not exist. After the implementation, the focused v31 tests pass and verify:

1. the selector becomes `RECOMP_TLS`;
2. applying the transform twice is idempotent;
3. the transform is part of the runtime-patch pipeline before code generation.

## Verification

Fresh source-tree verification after the v31 implementation:

- focused v31 regression suite: **3 passed**;
- Python syntax compilation: **passed**;
- full Python suite: **83 passed**;
- fresh native CMake/Ninja build: **passed**;
- native CTest suite: **20/20 passed**.

Windows JSRF runtime verification is still required before claiming the original runtime corruption is fixed in the real title.

## Next Windows evidence

Extract `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v31.zip` into a new folder and double-click `START JSRF TEST.bat`. Return the generated `JSRF_TEST_LOG.txt` and related runtime/source logs if produced.
