# JSRF Native v50 artifacts

## Packages

- Easy test: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v50.zip`
  - size: 429,508 bytes
  - SHA-256: `5b04ad039c192f7cb01c7aebbcd89a7d398df6edba84856d7e383fcea1e20804`
- Source checkpoint: `JSRF-Native-Source-Checkpoint-08-HOTFIX49.zip`
  - size: 429,508 bytes
  - SHA-256: `5b04ad039c192f7cb01c7aebbcd89a7d398df6edba84856d7e383fcea1e20804`
- Exact v49→v50 patch: `JSRF-V49-to-V50.patch`
  - size: 18,863 bytes
  - SHA-256: `2dfdbda8e5f0c7af0b30962dc0e47db7e5c52f7062590e14791c2c83e6b91cc5`
- GitHub-safe compressed/base64 patch: `JSRF-V49-to-V50.patch.gz.b64`
  - size: 8,213 bytes
  - SHA-256: `f9a26fa1a7705975c9a1def488a3b462bcd22ac5a69c1d75458654ad13965bc5`

## Documentation

- `JSRF-V50-MANIFEST.md`
  - SHA-256: `f5ad194f7aa5b9cfd316e48ed140cc92380639111f105b6bb8e63f5ae0d52c10`
- `JSRF_V50_7B8D0_SEH_VTABLE_BOUNDARY.md`
  - SHA-256: `cfbad80c28b51367739f0f86cbac96424af7849432589201de042dbd982c9cd2`

## Fresh final verification

The actual final easy-test ZIP was extracted into a new directory and checked:

- root `START JSRF TEST.bat`: present;
- no `default.xbe`, `game/`, or `isoextract/` retail payload: present;
- ZIP integrity: clean;
- Python suite: **164 passed**;
- Python `py_compile`: passed;
- fresh CMake/Ninja configure/build: passed;
- native CTest: **20/20 passed**;
- v49→v50 patch applied cleanly to a fresh v49 package;
- patched-tree focused v50 tests: **5/5 passed**;
- patched-tree full Python suite: **164 passed**.

Windows runtime confirmation is still required before claiming the retail title
transition reaches the main menu. The startup flicker is explicitly not claimed
fixed by v50.
