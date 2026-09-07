# JSRF Native v27 manifest

## What v26 proved

v26 got past the previous DirectSound startup blockers and into JSRF's long-running worker/game code. Its automatic multi-pass test also discovered and safely promoted two additional real retail callback entries: `0x0007BE30` and `0x00024700`.

The same run exposed a deeper runtime problem rather than another ordinary missing function: the hottest `KeWaitForSingleObject` call comes from retail D3D routine `0x0018CE50`, which clears and waits on an embedded Xbox `KEVENT` at device `+0x2430`. The pinned bridge treated that guest dispatcher object pointer as if it were a Win32 handle, so the wait failed immediately instead of sleeping for the GPU event.

Retail disassembly proves the matching signal path. JSRF/XDK connects NV2A interrupt vector 3, its ISR tests master pending bit `0x01000000`, queues a DPC, and the DPC acknowledges PGRAPH then calls `KeSetEvent` on that exact same embedded event. Therefore the correct fix is to restore the hardware interrupt -> retail ISR -> retail DPC -> retail `KeSetEvent` path, not force the wait to succeed.

## v27 changes

1. Advances the pinned xboxrecomp revision from `4d337526dc4ab892483aad0246aee9d1c33d1b81` to `8146ca2741e56e9d16aec1e7b04d8fb8ccd52ba6`, which contains connected-interrupt tracking and deferred DPC infrastructure needed for the real path.
2. Models guest in-memory Xbox `KEVENT` dispatcher objects for `KeSetEvent` and `KeWaitForSingleObject`; guest `SignalState` remains authoritative.
3. Registers JSRF's retail-proven NV2A vblank interrupt shape: vector 3, master pending register `+0x100`, mask `0x01000000`, PGRAPH acknowledge register `+0x600100`, acknowledge value `1`.
4. Delivers vblank at display cadence through JSRF's own connected ISR and DPC instead of setting the event directly.
5. Adds a separate asynchronous kernel/DPC worker so the NV2A thread remains free to observe JSRF's hardware acknowledge and clear the pending condition.
6. Serializes xboxrecomp's shared guest-heap bookkeeping. Worker threads allocate their TIB/TLS blocks concurrently, but the allocator state (`g_heap_next`, block table and count) is process-global. This fixes a real race without changing guest allocation policy.
7. Logs each spawned worker's TIB and `fs:[4]` value, and extends crash diagnostics with thread context so a repeated TLS failure is attributable to a specific worker.
8. If the exact old `sub_00146429` / `0xFFFFFFEC` TLS crash reappears, the easy tester automatically performs one extra mirror-protection diagnostic pass and preserves both baseline and diagnostic logs.
9. Permanently pins the two real retail callbacks discovered automatically by v26: `0x0007BE30..0x0007BFC4` and `0x00024700..0x00024964`.
10. Preserves all prior JSRF-specific correctness fixes: lock-XADD flags, repeated CMPS/SCAS flags, DirectSound boundaries, APU/AC97 activation, NV2A fence mirroring, progress-aware watchdog, automatic safe callback promotion, and the existing retail callable boundaries.

No game return value, HRESULT, wait result, audio result, gameplay state, or arbitrary loop exit is forced.

## Verification

Fresh verification performed on the final v27 tree/package:

- Python tests: **70 passed**
- Root CMake configure/build: passed
- CTest: **20/20 passed**
- v26 -> v27 patch: `git apply --check` passed against a freshly extracted v26 tree
- applied-patch tree: key changed runtime/build files reproduce byte-for-byte
- applied-patch Python suite: **70 passed**
- Easy ZIP integrity (`ZipFile.testzip`): passed
- extracted final Easy ZIP Python suite: **70 passed**
- extracted final Easy ZIP CMake build: passed
- extracted final Easy ZIP CTest: **20/20 passed**
- `START JSRF TEST.bat`: present at ZIP archive root
- Easy/source archives: byte-identical

## Files

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v27.zip`
  - SHA-256 `9c9f1e1731ed6c4784d6e52094e7303f6dce9ac9da9663246a0e23eaa0896660`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX26.zip`
  - SHA-256 `9c9f1e1731ed6c4784d6e52094e7303f6dce9ac9da9663246a0e23eaa0896660`
- `JSRF-V26-to-V27.patch`
  - SHA-256 `bb8b969f1ff0d0974c94b59f43c98c03a50991d582e6f75d5c5ab74fff1be53e`

Full v27 packages and the complete patch are also backed up in the ChatGPT Library `/JSRF` folder.
