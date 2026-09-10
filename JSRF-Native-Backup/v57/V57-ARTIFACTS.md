# JSRF Native v57 artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v57.zip`
  - SHA-256 `b96959f5313a164d05994715c10397980f4f597709988fcdf6fa8156817a9b50`
- `JSRF-Native-Source-Checkpoint-08-FIXED-v57.zip`
  - SHA-256 `b96959f5313a164d05994715c10397980f4f597709988fcdf6fa8156817a9b50`
- `JSRF-V56-to-V57.patch`
  - SHA-256 `a1e1e3cbb45cab681761a5f00b34882a3dce3352bd2aed6159a998ac142c0590`

Fresh verification of the final extracted easy-test archive: 200 Python tests passed, Python compile passed, fresh native CMake/Ninja build passed, native CTest 20/20 passed, ZIP integrity passed, `START JSRF TEST.bat` is present, the permanent `0x0013D840` seed is present, the v57 trace-budget default is present, and no retail `default.xbe` is included. The v56-to-v57 patch also applies cleanly to a clean v56 tree and reproduces the final non-transient v57 source tree byte-for-byte.
