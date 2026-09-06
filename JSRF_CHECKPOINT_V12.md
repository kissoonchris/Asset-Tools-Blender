# JSRF native checkpoint v12

Local source commit: `8ec463c4b610f427a2cb9b1d0d24aae07691bc01` (`Pin JSRF DirectXFile retail stack contract`).

The v11 Windows run reproduced the same failure as v10: ABI diagnostics still reported `0x00176630` returning with corrupted ESI/EDI/ESP immediately before the later `0xFFFFFFFC` access violation in `0x0019E4DC`, after roughly 3,001 indirect calls. That disproved the seed-only correction: adding `0x00178A00` to the generic seed list did not force downstream code generation to preserve it as a strong standalone function.

Retail analysis proves `0x00176630` dispatches through vtable `0x001E4020` slot `+0x0C` to `0x00178A00`. The caller pushes six 32-bit arguments, and every retail exit from `0x00178A00` uses `ret 0x18`. v12 therefore pins the exact retail function interval `0x00178A00..0x00178B2B` directly into the disassembler function database, records the factual caller `0x00176630` so xboxrecomp treats it as a strong entry, removes false starts inside that interval, and clamps any predecessor that overlapped it.

v12 also audits generated C before compilation: `sub_00178A00` must exist and every lifted return in that body must be `esp += 28; return; /* ret 24 */` (4-byte return address + 24 bytes of callee-cleaned arguments). The build now aborts before runtime if that retail stack contract is lost again.

Artifacts persisted in ChatGPT Library:
- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v12.zip` (`libfile_60a4702120808191bdc6a24cb36fddfd`), SHA-256 `52bbe7317977cfec8ad69867d20ef498946ab313dd91166eaab87b41c08b3276`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX11.zip` (`libfile_a0aac271f4048191ae6a1dfeec26cf6a`), SHA-256 `52bbe7317977cfec8ad69867d20ef498946ab313dd91166eaab87b41c08b3276`
- `/JSRF/JSRF-Native-Checkpoint-08-HOTFIX11.git.bundle` (`libfile_fa18e6e9ba1c8191aa952480419052f3`), SHA-256 `59f0e91c261c6a4b5fbde8a278513c666da75f5380ca668e6903a7878954ebd9`
- `/JSRF/JSRF-Checkpoint-08-HOTFIX11.patch` (`libfile_6f5da65452788191b842253d6c04961b`), SHA-256 `66a6d402e18fd1e5150057d21a1b238e163736743908edba67b1e9af458078a7`

Fresh verification before packaging: 18/18 Python diagnostic/launcher/integration tests, 20/20 CTest tests, ZIP integrity passed, git bundle verified, and working tree clean.
