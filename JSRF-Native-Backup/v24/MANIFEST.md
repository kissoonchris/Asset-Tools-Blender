# JSRF Native Backup v24

Date: 2026-09-07
Base: v23 (`jsrf-native-backup-v23-stage`)

## v23 Windows root cause

v23 proved the earlier APU/AC'97 work remains active and the DirectSound device
creation stage now succeeds. The remaining startup failure moved into the
interface-selection sequence inside retail `sub_00116EC0`.

The v23 traces show:

- `sub_001680D0` returns `0x00000000`;
- `sub_00116EC0` calls retail QueryInterface-style method `0x00168310` with
  IID pointer `0x001E391C`;
- `0x00168310` returns success;
- the caller nevertheless receives the root object `0x0107FE30`, whose vtable
  is `0x001E3A70`;
- slot `+0x0C` therefore dispatches to root method `0x00168480` instead of the
  secondary-interface method `0x00168400`;
- `0x00168480` returns `0x800401F1`, which propagates through startup and causes
  the normal game-main-return/XAPI reboot path.

Exact retail disassembly of `0x00168310` proves why the wrong interface is
selected. The first 16-byte GUID test is:

```
00168321 xor  edx, edx
00168323 mov  ecx, 4
00168328 mov  edi, 001E4C94
0016832D mov  esi, eax
0016832F repz cmpsd
00168331 sete dl
00168334 test dl, dl
00168336 jne  001683E2
```

The requested IID `0x001E391C` has a later direct/same-GUID path that returns
`this + 4`. For the observed object, that is `0x0107FE34`, whose vtable is
`0x001E3A5C`; its slot `+0x0C` is retail `0x00168400`.

## xboxrecomp semantic mismatch

The pinned xboxrecomp instruction lift already executes repeated word/dword
CMPS/SCAS operations and writes their final equality state to `_flags`.

Its block-level flag tracker only recognizes `cmpsb` and `scasb` as flag
producers under a `rep*` instruction. It does not recognize `cmpsw`, `cmpsd`,
`scasw`, or `scasd`.

For JSRF's `xor edx,edx; ...; repz cmpsd; sete dl` sequence, the tracker thus
retains XOR as the most recent flag producer. XOR-self sets ZF, so the lifted
`sete` evaluates true regardless of the four dwords actually compared. That
forces the first root-IID path and returns `this` instead of `this + 4`.

## v24 correction

v24 adds `patch_xboxrecomp_repeated_string_compare_flags()` to the existing
pinned-toolkit correction stage. It changes only block-level EFLAGS tracking so
all repeated comparison widths already implemented by the lifter are treated as
flag producers:

- `cmpsb`, `cmpsw`, `cmpsd`
- `scasb`, `scasw`, `scasd`

The actual comparison loops are untouched. There is no JSRF IID special case,
no forced HRESULT, no guest DirectSound replacement, and no gameplay override.

The v17 lock-XADD EFLAGS fix, NV2A fence mirror, MCPX APU/AC'97 path, and v23
retail DirectSound function boundaries remain present.

## TDD

The new v24 regression first failed because the repeated-string flag correction
did not exist and was not invoked by the pipeline. After the minimal correction,
the focused regression passes and verifies idempotent patching plus execution
before disassembly.

## Fresh verification

Verification was run from a fresh extraction of the final v24 ZIP:

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — PASS
- `python -m pytest -q tests` — **41 passed**
- root CMake configure/build — PASS
- root CTest — **20/20 passed**
- pristine final ZIP contract — PASS
- Easy-test ZIP integrity — PASS
- Source ZIP integrity — PASS
- v23→v24 patch `git apply --check` — PASS
- v23→v24 patch applies to a fresh v23 source tree — PASS
- applied patch reproduces all three changed/new v24 files byte-for-byte — PASS
- applied-tree focused v24 tests — **2 passed**
- `START JSRF TEST.bat` is present at archive root
- no pytest/pycache/build artifacts are packaged

This verification proves the source/package correction, not that the Windows
title has crossed the gate. The next Windows run is required to validate that
retail QueryInterface now returns the secondary `this + 4` interface and to
expose the next real blocker if one remains.

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v24.zip`
  SHA-256: `72f2f4fa8e524a3cf2c7b6e4e1685d7b3a0ac547b5243c9f6a481eb7ec52746f`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX23.zip`
  SHA-256: `72f2f4fa8e524a3cf2c7b6e4e1685d7b3a0ac547b5243c9f6a481eb7ec52746f`
- `JSRF-V23-to-V24.patch`
  SHA-256: `bbaff7fb09ec6c91fb099141a3e3f1da38a8ce70ded4ee88b3f9aef92c2eb34c`

Full ZIP archives are saved in the ChatGPT `/JSRF/` Files Library. GitHub stores
this manifest and the complete v23→v24 text patch as redundant source backup.
