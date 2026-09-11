# JSRF Native v64 checkpoint

Base: cleaned JSRF Native v63 project.

Purpose: read-only CRI/WX ADXT request-return diagnostic. v63 proves the host can read `title.adx` while the compressed source immediately cancels its ring reservation. v64 brackets the accepted request count at retail WX `0x001403B0`, CRI wrapper `0x0013DD70`, and source updater `0x0013C070` so the next Windows run can identify the first layer where the positive result is lost without forcing guest state.

Artifacts:
- `JSRF-Native-v64-CLEAN-PROJECT.zip` SHA-256 `9186d7bce2f75917990e75b9cc142888b9c6e0a85c85e357ff7f8c6a2decafb9`
- raw reconstruction patch SHA-256 `09d1a0f76c1fd83a5ec802f15d1ac8743fabb1e6ede79d20e8762655de279262`
- deterministic gzip patch SHA-256 `8d13c5ee18c5efaa57601a6723c7e02083f7922e53304aae9bb0bc54d1ff98d7`
- `.gz.b64` transport SHA-256 `dfa3f295dae2e18f3b034cb243f2e7ad5258792becb4d62cefd54f548c93784a`

Fresh packaged verification before backup:
- ZIP integrity passed; 193 files; one-click launcher present.
- 241 Python tests passed from the re-extracted package.
- `scripts/jsrf_xboxrecomp.py` py_compile passed.
- `recomp/src/recomp_manual.c` C11 syntax check passed.
- native Release build passed.
- CTest 20/20 passed.
- applying the raw patch to the clean v63 base reconstructs the v64 source tree exactly.

The active distributable stays clean: no retail `default.xbe`, no `game/` or `isoextract/` payload, and no build/cache directories.
