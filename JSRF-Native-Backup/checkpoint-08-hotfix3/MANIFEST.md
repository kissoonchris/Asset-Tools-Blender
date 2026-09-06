# JSRF Checkpoint 08 Hotfix 3

Local source commit: `9594e462cd1b6da665b95daaa8e49ce9dd41a586`

Observed Windows result from user test:
- native host compiled successfully
- process exited with code `3`
- xboxrecomp source confirms `_exit(3)` is its watchdog diagnostic exit, not the host's normal return path
- uploaded transcript only retained the tail of the guest-stack dump, so it did not preserve the watchdog header/recent icall targets needed to diagnose the exact spin

Hotfix 3 changes:
- captures the native runtime output to `JSRF_RUNTIME_LOG.txt`
- automatically runs xboxrecomp `tools/stackwalk.py` against `tools/disasm/output/functions.json`
- saves decoded stack to `JSRF_STACKWALK_LOG.txt`
- appends both diagnostics into the single user-facing `JSRF_TEST_LOG.txt`
- keeps `START JSRF TEST.bat` as the one-click entry point
- labels exit code 3 as the xboxrecomp watchdog diagnostic exit

Verification:
- PowerShell regression tests: 3/3 passed
- JSRF xboxrecomp integration tests: 7/7 passed
- CTest suite: 20/20 passed
- git diff --check clean
- Git bundle verification: complete history, OK

Persistent Library artifacts:
- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v4.zip` SHA256 `ab762d4a6134e808277570a95cb051abd14309f4fc130f1ddf2447e0ea5c2089`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX3.zip` SHA256 `8e0d0532d22510db7b12051802cfb5af5eb3db0bbd01f45ac5b0818144f07fbd`
- `JSRF-Native-Checkpoint-08-HOTFIX3.git.bundle` SHA256 `d8fd5e2fb0774154c7567d800bdae0c4bebd3d933113c9a00ccd2832e31b94df`
- `JSRF-Checkpoint-08-HOTFIX3.patch` SHA256 `cf3516a3d6ecb566b7021501fcdb33d445efa96df9f086d769ddd7bea427fb22`

Next action: run the v4 easy-test package once and return only `JSRF_TEST_LOG.txt`; it should now contain the complete watchdog output plus decoded guest stack so the exact runtime blocker can be fixed instead of guessed.