# JSRF Native v52 artifacts

## Easy test

`JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v52.zip`

- size: 437,755 bytes
- SHA-256: `5c084e1072016362a0c3831ba5963cf8b2b6408a3b94b5925a141fe01cba12ee`
- ZIP entries: 208
- root launcher: `START JSRF TEST.bat`

## Source checkpoint

`JSRF-Native-Source-Checkpoint-08-HOTFIX51.zip`

- size: 437,755 bytes
- SHA-256: `5c084e1072016362a0c3831ba5963cf8b2b6408a3b94b5925a141fe01cba12ee`
- byte-identical to the easy-test archive

## Delta

`JSRF-V51-to-V52.patch`

- size: 16,814 bytes
- SHA-256: `40ce5d4d8c3ac0a5da1e48af8d018ed5f5c0f3846afae026aae2461047390565`
- patch-apply verification from the final v51 source checkpoint passed

GitHub transport copy:

`JSRF-V51-to-V52.patch.gz.b64`

- size: 7,125 bytes
- SHA-256: `3ba0dc0d15a0c0dd09c6593f68c4a0e11ccb1f57ed27c80305d1ab91bba6b9f9`

## Verification

Fresh extraction of the actual final v52 ZIP:

- focused v52 regression: 6/6 passed
- full Python suite: 174 passed
- Python `py_compile`: passed
- `gcc -std=c11 -fsyntax-only recomp/src/recomp_manual.c`: passed
- fresh CMake/Ninja configure/build: passed
- native CTest: 20/20 passed
- ZIP integrity: passed
- root launcher present
- retail `.xbe` / Media payload name scan: clean

This verification proves the package/build contracts, not that the Windows
retail runtime has reached the main menu. That remains for the next user run.
