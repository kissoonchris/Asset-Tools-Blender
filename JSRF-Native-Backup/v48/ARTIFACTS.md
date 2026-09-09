# JSRF v48 artifacts

## Library artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v48.zip`
  - SHA-256: `93ec564dcd94c407246c8a25c4bf24ad3cd50ce82e7290868cab78f0079f8900`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX47.zip`
  - SHA-256: `93ec564dcd94c407246c8a25c4bf24ad3cd50ce82e7290868cab78f0079f8900`
- `JSRF-V47-to-V48.patch`
  - SHA-256: `4568808d4b166934b2c69f1cbf9318807dc8f3867a50cea55aa2c939e35a6a2f`
- `JSRF-V48-MANIFEST.md`
- `JSRF_V48_FADE_SERVICE_DIAGNOSTICS.md`

## Verification snapshot

Fresh extraction of the final easy-test ZIP passed:

- Python: **155 passed**
- Python `py_compile`: passed
- `recomp_manual.c` syntax check: passed
- fresh CMake/Ninja native build: passed
- native CTest: **20/20 passed**
- ZIP integrity: passed
- root `START JSRF TEST.bat`: present
- retail-payload scan: clean

`V47-to-V48.patch.gz.b64` is the exact unified patch compressed with gzip and base64 encoded.
