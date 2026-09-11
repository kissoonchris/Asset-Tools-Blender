# JSRF Native v78 Checkpoint

## Status
v78 follows the v77 Windows runtime run.

The v77 logs preserve the title ADX progress (`filled=51164`, `read_pos=51200`) and reproduce the same GameObj crash. `[GOBJVCALL]` only records normal high-bit state transitions on unrelated objects, both with `register_changed=0`. The key correction is retail control flow: when `0x013D1010` is finally observed, `state4=0x8C818000` already has its sign bit set, so `recursiveExecDefault` branches directly from `loc_00011077` to `loc_00011096`. The corrupt object therefore skips both the vtable callback and +0x28 child recursion and arrives through an earlier +0x30 sibling edge already bad.

v78 adds a read-only incoming sibling target tracker. `[GOBJSIB]` records the predecessor parent and the sibling target only when that target is first observed with an invalid GameObj vtable or transitions from a previously valid header to an invalid one. It retains the previous target header so the next Windows run can distinguish an already-bad insertion from memory reuse/overwrite between frames.

## Verification
- v78 TDD red phase: 4 expected failures before implementation
- focused v78: 4/4 passed
- preserved v74-v78 GameObj suite: 18/18 passed
- full Python suite: 296/296 passed
- Python compilation: passed
- `recomp_manual.c` C11 syntax: passed
- source-tree Release build: passed
- source-tree CTest: 20/20 passed
- exact ZIP fresh extraction: 296/296 Python + syntax passed
- exact ZIP Release build: passed
- exact ZIP CTest: 20/20 passed
- clean distributable: 225 files
- root `START JSRF TEST.bat`: present
- retail `default.xbe`: absent
- build/Python caches: absent
- fresh v77 + patch -> v78: 225 files, missing 0, extra 0, changed 0

## Artifacts
- `JSRF-Native-v78-CLEAN-PROJECT.zip`
  - SHA256 `c9e414eb61cdf814e2dc3e7120fc66ad38a4bc06d8ae93217d0c485377a9ded5`
- `JSRF-v77-CLEAN-to-v78.patch`
  - SHA256 `189b20ae216bfe85621b8f160bcad1a93c756211dd80e73ab2f1213a8764316b`
