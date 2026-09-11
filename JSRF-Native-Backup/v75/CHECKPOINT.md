# JSRF Native v75 Checkpoint

## Status
v75 follows the v74 GameObj traversal trace.

The v74 Windows run proves the immediate bad edge is the `+0x30` sibling of parent `0x013D1010`, which points to `0x81688000`. More importantly, `0x013D1010` is not suspicious at the earlier vtable/child boundaries; it is corrupt only after its recursive `+0x28` child subtree returns. The overwrite therefore happens inside recursive child execution, not at the parent sibling read.

v75 adds a nested-safe, read-only parent snapshot around the actual recursive child call in retail `0x00011070` (`recursiveExecDefault`). It records parent/child/depth and parent header/link fields immediately before descending through `+0x28`, then compares the same parent immediately after that child returns. `[GOBJMUT]` is emitted only when a child subtree actually changed its parent. No JSRF address or object value is hardcoded into the detection logic and no guest state is repaired.

## Verification
- focused v75 + preserved v74 regression: 8/8 passed after red phase
- full Python suite: 286/286 passed
- `scripts/jsrf_xboxrecomp.py` py_compile: passed
- `recomp/src/recomp_manual.c` C11 syntax: passed
- source-tree Release build: passed
- source-tree CTest: 20/20 passed
- exact final ZIP re-extracted: 286/286 Python + syntax passed
- exact final ZIP Release build: passed
- exact final ZIP CTest: 20/20 passed
- clean distributable: 219 files
- root `START JSRF TEST.bat`: present
- retail `default.xbe`: absent
- build/Python caches: absent
- fresh v74 + patch -> v75: 219 files, missing 0, extra 0, changed 0

## Artifacts
- `JSRF-Native-v75-CLEAN-PROJECT.zip`
  - SHA256 `10a4656b7b720765eda4ae5098c7e8dbb2c6cfa28d4e1c33403ff6d805327360`
- `JSRF-v74-CLEAN-to-v75.patch`
  - SHA256 `1a9965d9aba568bba7c8c31d52e4113f02de8cd986844066250688d84c1369fa`
- compressed patch gzip SHA256 `439669d835b2d06445252591f99adc5d9ad10653f19ed36a80ff1ad1cb9d7c1d`
- compressed patch base64 SHA256 `fa2f5d4d54f65da7a4951220fe55b5f5dd8137c7b4dec327f8ea13fdb77f196a`
