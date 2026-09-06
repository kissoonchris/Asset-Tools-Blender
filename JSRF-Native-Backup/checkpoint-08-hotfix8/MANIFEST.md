# JSRF native checkpoint 08 hotfix 8

Local source commit: `daab46cd2e95667cba041abe469c805dd9359af7`

Fix: the v8 diagnostic build enabled `RECOMP_ABI_CHECK`, so generated objects referenced `recomp_abi_violation_log`, but the JSRF host link did not provide that symbol. Hotfix 8 defines the callback in `recomp/src/recomp_manual.c` and adds a regression test.

Verification:
- 16/16 xboxrecomp + launcher Python tests passed
- 20/20 CTest project tests passed
- Easy-test ZIP integrity verified

Artifacts:
- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v9.zip` SHA-256 `f5a55ccae3edad73a9ff646ca5a8c5cdea82f9b11be73ae7bf2ad9ef573f7673`
- `JSRF-Native-Checkpoint-08-HOTFIX8.git.bundle` SHA-256 `5cf0a4b0c92ba3e0d9d3f930f4efe74497ee2d5f4f869344f7075f91afcb579c`
- `JSRF-Checkpoint-08-HOTFIX8.patch` SHA-256 `5815ea5af39567f3fc982bef891d6eb6d5daf4a14d9179d8e53e47b626bd492b`
