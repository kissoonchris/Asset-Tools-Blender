# JSRF Native checkpoint 08 — xboxrecomp integration

Local source commit: `984fcc99cb0c8b3976e5c260e7905f30b7695518` (`984fcc9`).

Retail ground truth:
- Title: Jet Set Radio Future
- Title ID: `0x5345000A`
- XBE base: `0x00010000`
- Entry point: `0x00148023`
- Kernel thunk VA: `0x001C3F60`
- Retail `default.xbe` SHA-256: `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`

xboxrecomp backbone pinned to upstream commit:
`4d337526dc4ab892483aad0246aee9d1c33d1b81`

Checkpoint 08 adds:
- Exact retail-XBE identity/section validation and xboxrecomp pipeline driver.
- One-click Windows bootstrap: `Build-JSRF-XboxRecomp.bat` / `scripts/Build-JSRF-XboxRecomp.ps1`.
- JSRF-specific xboxrecomp Windows host and CMake project.
- Generated-code target at `recomp/src/recomp/gen/`.
- ICALL/crash/watchdog diagnostics with no active gameplay overrides.
- Research metadata for the retail XBE and xboxrecomp bring-up order.
- Seven Python integration regression tests and a CTest integration hook.

Fresh local verification on the source represented by this checkpoint:
- Python xboxrecomp integration tests: 7/7 passed.
- Full CTest suite: 20/20 passed, 0 failed.
- `git diff --check`: clean.

No placeholder locomotion or invented gameplay is wired into the recomp path. The generated retail code is authoritative; manual overrides are reserved only for title-specific failures proven during bring-up.

Persistent artifacts are stored in the JSRF ChatGPT Files Library:
- `JSRF-Native-Source-Checkpoint-08.zip`
  - SHA-256: `c91b44d74ff5439593cc13dba171443cf37036bfb66652c2a9406372f71dfa62`
- `JSRF-Native-Checkpoint-08.git.bundle`
  - SHA-256: `0f3f0cc680d5adc8175e44032c4ea2fcb3c31a2a7b506d60171459969d807a8d`
  - Contains complete local Git history through `984fcc9`.
- `JSRF-Checkpoint-07-to-08.patch`
  - SHA-256: `e55f50fdc8131affc1a2c4c651ca9e28aa1e23e729e2df7249ff393d92a8bbaa`
- `JSRF-Checkpoint-06-to-08.patch`
  - SHA-256: `e5048ef9246872b86481e6a554b1f5f9d597550c7c000eb1d48f055db0633b1d`

The actual first Windows xboxrecomp lift/build/boot is not claimed here because this workspace cannot execute the required Windows/MSVC runtime. The included bootstrap performs that pipeline on the Windows development machine using the retail game files.
