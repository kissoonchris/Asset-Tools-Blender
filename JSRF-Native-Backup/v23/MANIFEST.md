# JSRF Native Backup v23

Date: 2026-09-07
Base: v22 (`jsrf-native-backup-v22-stage`)

## v22 Windows result

v22 successfully activated the upstream xboxrecomp MCPX/AC'97 path:

- the APU MMIO range is trapped;
- the AC'97 codec-ready bit is reported;
- XAudio2 and the standalone MCPX APU initialize;
- the title itself starts the APU by writing SECTL/FECTL.

The previous DirectSound no-driver failure (`0x88780078`) is therefore cleared.
`sub_001680D0` now returns success (`0x00000000`).

The next startup failure is `sub_00116EC0`, which returns `0x800401F1`. That
status propagates through `sub_0005F350`, then through the app setup/run checks,
and the retail title follows its normal firmware routine-2 exit path.

## Retail interface evidence

Static analysis of the exact retail XBE shows that `sub_00116EC0` performs two
COM-style interface calls through the object at global `0x002589F4`:

1. Query the root object for interface IID at `0x001E391C`.
2. Call slot `+0x0C` on the returned interface with IID_IDirectSound8 at
   `0x001E38FC`.

The relevant retail vtable-only methods are padding-delimited standalone
functions with no direct CALL xrefs:

- `0x00168310..0x001683F9` — root QueryInterface-style method; the IID at
  `0x001E391C` returns the secondary interface (`this + 4`).
- `0x00168400..0x00168473` — secondary-interface slot `+0x0C`; resolves the
  initialized DirectSound interface.
- `0x00168480..0x001684DF` — primary/root slot `+0x0C`; returns `0x800401F1`
  when the root is already initialized.

The v22 runtime returns `0x800401F1` from `sub_00116EC0` with ECX still equal to
primary vtable `0x001E3A70`, matching an incorrect dispatch through the primary
slot `+0x0C` rather than the secondary vtable. This is consistent with the
three vtable-only retail entries having been swallowed/aliased incorrectly by
function-boundary recovery.

## v23 correction

v23 does not force an HRESULT or replace DirectSound behavior. It preserves the
retail interface code by making the three proven standalone entries explicit:

- runtime seeds added for `0x00168310`, `0x00168400`, `0x00168480`;
- trace targets added for the same three methods;
- the JSRF retail-boundary post-pass pins exact ends:
  - `0x00168310 -> 0x001683F9`
  - `0x00168400 -> 0x00168473`
  - `0x00168480 -> 0x001684DF`

A research note, `research/JSRF_V22_DSOUND_INTERFACE_BOUNDARY.md`, records the
retail and runtime evidence.

The v17 lock-XADD EFLAGS correction, v19 NV2A guest-memory fence mirror, v21
MCPX APU ownership/MMIO routing, and v22 CRT environment switch remain intact.

## TDD

`tests/test_v23_dsound_interface_boundaries.py` was added first. Its two tests
failed against the v22 behavior because the methods were absent from the seeds
and the retail-boundary post-pass did not pin them. After the v23 correction,
the focused tests pass.

## Verification

Fresh verification from a clean extraction of the final v23 ZIP:

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — PASS
- `python -m pytest -q tests` — 39 passed
- root CMake configure/build — PASS
- root CTest — 20/20 passed
- v22→v23 patch applies cleanly to a fresh v22 tree — PASS
- patched tree exactly matches the v23 package tree — PASS
- Easy-test ZIP integrity — PASS
- Source ZIP integrity — PASS
- final ZIP contract confirms:
  - `START JSRF TEST.bat` is at archive root
  - v17 lock-XADD patch is still invoked
  - JSRF NV2A fence mirror is still registered
  - all three DirectSound interface entries are present in runtime seeds
  - all three are present in trace configuration
  - all three exact retail function ends are pinned
  - no pytest/pycache/build artifacts are packaged

This does not claim that Windows JSRF now reaches the persistent main loop. The
next Windows run is required to verify the corrected secondary-interface
execution and reveal the next real blocker if one remains.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v23.zip`
  SHA-256: `24762b9df49e9cc080f9b0d769c3836b7569cc38cf0dceaa0bbcd664f87e3b60`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX22.zip`
  SHA-256: `24762b9df49e9cc080f9b0d769c3836b7569cc38cf0dceaa0bbcd664f87e3b60`
- `JSRF-V22-to-V23.patch`
  SHA-256: `692a5c77cfea188251cdd05099e5bb05a8e221b66b009e14ca18eb451871eadd`

Full ZIP archives are saved in the ChatGPT `/JSRF/` Files Library. GitHub stores
this manifest and the complete v22→v23 text patch as redundant source backup.
