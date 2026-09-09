# JSRF v44 — directory-query context lifetime

## v43 Windows evidence

The v43 run is stable and remains on the SEGA frame while the title repeatedly
checks `JSRF_CACHE_COMPLETE00.CMP` through `08.CMP`. The cache manager becomes
enabled but stays at slot `0xFFFFFFFF`, state 0. All nine retail
`DmCacheXX.tbl` and `CacheXX.tbl` source pairs are present in the user's
extracted game.

The runtime also shows kernel ordinal 207 (`NtQueryDirectoryFile`) active during
the completion-marker loop. That is the important host boundary for v44.

## Exact retail existence path

Disassembly of the supported retail XBE gives the marker test chain:

`0x00024EF0 -> 0x00025770 -> 0x00145E7E`

`0x00024EF0` formats the numbered completion-marker path and calls `0x25770`.
`0x25770` calls `0x145E7E` and returns 1 if that function returns anything other
than `-1`.

`0x00145E7E` is the XAPI FindFirstFile-style wrapper. It:

1. splits the supplied path into directory and filename pattern;
2. opens the directory through kernel thunk slot 8, ordinal 202 (`NtOpenFile`);
3. queries the filename through slot 22, ordinal 207 (`NtQueryDirectoryFile`);
4. returns `-1` when that query fails, otherwise returns the directory handle.

The retail query passes `RestartScan = FALSE` even though each existence test
opens a fresh directory handle.

## Host-side defect

Pinned xboxrecomp's Windows file HLE keeps a `FindFirstFileW`/`FindNextFileW`
`DIR_CONTEXT` keyed by the native directory `HANDLE`.

Two pieces did not share one lifetime:

- `kernel_file.c` kept the directory enumeration context alive;
- `bridge_NtClose` removed the handle-table token and called `CloseHandle`
  directly, bypassing `xbox_NtClose` and any file-HLE cleanup.

The context therefore survived after the directory handle was closed. Windows
may recycle the same numeric handle value on the next open. Because JSRF calls
`NtQueryDirectoryFile(... RestartScan=FALSE)`, the recycled handle can match the
old `DIR_CONTEXT` and continue its old `FindNextFileW` search instead of starting
`FindFirstFileW` with the new `JSRF_CACHE_COMPLETEXX.CMP` pattern.

That can turn a missing marker into a false successful existence result without
crashing or corrupting the title stack, matching the stable v43 loop.

## v44 correction

The JSRF xboxrecomp patch stage now:

- routes the kernel `NtClose` bridge through the existing `xbox_NtClose` file
  HLE instead of calling host `CloseHandle` directly;
- makes the Win32 `xbox_NtClose` release every directory query context owned by
  the handle before closing it;
- calls `FindClose` for the outstanding search handle and clears
  `file_handle`, `find_handle`, `first_done`, and cached find data;
- adds a read-only `[DIRQUERY]` record for the JSRF completion-marker pattern,
  including the real `NtQueryDirectoryFile` status and restart flag.

No completion marker is created, no cache-manager state is forced, and no file
result is fabricated.

## Expected v44 Windows result

For a completion marker that is genuinely absent, `[DIRQUERY]` should report a
failing/no-more-files status for that exact pattern rather than inheriting a
previous search. The retail cache manager should then be free to choose the
missing slot and advance into its normal cache-table path. A Windows run is
still required to prove that transition.
