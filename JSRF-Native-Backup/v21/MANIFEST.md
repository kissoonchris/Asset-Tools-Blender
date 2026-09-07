# JSRF Native Backup v21

Date: 2026-09-07
Base: v20 (`jsrf-native-backup-v20-stage`)

## v20 Windows root cause

The v20 trace identifies the first causal negative startup status:

- `sub_001680D0` returns `0x88780078`.
- `sub_0005F350` immediately returns the same value.
- the application constructor stores it at object offset `+0x10`.
- setup `0x00012C10` returns that negative value and run `0x00013F80`
  returns `-1`, so retail game main returns and XAPI reboots.

Pinned xboxrecomp documents the same hardware failure in
`src/kernel/xbox_memory_layout.c`: with the MCPX/AC'97 aperture left zeroed,
Xbox DirectSound polls codec status at `0xFEC00130`, times out, and returns
`DSERR_NODRIVER (0x88780078)`.

## v21 correction

v21 uses xboxrecomp's existing generic MCPX APU hardware model rather than
forcing the DirectSound HRESULT or bypassing retail startup logic:

- enables `RECOMP_AC97_READY` before `xbox_MemoryLayoutInit()`, which causes the
  pinned runtime to expose the AC'97 ready bit and trap only the APU register
  window `0xFE800000..0xFE87FFFF`;
- initializes `mcpx_apu_init_standalone(xbox_GetMemoryBase())`;
- publishes the state through xboxrecomp's existing `g_apu_state`;
- routes only APU-range access violations through the existing Win32
  `apu_hook_handle_mmio()` decoder and leaves all other crash diagnostics intact;
- shuts the APU down before guest RAM is released;
- enables the upstream bounded `RECOMP_APU_TRACE` for the next Windows run.

The v17 `lock xadd` EFLAGS correction and the v18/v19 JSRF NV2A fence mirror
remain present. No guest gameplay/audio function is replaced and no success
HRESULT is fabricated.

## Verification

Fresh verification on the final v21 source/package:

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — PASS
- `python -m pytest -q tests` — 36 passed
- root CMake configure/build — PASS
- root CTest — 20/20 passed
- `git diff --check` on v20→v21 patch construction — PASS
- Easy-test ZIP integrity — PASS
- Source ZIP integrity — PASS
- final ZIP contract confirms:
  - `START JSRF TEST.bat` at archive root
  - v17 lock-XADD patch still invoked
  - JSRF D3D/NV2A fence mirror still registered
  - MCPX APU initialization present
  - APU MMIO VEH delegation present
  - no pytest/pycache artifacts included

This does not claim the Windows title now reaches the persistent main loop; the
next Windows run is required to validate the hardware path and expose any later
DSP/APU handshake blocker.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v21.zip`
  SHA-256: `bb4ce81e06ff404f43c9ce66a707f5e6edeb4486d01c46a9e0332c6314b240d0`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX20.zip`
  SHA-256: `bb4ce81e06ff404f43c9ce66a707f5e6edeb4486d01c46a9e0332c6314b240d0`
- `JSRF-V20-to-V21.patch`
  SHA-256: `f786c820f97ec625e37b4255dba5cfbbc9247bb2fd88b8fa7f8a48a52087e974`

Full ZIP archives are saved in the ChatGPT `/JSRF/` Files Library. GitHub stores
this manifest and the complete v20→v21 text patch as redundant source backup.
