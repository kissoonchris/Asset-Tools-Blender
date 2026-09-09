# JSRF Native v49 manifest

## Purpose

v48's read-only fade diagnostics identify the next title-screen blocker as a
recompiler instruction-semantics defect rather than a cache, title-state, or
fade-state policy problem. The retail fade update advances channel 0 by exactly
one configured step, but the clamp helper called before update return writes the
channel back to zero every frame.

v49 implements the missing x87 `FCMOVE` / `FCMOVNE` behavior in the pinned
xboxrecomp lifter. It does not synthesize a completed fade and does not force the
logo object out of state 2.

## v48 Windows evidence

At logo timer 120, retail code issues service-6 command `packed=0xFF000000` with
step bits `0x3C088889` (approximately `1/120`). On the first state-2 update:

- fade update entry has channel 0 at `0.0` and target `1.0`;
- the internal completion-decision point has channel 0 at `0x3C088889`;
- update exit has channel 0 back at `0.0`;
- the completion count remains 3 because only the other three channels match;
- the same one-step-then-zero pattern is still present thousands of frames
  later while logo state remains 2.

That proves the interpolation branch itself is executing and narrows the reset
to code between retail `0x24945` and `0x24962`.

## Retail clamp path

Direct disassembly shows `0x24954` calls retail helper `0x000A4CF0`, passing the
four current fade-channel floats. The helper clamps each channel to `[0, 1]` by
calling CRT routines `0x0014C870` and `0x0014C850`. Those routines use:

- `fcmove st, st(1)`;
- `fcmovne st, st(1)`.

For `max(value, 0)`, dropping `FCMOVNE` leaves x87 `st(0)` holding the literal
zero. The caller then stores that zero over the positive fade value. This
exactly matches the v48 runtime trace.

## Recompiler defect

The pinned xboxrecomp revision routes every `f*` mnemonic to `_lift_fpu()`.
Unrecognised x87 mnemonics fall through to a comment-only lift. The pinned
lifter has no `fcmove`/`fcmovne` implementation, so both retail conditional
moves were silently discarded.

## v49 correction

`scripts/jsrf_xboxrecomp.py` adds `patch_xboxrecomp_fcmov_flags()`, applied
before code generation. It patches the pinned `tools/recomp/lifter.py` to:

- recognize `fcmove` and `fcmovne` in `lift_basic_block`;
- map them to the existing ordinary `CMOVcc` condition reconstruction so they
  consume the correct live TEST/CMP EFLAGS;
- copy the selected x87 `st(i)` source into `st(0)` when the condition is true;
- classify `fcmove`/`fcmovne` as EFLAGS-preserving;
- remain idempotent and fail loudly if the pinned lifter's expected structure
  changes.

The v48 fade diagnostics remain read-only and enabled. Runtime startup identifies
itself as `JSRF Native v49`.

## Regression coverage

`tests/test_v49_fcmov_lifter_fix.py` covers:

- insertion of real `fcmove`/`fcmovne` handling;
- reuse of the existing CMOVcc EFLAGS condition builder;
- x87 source selection into `st(0)`;
- EFLAGS preservation;
- patch idempotence;
- pre-codegen pipeline ordering;
- unambiguous v49 runtime banner.

## Verification

Pre-package verification:

- focused v49 regression: **4/4 passed**;
- full Python suite: **159 passed**;
- Python `py_compile`: **passed**;
- `recomp_manual.c` C syntax check: **passed**;
- fresh CMake/Ninja native configure/build: **passed**;
- native CTest suite: **20/20 passed**.

The final ZIP is freshly extracted and re-verified before release. Windows
runtime confirmation is still required before claiming the title advances
beyond the SEGA/logo transition.
