# JSRF v45 — NtQueryDirectoryFile ABI correction

## Measured title behavior

The v44 run reaches the retail cache manager and loops on
`JSRF_CACHE_COMPLETE00.CMP` / `JSRF_CACHE_COMPLETE01.CMP` while the cache object
remains enabled with no selected slot. The new v44 `[DIRQUERY]` line never
appeared even though ordinal 207 is the retail directory query used by the
FindFirstFile-style wrapper.

## Retail disassembly

`sub_00145E7E` splits the requested path into directory + mask, opens the
directory through ordinal 202, then calls thunk `0x001C3FB8` (ordinal 207).
Immediately before that call it pushes ten dwords. In right-to-left API order the
last three are:

- arg 7: `1` = `XboxFileDirectoryInformation`;
- arg 8: pointer to the ANSI filename mask;
- arg 9: `0` = `RestartScan`.

The wrapper tests the returned NTSTATUS as signed and converts failure to `-1`.
`0x00025770` then converts that to the boolean file-exists result consumed by
`0x00024EF0` and the cache manager.

## xboxrecomp defect

Pinned `kernel_bridge.c` declared ordinal 207 as 36 bytes and read args 7/8 as
filename/restart. `kernel.h` and `kernel_file.c` likewise omitted the
`FileInformationClass` parameter.

This is not merely a logging mismatch. With JSRF's literal class value `1` in
arg 7, the bridge dereferences guest address 1 as though it were an ANSI_STRING.
The filename filter is therefore lost; the backend can enumerate the directory
without the intended `JSRF_CACHE_COMPLETEXX.CMP` mask. The 36-byte cleanup also
leaves one argument on the simulated stack.

## v45

v45 restores the 10-argument original-Xbox ABI and 40-byte cleanup end to end.
The existing v44 directory-context lifetime correction is retained. The first
Windows v45 run should now produce `[DIRQUERY]` lines containing the actual
marker mask and its returned status; those lines are diagnostic only.
