# JSRF v45 artifacts

## Library artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v45.zip`
  - SHA-256: `7f46d4f33b17b30b743dd74a223435b94762960887f5ccfefc0e48b20123d0c7`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX44.zip`
  - SHA-256: `7f46d4f33b17b30b743dd74a223435b94762960887f5ccfefc0e48b20123d0c7`
- `JSRF-V44-to-V45.patch`
  - SHA-256: `cd49a28a8033542f5fdffc1f38d2770c6c7f3cb3fbd7863f2ad6ef106e06617c`
- `JSRF-V45-MANIFEST.md`
- `JSRF_V45_NTQUERYDIRECTORYFILE_ABI.md`

These are backed up in ChatGPT Library under `/JSRF/`.

## GitHub patch backup

`V44-to-V45.patch.gz.b64` is the exact v44-to-v45 patch compressed with gzip and encoded as base64 so it can be stored as UTF-8 text.

Reconstruct with:

```sh
base64 -d V44-to-V45.patch.gz.b64 | gzip -d > JSRF-V44-to-V45.patch
```

The reconstructed patch SHA-256 is:

`cd49a28a8033542f5fdffc1f38d2770c6c7f3cb3fbd7863f2ad6ef106e06617c`

## Verification snapshot

- final packaged tree full Python suite: **141 passed**
- focused v44/v45 regression: **9 passed**
- fresh native build: **passed**
- native CTest: **20/20 passed**
- ZIP integrity: **passed**
- root `START JSRF TEST.bat`: **present**
- retail payload scan: **clean**

Windows runtime confirmation remains pending.
