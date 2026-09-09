# JSRF v47 artifacts

## Library artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v47.zip`
  - SHA-256: `90c3d274b56b11947659bce09b3078d9ccf8e2e48343105362d6772446df9248`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX46.zip`
  - SHA-256: `90c3d274b56b11947659bce09b3078d9ccf8e2e48343105362d6772446df9248`
- `JSRF-V46-to-V47.patch`
  - SHA-256: `24307b5e46d7136320a4ad2b93f4915ff8e1d10eb53544268f39c30a3fcfeace`
- `JSRF-V47-MANIFEST.md`
- `JSRF_V47_POST_CACHE_TITLE_STATE_DIAGNOSTICS.md`

These are backed up in ChatGPT Library under `/JSRF/`.

## GitHub patch backup

`V46-to-V47.patch.gz.b64` is the exact v46-to-v47 unified patch compressed with gzip and encoded as base64.

Reconstruct with:

```sh
base64 -d V46-to-V47.patch.gz.b64 | gzip -d > JSRF-V46-to-V47.patch
```

The reconstructed patch SHA-256 is:

`24307b5e46d7136320a4ad2b93f4915ff8e1d10eb53544268f39c30a3fcfeace`

## Verification snapshot

- final ZIP focused v47 regression: **5/5 passed**
- final ZIP full Python suite: **151 passed**
- Python `py_compile`: **passed**
- `recomp_manual.c` C syntax: **passed**
- fresh native CMake/Ninja build: **passed**
- native CTest: **20/20 passed**
- ZIP integrity: **passed**
- root `START JSRF TEST.bat`: **present**
- retail payload scan: **clean**

Windows runtime confirmation of the new diagnostic observations remains pending.