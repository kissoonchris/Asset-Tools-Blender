# JSRF Native v45 manifest

## Purpose

v45 corrects the original-Xbox `NtQueryDirectoryFile` ABI used by JSRF's cache
completion-marker checks. The user's v44 runtime remains alive and rendering but
repeatedly tests `JSRF_CACHE_COMPLETE00/01.CMP` while the cache manager remains
`enabled=1`, `slot=0xFFFFFFFF`, `state=0`.

## Root cause

Retail JSRF function `0x00145E7E` is the FindFirstFile-style wrapper used by the
marker probe. At `0x00145F2D..0x00145F4A` it pushes ten dwords before calling the
kernel thunk at `0x001C3FB8` (ordinal 207):

1. FileHandle
2. Event
3. ApcRoutine
4. ApcContext
5. IoStatusBlock
6. FileInformation
7. Length (`0x148`)
8. FileInformationClass (`1`, `XboxFileDirectoryInformation`)
9. FileMask (ANSI_STRING)
10. RestartScan (`0`)

The pinned xboxrecomp bridge modeled ordinal 207 as nine arguments / 36 bytes,
read `STACK_ARG(7)` as `FileName`, and `STACK_ARG(8)` as `RestartScan`. That makes
literal `1` look like an ANSI_STRING pointer, shifts the real mask into the
restart slot, and leaves one dword on the guest stack after every call.

Cxbx-Reloaded's original-Xbox kernel declaration independently confirms the
10-argument layout: `FileInformationClass`, `FileMask`, then `RestartScan` after
`Length`.

## Production changes

- ordinal 207 stdcall cleanup: 36 -> 40 bytes;
- bridge reads `FileInformationClass` from stack arg 7;
- bridge reads `FileName` from stack arg 8;
- bridge reads `RestartScan` from stack arg 9;
- kernel declaration and both Win32/POSIX backends accept the information class;
- only `XboxFileDirectoryInformation` (class 1), which JSRF uses here, is
  accepted by the current directory-information backend;
- v44's directory-context close fix and `[DIRQUERY]` trace are retained;
- runtime startup identifies itself as `JSRF Native v45`.

No completion file is synthesized and no game state is forced.

## Regression coverage

`tests/test_v45_ntquerydirectoryfile_abi.py` verifies:

- ten-argument bridge slot mapping;
- 40-byte stdcall cleanup;
- header/backend signature propagation;
- unsupported information classes are not silently treated as directory info;
- idempotence;
- v45 runs before the v44 directory-lifetime patch;
- v44 `[DIRQUERY]` tracing composes with the corrected ABI;
- the runtime has an unambiguous v45 banner.

## Verification

Fresh verification from the final packaged tree:

- focused v44/v45 regression suite: **9/9 passed**;
- full Python suite: **141 passed**;
- Python `py_compile`: **passed**;
- fresh CMake/Ninja native configure/build: **passed**;
- native CTest suite: **20/20 passed**;
- root `START JSRF TEST.bat`: **present**;
- retail payload scan (`default.xbe`, `game/`, `isoextract/`): **clean**.

Windows runtime confirmation is still required; do not infer post-SEGA progress
until a returned log contains the `JSRF Native v45` banner.
