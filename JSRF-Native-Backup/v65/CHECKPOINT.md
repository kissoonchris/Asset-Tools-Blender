# JSRF Native v65 checkpoint

Base: cleaned JSRF Native v64 project.

Purpose: correct the v64 read-only ADXT request-return diagnostic so it matches xboxrecomp's real one-line generated RET form. The v64 Windows run reaches both request entry probes but records no phase-1 returns because the injector only matched standalone `return;` lines; v65 also matches generated `...; return; /* ret */` lines without changing guest behavior.

Artifacts:
- `JSRF-Native-v65-CLEAN-PROJECT.zip` SHA-256 `aa316487a030aa3e151aedc5ee7635261b4319baea13af38a1c53ba24cf3419c`
- raw reconstruction patch SHA-256 `8740ed45ec572a6c3b896538af73dd747029603166deefc377ae6396d7beb40f`
- deterministic gzip patch SHA-256 `6d18b7f7d604264038eb1079251bae588aa0ded67ba272c65c5999483caf3e97`
- `.gz.b64` transport SHA-256 `b80cc5d9618b0a8d751ecffecb9c4b56c54d7d250841e381caa85bc439f4babc`

Fresh packaged verification before backup:
- ZIP integrity passed; 196 files; one-click launcher present.
- 245 Python tests passed from the re-extracted package.
- `scripts/jsrf_xboxrecomp.py` py_compile passed.
- `recomp/src/recomp_manual.c` C11 syntax check passed.
- native Release build passed.
- CTest 20/20 passed.
- applying the raw patch to the clean v64 base reconstructs the packaged v65 source tree exactly.

The active distributable stays clean: no retail `default.xbe`, no `game/` or `isoextract/` payload, and no build/cache directories.
