# JSRF Native v49 artifacts

- Easy-test ZIP: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v49.zip`
  - SHA-256: `0b842a3784415498faf6020af42d2f963fb97dc3b46d3c88b2206839e5e773ab`
  - size: 413031 bytes
- Source checkpoint: `JSRF-Native-Source-Checkpoint-08-HOTFIX48.zip`
  - SHA-256: `0b842a3784415498faf6020af42d2f963fb97dc3b46d3c88b2206839e5e773ab`
  - byte-identical to the easy-test ZIP
- v48 -> v49 patch: `JSRF-V48-to-V49.patch`
  - SHA-256: `89a48b4634bbc167aea76f7daa4cd8289eaa98b00d832a4bd4704a03ec05556b`
  - size: 19801 bytes
- compressed/base64 patch: `JSRF-V48-to-V49.patch.gz.b64`
  - SHA-256: `1874a6b6cb9dc600f5dc0d39f740136d3970bfc349f33e4df550d97c86fdc334`
  - size: 8937 bytes

Final package verification was performed from a fresh extraction of the exact easy-test ZIP:

- focused v49 Python: 4/4 passed;
- full Python: 159 passed;
- Python compile: passed;
- `recomp_manual.c` syntax: passed;
- fresh CMake/Ninja configure/build: passed;
- CTest: 20/20 passed;
- root `START JSRF TEST.bat`: present;
- runtime banner: `JSRF Native v49`;
- no retail `default.xbe`, `game/`, or `isoextract/` payload.
