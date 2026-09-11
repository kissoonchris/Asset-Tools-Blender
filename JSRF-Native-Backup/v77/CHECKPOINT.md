# JSRF Native v77 Checkpoint

## Status
v77 follows the first successful v76 Windows runtime run.

v76 confirms the generation hotfix and reproduces the same GameObj crash while preserving the fixed title-ADX path (`filled=51164`, `read_pos=51200`). No `[GOBJMUT]` record appears before parent `0x013D1010` is observed corrupt at `loc_00011096`, so the `+0x28` recursive child call is not the operation that changes the parent. This rejects the prior child-subtree overwrite hypothesis.

v77 adds a nested-safe, read-only bracket around the remaining state-changing boundary: the `vtable + 4` indirect call at `loc_00011083`. `[GOBJVCALL]` records the original parent, dispatch vtable, resolved target, returned `ESI`/parent value, and before/after parent fields when either memory or the returned object register changes. The diagnostic does not repair or skip guest state.

## Verification
- v77 TDD red phase: 4 expected failures before implementation
- focused v77: 4/4 passed after implementation
- preserved v74-v77 GameObj regressions: 14/14 passed
- full Python suite: 292/292 passed
- `scripts/jsrf_xboxrecomp.py` py_compile: passed
- `recomp/src/recomp_manual.c` C11 syntax: passed
- source-tree Release build: passed
- source-tree CTest: 20/20 passed
- exact final ZIP re-extracted: 292/292 Python + syntax passed
- exact final ZIP Release build: passed
- exact final ZIP CTest: 20/20 passed
- clean distributable: 223 files
- root `START JSRF TEST.bat`: present
- retail `default.xbe`: absent
- build/Python caches: absent
- fresh v76 + patch -> v77: 223 files, missing 0, extra 0, changed 0

## GitHub lineage
- `jsrf-native-backup-v76-stage` does not exist.
- `jsrf-native-backup-v77-stage` is based on `jsrf-native-backup-v75-stage`.
- A cumulative v75 -> v77 patch payload is stored on the v77 branch so its contents are reconstructible from the actual GitHub parent.

## Artifacts
- `JSRF-Native-v77-CLEAN-PROJECT.zip`
  - SHA256 `09dd58bc357ed7082ccb2a36e0215df8c9274ff72f62fdc0428c4528a506578a`
- `JSRF-v76-CLEAN-to-v77.patch`
  - SHA256 `3399eeb30414647855cb62c6ae5efb50687163781635a95b473f63c61867b107`
- cumulative `JSRF-v75-CLEAN-to-v77.patch` SHA256 `e6b2452e87f2dd4f57661b8ed209e6e343522d8c271b79ba780a391a8c4e981d`
- cumulative patch gzip SHA256 `47b672a9757f249fcb96a3f08b88f983a0e0c62e0791d6558c51171efc2a30e9`
- cumulative patch base64 SHA256 `b560c1309d85d87a6894fc920286331a558440f2d3dd8f3f8ab853e06aead30d`
