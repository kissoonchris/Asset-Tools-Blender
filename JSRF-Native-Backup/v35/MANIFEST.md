# JSRF Native v35 manifest

## Purpose

v35 moves past the now-confirmed DirectSound DSP mailbox fix and addresses the next stable retail-code translation failure: corrupted callee-save/stack cleanup from low-.text CFG splitting.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v34 Windows evidence

v34 successfully acknowledged retail DirectSound command `3` at the corrected GP mailbox `0x803C0810`. The audio-hardware wait that blocked earlier checkpoints is therefore cleared.

The automatic callback pass then discovered and validated two real retail indirect targets, `0x00173DB0` and `0x00175300`. Once both were available, the last two passes converged on the same crash.

Immediately before that crash, the ABI checker reports:

- `0x00025040`: ESP `0x00F7FEE8 -> 0x00F7FEE0`
- `0x00011070`: ESI `0x01080FA0 -> 0`, ESP `0x00F7FEF0 -> 0x00F7FEE8`

The resulting bad object traversal later reaches `sub_00011E40` with `ECX/EDI = 0xD80C2444`, where reading object field `+4` faults.

## Root cause addressed by v35

Both ABI-violating methods have retail control-flow shapes that generic recovery can split incorrectly:

- `0x00011070..0x0001109F` is one recursive traversal with a single `pop esi; ret`; direct caller `0x000124BE` proves the entry.
- `0x00025040..0x000252DF` is one virtual state machine whose eight-way computed jump table is embedded at `0x000252BC..0x000252DB`. Its constructor-installed vtable slot at `0x001C4D5C` contains literal method pointer `0x00025040`. The next real function begins at `0x000252E0`.

v35 pins exact boundaries:

- `[0x00011070, 0x000110A0)`
- `[0x00025040, 0x000252E0)`

Internal false starts inside those ranges are removed before code generation.

## Callback seed consolidation

`recomp/jsrf_runtime_seeds.json` now permanently includes:

- `0x00173DB0`
- `0x00175300`

These are the two callbacks automatically promoted by the user's v34 run, so future tests begin with the already-proven callback set instead of relearning them.

## Generated-code safety check

`verify_jsrf_generated_contracts()` now also checks the two v35 methods after codegen. The build will stop before running if `sub_00011070` does not retain its one plain-ret exit or `sub_00025040` does not retain all nine retail plain-ret exits.

## Regression coverage

`tests/test_v35_low_text_cfg_boundaries.py` was written first. Against v34 it failed because both boundaries and both promoted seeds were absent and the generated-contract verifier ignored the low-.text methods. After the v35 change it passes.

## Fresh local verification

- Focused v35 regression: **5 passed**.
- Full Python suite: **91 passed**.
- Python compile check: **passed**.
- Fresh native CMake configure/build: **passed**.
- Native CTest suite: **20/20 passed**.
- Windows runtime verification is still required. This manifest does not claim the final v34 access violation is fixed until the user's v35 run confirms the two ABI violations are gone.
