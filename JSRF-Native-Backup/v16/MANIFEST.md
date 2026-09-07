# JSRF Native v16 — heap free-list stall probe

Date: 2026-09-06
Base backup branch: `jsrf-native-backup-checkpoint08-stage`
Base backup commit: `4b6a08334ae750c68005949430c0fc9ecbcf401e`
Retail XBE SHA-256: `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`
Pinned xboxrecomp: `4d337526dc4ab892483aad0246aee9d1c33d1b81`

## v15 Windows finding

The uploaded v15 trace proves the final allocator call enters the heap critical
section successfully, then remains inside retail `sub_001497DC` until the
60-second watchdog fires.  The watchdog register tuple maps to the allocator's
large free-list walk.  This makes a malformed/cyclic large free list the current
leading hypothesis, but v15 does not expose the node links needed to prove it.

## v16 change

v16 is diagnostic-only.  It injects `jsrf_heap_probe()` calls after exact retail
labels inside `sub_001497DC` at:

- `0x00149832`
- `0x001499B3`
- `0x00149A1D`
- `0x00149B1A`
- `0x00149B28`
- `0x00149C09`
- `0x00149D5B`

The callback logs the live node, next/prev links, size units, sentinel links,
registers and hit count.  It performs no guest writes and does not alter retail
gameplay, synchronization, allocator branching, rendering, or input semantics.

## Verification

Fresh verification from the v16 source tree:

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — pass
- `python -m pytest -q tests` — 25 passed
- CMake configure/build — pass
- CTest — 20/20 passed
- Both distributable ZIPs pass `unzip -t`
- `START JSRF TEST.bat` is present at archive root

## Persistent artifacts

ChatGPT Files Library `/JSRF/`:

- `JSRF-Native-Source-Checkpoint-08-HOTFIX15.zip`
  - SHA-256 `3fd6594bd00de850b37d45a69c1f7ac4e62d17d70f02cd9ac9772c5910f006ff`
- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v16.zip`
  - SHA-256 `3fd6594bd00de850b37d45a69c1f7ac4e62d17d70f02cd9ac9772c5910f006ff`
- `JSRF-V15-to-V16.patch`
  - SHA-256 `b15c81bd8be7836cc0ec5fa2bab68a36b43331c2cd916d1835b1b8eb45c509e2`

GitHub backup branch:

- `jsrf-native-backup-v16-stage`
- Stores this manifest, the v15→v16 patch, and complete copies of every modified
  text source file needed to recover the v16 diagnostic delta.

## Next Windows evidence needed

Run `START JSRF TEST.bat` from the v16 easy-test archive and return the generated
`JSRF_TEST_LOG.txt` (plus runtime/stackwalk logs if produced).  Repeated
`[HEAPPROBE]` tuples will identify the exact stalled free-list loop and cycle.
