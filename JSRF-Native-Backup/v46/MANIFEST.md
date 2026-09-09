# JSRF Native v46 manifest

## Purpose

v46 fixes the guest-memory release path exposed by the first successful v45
retail cache build. v45 corrected `NtQueryDirectoryFile`: JSRF now finds a free
cache slot, loads the real cache table, copies retail assets, reaches all 174
entries for slot 1, and creates `JSRF_CACHE_COMPLETE01.CMP`. During that work,
however, the runtime exhausts its guest heap because every observed kernel
ordinal 199 (`NtFreeVirtualMemory`) release returns `0xC0000001`.

## v45 Windows evidence

The returned v45 log proves the cache path is now executing rather than merely
polling marker names:

- `[DIRQUERY]` receives the exact `JSRF_CACHE_COMPLETEXX.CMP` filename mask;
- the cache manager changes from `slot=0xFFFFFFFF/state=0` to slot 1/state 6;
- resource category 31 loads the cache table and reports 174 entries;
- the cache manager walks all 174 entries and reaches state 5;
- `JSRF_CACHE_COMPLETE01.CMP` is successfully created;
- the manager proceeds to slots 2 through 8;
- no unresolved indirect target or stack-guard failure is required to explain
  the remaining stall.

The first heap failure occurs during slot-1 asset copying with roughly the full
48.5 MB guest heap consumed. The run subsequently records thousands of
`xbox_HeapAlloc: out of memory` failures; after slot 1 finishes, even the small
allocation needed to load the next cache table can no longer be satisfied.

## Root cause

Original-Xbox `NtFreeVirtualMemory` is a three-argument API:

1. `PVOID *BaseAddress`
2. `PULONG FreeSize`
3. `ULONG FreeType`

Those first two arguments point to **32-bit fields in guest RAM**. The pinned
xboxrecomp bridge translated the *addresses of those fields* into host pointers
and passed them directly to `xbox_NtFreeVirtualMemory`, whose x64 host signature
uses native `PVOID *` and `SIZE_T *`. The host helper therefore reads the wrong
field widths and calls Win32 `VirtualFree` on a guest allocation that actually
belongs to xboxrecomp's guest heap. In the measured v45 run all directly logged
ordinal-199 calls return `STATUS_UNSUCCESSFUL (0xC0000001)`.

Retail JSRF's wrapper at `0x00145A5D` supplies the expected three arguments and
its measured cache/loader cleanup paths use `MEM_RELEASE (0x8000)` with
`FreeSize == 0`.

## Production changes

v46 replaces only that bridge behavior:

- reads `BaseAddress` and `FreeSize` explicitly as 32-bit guest fields;
- handles `MEM_RELEASE` in guest address space;
- validates the release-size contract;
- releases heap-backed guest virtual allocations with the existing
  `xbox_HeapFree`, preserving the allocator's existing free-block reuse and
  coalescing;
- keeps `MEM_DECOMMIT` distinct and does not release the reservation;
- does not call host `VirtualFree` for these guest heap pointers;
- adds rate-limited `[VMFREE]` diagnostics with the released guest address and
  known heap-block size;
- retains v45's directory-query ABI correction and every earlier runtime fix;
- runtime startup identifies itself as `JSRF Native v46`.

No heap-size increase, fabricated completion marker, forced cache state, or
replacement game behavior is added.

## Regression coverage

`tests/test_v46_ntfreevirtualmemory_guest_bridge.py` covers:

- guest 32-bit IN/OUT field access;
- guest heap release rather than host `VirtualFree`;
- `MEM_RELEASE` size validation;
- release through `xbox_HeapBlockSize` / `xbox_HeapFree`;
- distinct `MEM_DECOMMIT` behavior;
- rate-limited `[VMFREE]` instrumentation;
- patch idempotence;
- pipeline ordering after the thread-safe guest-heap patch;
- unambiguous v46 runtime banner.

## Verification

Fresh verification from the packaged v46 tree:

- focused v46 regression: **5/5 passed**;
- full Python suite: **146 passed**;
- Python `py_compile`: **passed**;
- `recomp_manual.c` C syntax check: **passed**;
- fresh CMake/Ninja native configure/build: **passed**;
- native CTest suite: **20/20 passed**;
- ZIP integrity: **passed**;
- root `START JSRF TEST.bat`: **present**;
- retail payload scan (`default.xbe`, `game/`, `isoextract/`): **clean**.

Windows runtime confirmation remains required before claiming that all cache
slots complete or that the title advances beyond the current SEGA/cache phase.
