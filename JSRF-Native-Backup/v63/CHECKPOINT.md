# JSRF Native v63 checkpoint

Base: cleaned JSRF Native v62 project.

Purpose: read-only CRI ADXT compressed-source diagnostic at the existing ring-cancel probe. v62 proves the watchdog now reaches its legitimate 1201-count threshold; v63 records the source request return, WX file implementation state, async request fields, reserved span, and first 16 bytes of the source buffer without forcing guest state.

Artifacts:
- `JSRF-Native-v63-CLEAN-PROJECT.zip` SHA-256 `f3e4423fd89ab66bb4b4fc82cf66a7f59ed8ce585c460f511e0c3d016ab3f6db`
- raw reconstruction patch SHA-256 `91200b37764cf233acc031874540deff2c6a236b61387ad78ed5cf1937c6dec0`
- uploaded deterministic gzip patch SHA-256 `fa5d163cb236baeab0def6ab30b09db4eeeb172c1f3a89e6e4d2387507b845d3`
- uploaded `.gz.b64` SHA-256 `c9acafb7196074dbf1692d110903da6ba4c8c4299860f3be613a1f86779c8d23`

Fresh packaged verification before backup:
- ZIP integrity passed; 190 files; one-click launcher present.
- 237 Python tests passed.
- `scripts/jsrf_xboxrecomp.py` py_compile passed.
- `recomp/src/recomp_manual.c` C11 syntax check passed.
- native Release build passed.
- CTest 20/20 passed.
- applying the raw patch to the clean v62 base reconstructs the packaged v63 source tree exactly after excluding test-generated caches.

The active distributable stays clean: no root per-version manifests, no old build directories, no preview/prototype checkpoint artifacts, and no retail `default.xbe`.
