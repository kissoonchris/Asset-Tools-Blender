# Active project cleanup

The active v62 working project was cleaned after validation so new checkpoints do not keep accumulating old per-version artifacts.

Removed from active distributables:
- root-level `JSRF-Vxx-MANIFEST.md` files from old checkpoints;
- root-level `Vxx-VALIDATION.txt` files from old checkpoints;
- `ARTIFACTS-V60.md`;
- generated `build_verify_v60/`, `build_verify_v61/`, and `build_verify_v62/` trees;
- `.pytest_cache/` and Python `__pycache__` / `.pyc` files;
- the stale hand-written prototype note/preview/prebuilt Linux prototype and old `Build-And-Run-Windows.bat` launcher.

Current checkpoint metadata now uses stable names:
- `VERSION.md`
- `docs/VALIDATION.md`
- `docs/PROJECT_LAYOUT.md`
- `docs/HISTORY.md`

The `research/` notes and regression tests remain because they are still useful evidence and protect earlier fixes. Historical manifests/validation remain recoverable from repository history and prior backup branches instead of being recopied into every new package.

Post-cleanup source tree: 188 files, about 1.4 MB before packaging. Fresh verification after cleanup: 233 Python tests passed, `scripts/jsrf_xboxrecomp.py` compiled, native Release build succeeded, and CTest passed 20/20.

Clean package SHA256: `09c38d9b376ebe952150953546afb3e4b70004b7a7fb997ca7d76a4ac57276cc` (`JSRF-Native-v62-CLEAN-PROJECT.zip`).
