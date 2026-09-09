# JSRF v46 artifacts

## Library artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v46.zip`
  - SHA-256: `72e3a8f3427dc93a7fd493b857e0ea89e2bd4fe3592958748e6608ae1a0e1a8f`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX45.zip`
  - SHA-256: `72e3a8f3427dc93a7fd493b857e0ea89e2bd4fe3592958748e6608ae1a0e1a8f`
- `JSRF-V45-to-V46.patch`
  - SHA-256: `f1da989552bf76bb0996b3894f682a4bda38ce08fb8977320c8d3582042fd3c6`
- `JSRF-V46-MANIFEST.md`
- `JSRF_V46_NTFREE_GUEST_HEAP_RELEASE.md`

These are backed up in ChatGPT Library under `/JSRF/`.

## GitHub patch backup

`V45-to-V46.patch.gz.b64` is the exact v45-to-v46 unified patch compressed with gzip and encoded as base64.

Reconstruct with:

```sh
base64 -d V45-to-V46.patch.gz.b64 | gzip -d > JSRF-V45-to-V46.patch
```

The reconstructed patch SHA-256 is:

`f1da989552bf76bb0996b3894f682a4bda38ce08fb8977320c8d3582042fd3c6`

## Verification snapshot

- final ZIP focused v46 regression: **5/5 passed**
- final ZIP full Python suite: **146 passed**
- Python `py_compile`: **passed**
- `recomp_manual.c` C syntax: **passed**
- fresh native CMake/Ninja build: **passed**
- native CTest: **20/20 passed**
- ZIP integrity: **passed**
- root `START JSRF TEST.bat`: **present**
- retail payload scan: **clean**

Windows runtime confirmation remains pending.
