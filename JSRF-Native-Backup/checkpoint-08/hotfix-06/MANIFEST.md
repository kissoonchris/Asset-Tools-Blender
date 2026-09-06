# JSRF Native Checkpoint 08 — Hotfix 06

Local source commit: `0a9de9c40cc8771e7deeff8f88408d4d2f8807a3`

This hotfix is based on the exact retail JSRF XBE (SHA-256 `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`) and xboxrecomp revision `4d337526dc4ab892483aad0246aee9d1c33d1b81`.

Runtime evidence showed `0x0017D15C` was being reached as an indirect target and had been incorrectly seeded as a function. Retail disassembly proves it is a switch-case label inside the CRT `memmove` body beginning at `0x0017CEC0`; the reverse-copy path uses a negative-index embedded jump table. Hotfix 06 removes that internal label from the seed set and pins `0x0017CEC0` to an exact host `memmove` manual replacement while preserving the retail cdecl stack contract.

Verification before packaging:
- JSRF/xboxrecomp Python integration tests: 10/10 passed.
- Easy-test launcher tests: 4/4 passed.
- Full CMake/CTest suite: 20/20 passed.
- v7 archive static verification passed and ZIP integrity check reported no errors.

Artifacts:
- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v7.zip` SHA-256 `1328954de29c8b57e02e92f342e37fdc91b976dfd02d0c3afe7a631f8121cc78`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX6.zip` SHA-256 `1328954de29c8b57e02e92f342e37fdc91b976dfd02d0c3afe7a631f8121cc78`
- `JSRF-Native-Checkpoint-08-HOTFIX6.git.bundle` SHA-256 `0108d81c3950b7bccfafb70553e1d137bede6c4d2e5bdbc56d87caa2da8ec9c8`
- `JSRF-Checkpoint-08-HOTFIX6.patch` SHA-256 `55e84278976f938047d76d5fad7e41abfb6dc47d64f07e04ff2039dca93c7f85`
