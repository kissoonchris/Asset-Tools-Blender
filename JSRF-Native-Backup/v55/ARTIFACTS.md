# JSRF Native v55 artifacts

## Easy-test package
- File: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v55.zip`
- SHA-256: `f11b74dc24b5cf8ef53c81a90fb7a01a031d3db4abe7c0ce86e4e47c95a7c946`

## Source checkpoint
- File: `JSRF-Native-Source-Checkpoint-08-FIXED-v55.zip`
- SHA-256: `f11b74dc24b5cf8ef53c81a90fb7a01a031d3db4abe7c0ce86e4e47c95a7c946`
- Byte-identical to the easy-test package.

## v54 -> v55 patch
- File: `JSRF-V54-to-V55.patch`
- SHA-256: `699df10a0483d0ebf5b79ea477a546c71aab6ab63c8ebd84b1388f97704f57fd`
- GitHub transport: `V54-to-V55.patch.gz.b64`
- Transport SHA-256: `d8a4dd2dcbb4dc0149d0d418ee7f74014e366d0250601df8952e655b800b57d2`
- Patch was applied to a clean v54 tree and the result compared byte-for-byte with the clean v55 tree.

## Final package verification
The actual easy-test ZIP was integrity-checked, extracted to a fresh directory, and verified from that extracted copy:
- root `START JSRF TEST.bat`: present;
- Python compile check: passed;
- Python suite: 193 passed;
- fresh CMake/Ninja configure/build: passed;
- native CTest: 20/20 passed;
- no retail `default.xbe`, pytest cache, or local verification build is packaged.

Windows runtime diagnosis remains pending until the v55 package is run and `[DISCDIAG]` is returned.
