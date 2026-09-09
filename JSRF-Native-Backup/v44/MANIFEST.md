# JSRF Native v44 manifest

## Purpose

v44 fixes the host-side file-enumeration lifetime defect isolated by the user's
v43 Windows run. v43 is stable: no new crash or unresolved indirect target is
required to explain the stall. Instead, the real retail cache manager becomes
enabled and repeatedly checks numbered completion markers without selecting a
cache slot.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v43 evidence

- all `DmCache00..08.tbl` and `Cache00..08.tbl` source pairs are present;
- the cache manager reaches `enabled=1` while `slot=0xFFFFFFFF`, `state=0`;
- the resource dispatcher finishes category 26 and remains idle;
- the title repeatedly checks `JSRF_CACHE_COMPLETEXX.CMP`;
- kernel ordinal 207 (`NtQueryDirectoryFile`) is active during that marker loop.

## Retail call-chain proof

Exact retail disassembly identifies the marker existence path as:

`0x00024EF0 -> 0x00025770 -> 0x00145E7E`

`0x00145E7E` is the XAPI FindFirstFile-style wrapper: it opens the directory with
ordinal 202 (`NtOpenFile`), then queries the filename with ordinal 207
(`NtQueryDirectoryFile`). It passes `RestartScan=FALSE` and returns `-1` only
when the query fails.

## Root cause and correction

Pinned xboxrecomp caches Win32 directory-search state by native directory
`HANDLE`. The kernel bridge previously consumed the guest handle token and
called `CloseHandle` directly, leaving the corresponding `DIR_CONTEXT` and
`FindFirstFileW` search alive. Windows can reuse the numeric HANDLE on a later
open, allowing a new `NtQueryDirectoryFile(... RestartScan=FALSE)` call to
continue an old `FindNextFileW` enumeration with the wrong filename pattern.

v44 now:

- routes bridge `NtClose` through `xbox_NtClose`;
- releases the matching directory-query context before the host handle closes;
- closes the outstanding find handle with `FindClose`;
- clears the cached handle/search state;
- emits read-only `[DIRQUERY]` lines for JSRF completion-marker checks so the
  next Windows run records the actual filename, restart flag, NT result, and
  IO status.

No retail file, marker, cache state, or success result is synthesized.

## Regression coverage

`tests/test_v44_directory_query_context.py` is the v44 regression. It was run
against the v43 tree before implementation and failed 4/4. It now verifies:

1. the close bridge cannot bypass the file HLE;
2. the Win32 directory context is closed and cleared before `CloseHandle`;
3. the patch is idempotent;
4. completion-marker query result tracing is present;
5. the v44 correction is installed before xboxrecomp code generation.

## Verification

Fresh local verification before packaging:

- focused v43+v44 regression: **11/11 passed**;
- full Python suite: **136 passed**;
- Python compile check: **passed**;
- fresh CMake/Ninja configure/build: **passed**;
- native CTest suite: **20/20 passed**.

Windows runtime confirmation remains required before claiming the SEGA/cache
loop is cleared.
