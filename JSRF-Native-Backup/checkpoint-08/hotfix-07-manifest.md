# JSRF native checkpoint 08 — hotfix 07

Local source commit: `8e2bf1112188475f49b98abea64bbd3960b3306b`

Purpose: instrument the v7 access violation without adding speculative gameplay/runtime behavior.

Changes:
- bad indirect-call logging now records the guest return address and register state and keys rate limiting by `(target, return address)`;
- recent ICALL targets are dumped at each unique bad call site;
- MSVC Release builds keep `/Zi` + `/DEBUG` symbols;
- `RECOMP_ABI_CHECK` is enabled for indirect-call ABI diagnostics;
- VEH crash output reports module base, RIP RVA, symbol/source line when DbgHelp resolves them;
- regression tests cover the diagnostic contract.

Artifacts persisted in ChatGPT Library:
- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v8.zip` — SHA-256 `29a7f28d8298d1bc50b52c99ced81a72bd30444b7da3c7d0c06aab3f66eb303f` — `libfile_9970bef0e87c8191ad5dc5e6e7bed08d`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX7.zip` — SHA-256 `29a7f28d8298d1bc50b52c99ced81a72bd30444b7da3c7d0c06aab3f66eb303f` — `libfile_0ebf2de97bf48191974611888b5c5070`
- `/JSRF/JSRF-Native-Checkpoint-08-HOTFIX7.git.bundle` — SHA-256 `2d1ec7959e7323cbc9d352ba97bc545793ea66732e2e912569a3384f45619b73` — `libfile_30bc8a56d3748191b4d81533cd65ab9d`
- `/JSRF/JSRF-Checkpoint-08-HOTFIX7.patch` — SHA-256 `6743ed3fe4946ef331fba650b6879310483aaacad12c8f72c9f77523c88a97b0` — `libfile_cd4e475277d8819193fc1e02126a5982`

Fresh local verification before packaging:
- targeted Python tests: 16 passed;
- project CTest suite: 20/20 passed;
- ZIP integrity check: no errors;
- git bundle verification: complete history, OK.
