# JSRF Native v51 artifacts

## Easy test package

- File: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v51.zip`
- SHA-256: `7bda272e9cf842aeefe0a108b45b03ca0b3b18fe85212d3e3ef89cd45eec8e9b`

## Source checkpoint

- File: `JSRF-Native-Source-Checkpoint-08-HOTFIX50.zip`
- SHA-256: `7bda272e9cf842aeefe0a108b45b03ca0b3b18fe85212d3e3ef89cd45eec8e9b`
- Byte-identical to the easy-test package.

## v50 -> v51 patch

- File: `JSRF-V50-to-V51.patch`
- SHA-256: `3ecb3c8a3203a4ac9d3e248347155e6e8c4524483c7156af48e34c08a94eb132`
- GitHub transport: `JSRF-V50-to-V51.patch.gz.b64`
- Transport SHA-256: `106f28b5603b5542b7123372d9fc95fce06f6dc4813151f196106f4d2bf243e4`

## Documentation

- `JSRF-V51-MANIFEST.md` SHA-256: `c268d8cf3c0b4543fc30ede2e92823f37d21a92dc57c7d7497d0954d9f4e49b1`
- `JSRF_V51_PACKED_16BIT_CLEAR_COLOR.md` SHA-256: `d6afbf7f11a63f931ee5e085f3697da81bee4013e9005b0b458ceb787d6a8381`

## Fresh final-package verification

The actual final easy-test ZIP was extracted into a clean directory and checked after packaging:

- ZIP integrity: passed
- root `START JSRF TEST.bat`: present
- retail XBE/ISO payload scan: clean
- runtime banner: `JSRF Native v51`
- packed-16-bit clear patch pipeline hook: present
- Python regression suite: **168 passed**
- Python compile check: passed
- C syntax check for `recomp_manual.c`: passed
- fresh CMake/Ninja configure/build: passed
- native CTest: **20/20 passed**

Windows runtime behavior still requires the user's v51 test. In particular, this package does not claim that the separately reported presentation flicker is fixed.
