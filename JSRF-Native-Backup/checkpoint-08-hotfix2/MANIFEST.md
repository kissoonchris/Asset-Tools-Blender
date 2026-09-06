# JSRF Checkpoint 08 hotfix 2 — runtime launch capture

Local commit: `0b4ec3ead6cb1a8c873baa7aeadf6d3b471e495d`

Root cause from the user's second Windows test log:
- the native JSRF host built successfully,
- but the CMake target was a `WIN32` GUI subsystem executable,
- so the beginner PowerShell launcher could report success without reliably waiting for or capturing the native runtime's stdout/stderr.

Fix:
- build `jsrf_recomp.exe` as a console-subsystem executable during bring-up,
- emit explicit `Launching native JSRF runtime...` and runtime exit-code markers,
- add regression tests preventing the GUI-subsystem launcher behavior from returning.

Verification:
- Python integration tests: 8/8 passed
- CTest: 20/20 passed
- zip integrity verified

No gameplay behavior or retail lifted code was replaced.
