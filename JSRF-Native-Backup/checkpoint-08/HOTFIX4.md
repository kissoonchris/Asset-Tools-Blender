# JSRF Checkpoint 08 Hotfix 4

- Local source commit: `d4c01bc7ec75b6ba2fdd7ee83b3724efbde02d1c`
- Purpose: prevent Windows PowerShell 5 `$ErrorActionPreference = Stop` from aborting native runtime capture when `jsrf_recomp.exe` writes normal diagnostics to stderr.
- Root cause observed on 2026-09-06: runtime reached `xbox_MemoryLayoutInit`, then PowerShell converted stderr into `NativeCommandError` before the actual JSRF runtime failure/watchdog could be captured.
- Fix: temporarily set `$ErrorActionPreference = "Continue"` only around the native `jsrf_recomp.exe 2>&1 | Tee-Object` invocation, preserve `$LASTEXITCODE`, then restore the previous preference.
- Added regression test: `test_native_stderr_cannot_trip_stop_mode_before_runtime_log_is_captured`.
- Verification: `tests/test_easy_test_powershell.py` 4/4 passed; `tests/test_jsrf_xboxrecomp.py` 7/7 passed; CTest 20/20 passed; `git diff --check` clean.

Persistent Library artifacts:
- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v5.zip` SHA256 `f0db7d03e4a72566a13d1a063ac13f7cb330a6c4fc6054ce1f94936ef4dc6430`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX4.zip` SHA256 `f0db7d03e4a72566a13d1a063ac13f7cb330a6c4fc6054ce1f94936ef4dc6430`
- `/JSRF/JSRF-Native-Checkpoint-08-HOTFIX4.git.bundle` SHA256 `e0105f0492631ddfc1ddd750a4554a9118b7bbcaf11e20087210d21124834e7f`
- `/JSRF/JSRF-Checkpoint-08-HOTFIX4.patch` SHA256 `04c0e0564a50181aadc3c8d7f18a903c24c9329ab0182283fec809a195324d20`

The game has not booted to a visible frame yet. The next v5 run is intended to capture the actual native runtime/watchdog output past memory mapping.