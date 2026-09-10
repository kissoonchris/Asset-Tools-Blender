# JSRF Native v54 artifacts

## Corrected easy-test package

- File: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v54.zip`
- SHA-256: `ef33770bc444c2742a28654ca9810fc89b869c20fd26487736861d371ca916c7`
- This supersedes the first v54 ZIP, whose Windows installer stopped before generation because its patch anchor expected a comment-free `NV097_SET_VERTEX_DATA_ARRAY_OFFSET` define.

## Corrected source checkpoint

- File: `JSRF-Native-Source-Checkpoint-08-FIXED-v54.zip`
- SHA-256: `ef33770bc444c2742a28654ca9810fc89b869c20fd26487736861d371ca916c7`
- Byte-identical to the corrected easy-test package.

## v53 -> v54 patch

- Corrected local patch file: `JSRF-V53-to-V54.patch`
- Corrected patch SHA-256: `f64a982765225b01cb080b6a8d270ed5bf9496730de0cc34ce5c78b7d98e89e7`
- Existing GitHub transport `V53-to-V54.patch.gz.b64` records the initial v54 change set.
- Apply `V54-WINDOWS-PATCH-ANCHOR-HOTFIX.patch` after that transport to recover the corrected Windows patch anchor.
- Hotfix SHA-256: `b9b2d23e7652ea8f85fcf66db3592432c332c9c9191a26d4fa19755636ac5faf`

## Documentation

- Corrected `MANIFEST.md`: `2c769971889b848b2166ff8e35b351647829b1b4cd50e5e97990d5da3a9e449f`
- Corrected `V54-VALIDATION.txt`: `b04f180a5832473e2c7b0e6ee8e2212c357797563bb83ab98c1e9c2f313e8a48`
- `NV2A_ALPHA_BLEND_COMPOSITING.md` remains unchanged.

## Final package verification

The corrected easy-test ZIP was integrity-checked, extracted to a clean directory, and verified from that extracted copy:

- root `START JSRF TEST.bat`: present;
- Python suite: 188 passed;
- Python compile check: passed;
- fresh CMake configure/build: passed;
- native CTest: 20/20 passed;
- regression for the exact pinned xboxrecomp inline-comment constant layout: passed;
- no retail `default.xbe` payload or local pytest/build cache is packaged.

Windows runtime behavior remains unverified until the corrected v54 easy-test is run with the user's retail JSRF files.
