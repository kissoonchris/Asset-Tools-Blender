# JSRF Native v18 Backup Manifest

## Status

v17 removed the previous retail heap/free-list stall. The Windows v17 run advanced from 461 indirect calls to 3,915 and reached the retail D3D shutdown/wait path before the 60-second watchdog.

The new v17 blocker is an NV2A GPU completion/fence wait in retail D3D, not a title heap lock or a kernel wait.

## Evidence from the retail XBE

The final guest chain reaches retail D3D `sub_00191440` and spins at `0x001914F0..0x001914F8` while reading a completion word through `device + 0x34` and comparing it against the submitted count at `device + 0x30`.

The retail D3D path allocates the completion block with `MmAllocateContiguousMemoryEx` (ordinal 166), stores that pointer at `device + 0x34`, and deliberately writes `0xDEADBEEF` into the completion word before waiting for the NV2A to update it. Therefore the `DEADBEEF` seen in the watchdog is an intentional retail sentinel, not random corruption.

The JSRF retail device pointer global is `0x0019DCE0`, giving the exact chain:

- device pointer: `MEM32(0x0019DCE0)`
- submitted/PUT count: `MEM32(device + 0x30)`
- GPU completion pointer: `MEM32(device + 0x34)`
- GPU completion value: `MEM32(MEM32(device + 0x34))`

## v18 change

The pinned xboxrecomp runtime already contains a generic NV2A fence-mirroring mechanism for this exact hardware behavior:

`xbox_Nv2aMirrorFence(device_ptr_va, put_off, get_ptr_off)`

v18 registers JSRF's evidence-proven retail chain as:

`xbox_Nv2aMirrorFence(0x0019DCE0u, 0x30u, 0x34u)`

This does not patch or bypass JSRF's retail D3D/game logic. It supplies the missing host-side NV2A completion behavior by using the runtime's existing generic hardware-emulation mechanism.

v18 also adds targeted, read-only `[D3DWAIT]` probes at measured retail addresses inside `sub_00191440` so the next Windows run can show the exact wait state if execution still stalls there.

The separate repeated `NtFreeVirtualMemory` failures visible late in v17 are deliberately not changed in this revision. They are a separate bridge/runtime issue and should only be modified if they remain relevant after the GPU fence wait is cleared.

## Changed source files

- `recomp/src/main.c`
- `recomp/src/recomp_manual.c`
- `scripts/jsrf_xboxrecomp.py`
- `tests/test_jsrf_xboxrecomp.py`
- `research/JSRF_V17_D3D_FENCE_STALL.md` (new)

## Verification

Fresh local verification for v18:

- Python syntax compilation: passed
- Python test suite: 28 passed
- CMake configure/build: passed
- CTest: 20/20 passed, including `xboxrecomp_integration`
- Easy-test ZIP integrity: passed
- Source ZIP integrity: passed
- `START JSRF TEST.bat` is present at the archive root

The Linux verification does not prove the Windows title runtime has cleared this blocker. The next Windows easy-test run is required to validate the fence registration against the retail title and reveal the next blocker, if any.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v18.zip`
  - SHA-256: `78be771d40732072fba98d69c23770c6cda09a3a532882b59a79d945b1382507`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX17.zip`
  - SHA-256: `78be771d40732072fba98d69c23770c6cda09a3a532882b59a79d945b1382507`
- `JSRF-V17-to-V18.patch`
  - SHA-256: `0b04c77386a08a72f4babf15ba63a428d8fdc07b55d868af2abdae23687fd51b`

The full ZIP archives are kept in the ChatGPT `/JSRF/` Files Library. GitHub stores this manifest and the complete v17 -> v18 text patch as a redundant source backup.
