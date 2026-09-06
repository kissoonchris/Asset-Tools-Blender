# JSRF Checkpoint 08 — Hotfix 5

Date: 2026-09-06

Local source commit: `e200660f9653bf270a30bd62bbb87d4a5dcb069c`

## First full native runtime trace

The Windows test now boots the retail JSRF XBE into the xboxrecomp runtime, completes memory layout, resolves the 120-entry kernel thunk table, enters the title's first system thread, performs file-system and allocator initialization, then reaches the watchdog after hundreds of indirect calls.

The trace exposed a concrete generation-coverage problem: the JSRF pipeline was using `tools.disasm --text-only`, while JSRF executes code from XDK/library sections outside `.text` and also reaches stripped/computed indirect targets not discovered statically.

## Hotfix 5

- Generation now includes JSRF code-bearing XDK sections: `D3D,DSOUND,MMATRIX,XGRPH,XPP,DOLBY`.
- Added `recomp/jsrf_runtime_seeds.json` with the 15 measured unresolved indirect-call targets from the first full Windows trace.
- The disassembler receives that file through `--seed-functions`.
- No speculative gameplay replacement or manual behavior stub was added.

Measured runtime seeds:
`0x0017D15C`, `0x0018AFB0`, `0x0018B1A0`, `0x0018B390`, `0x0018B580`, `0x0018B770`, `0x0018B960`, `0x0018BB50`, `0x0018BD40`, `0x0018C1D0`, `0x0018C3C0`, `0x001BDAA9`, `0x0018E410`, `0x001A5299`, `0x001A52A4`.

## Verification

- `tests.test_jsrf_xboxrecomp`: 8/8 passed.
- PowerShell launcher tests: 4/4 passed.
- CTest suite: 20/20 passed.
- `git diff --check`: clean.
- Git bundle verified as complete history.

## Persistent artifacts

Stored in ChatGPT Files Library under `/JSRF/`:

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v6.zip`
  - SHA-256: `6125f47835bf5f7c840da8fb245d7913dbd74ca65b1ec8d6de56edfc74149d38`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX5.zip`
  - SHA-256: `6125f47835bf5f7c840da8fb245d7913dbd74ca65b1ec8d6de56edfc74149d38`
- `JSRF-Native-Checkpoint-08-HOTFIX5.git.bundle`
  - SHA-256: `d637f3f7db7f1ec40f749a28e37c58b23dc6cc76767e254a19706009e182a1cc`
- `JSRF-Checkpoint-08-HOTFIX5.patch`
  - SHA-256: `2c186b4a00afaad1d435d161aa4ed8ea31ba688406e7e48d383c259b3c5141b5`

This checkpoint does **not** claim graphics/title-screen boot yet. The next Windows run is intended to test the newly lifted XDK sections and measured runtime targets.