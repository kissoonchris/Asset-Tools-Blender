# JSRF Native v54 artifacts

## Easy-test package

- File: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v54.zip`
- SHA-256: `aa8f4a1a138e6d23ef4dbf1d8abc2c5e0fd82a68d00e0249c560c4bd0d6a1dcd`

## Source checkpoint

- File: `JSRF-Native-Source-Checkpoint-08-HOTFIX53.zip`
- SHA-256: `aa8f4a1a138e6d23ef4dbf1d8abc2c5e0fd82a68d00e0249c560c4bd0d6a1dcd`
- Byte-identical to the easy-test package.

## v53 -> v54 patch

- File: `JSRF-V53-to-V54.patch`
- SHA-256: `e0d72960da65ca318985de527e600f200ce71fb7b768e5d19e570a5b6f210e92`
- GitHub transport: `V53-to-V54.patch.gz.b64`
- Transport SHA-256: `2489049adfeec9ab29087e1933b6e162fb525715242a813ac641b092d9ddc686`
- Patch was verified by applying it to the latest packaged v53 source and comparing the result byte-for-byte with the cleaned v54 source tree.

## Documentation

- `MANIFEST.md`: `89eeb442e075b3f91a533eec53c0876c010b786967933ce804a26102764134b4`
- `V54-VALIDATION.txt`: `a6570fe5e7c6d4ab9033661f000b26387ca25e12efe10c0b8e44b05c414b6920`
- `NV2A_ALPHA_BLEND_COMPOSITING.md`: `bd2ba25fd23747b4b710982a366a95b6bd5b663f18b5d68391f4133d319d7800`

## Final package verification

The actual easy-test ZIP was integrity-checked, extracted to a clean directory, and verified from that extracted copy:

- root `START JSRF TEST.bat`: present;
- Python suite: 187 passed;
- Python compile check: passed;
- fresh CMake/Ninja configure/build: passed;
- native CTest: 20/20 passed;
- no retail `default.xbe` payload or local pytest/build cache is packaged.

Windows runtime behavior remains unverified until the v54 easy-test is run with the user's retail JSRF files.
