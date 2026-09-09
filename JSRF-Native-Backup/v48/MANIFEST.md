# JSRF Native v48 manifest

## Purpose

v47 narrows the remaining visible SEGA/title stall to the retail logo/bootstrap
object rather than the cache manager. The main title dispatcher reaches state 8
and remains there while the logo object advances through its timed state 1 and
then enters state 2. State 2 waits on retail helper `0x24650`, which returns the
completion flag of service id 6 at object offset `+0xC0`.

v48 adds read-only diagnostics around that real fade service. It does not force
the logo state, synthesize fade completion, or replace title behavior.

## v47 Windows evidence

- title state: `0 -> 3 -> 6 -> 7 -> 8`;
- logo state: `0 -> 1`, timer advances through 120, then state 2 with timer 121;
- cache-started flag becomes 1 during the timer;
- logo state 2 persists for thousands of updates;
- no runtime crash was required to explain the stall;
- the master cache completion marker already exists and is successfully queried.

## Retail fade path

Direct retail disassembly shows:

- `0x24540`: service-6 fade command. It clears completion `+0xC0`, decodes the
  packed four-channel target into floats at `+0xA8..+0xB4`, and stores the
  per-frame step at `+0xB8`.
- `0x24650`: service-6 completion query. It returns `+0xC0`.
- `0x24700..0x24963`: service-6 update. It moves current channels
  `+0x98..+0xA4` toward their targets, counts equal channels, sets `+0xC0 = 1`
  only when all four match, and returns.
- `0x24970`: next retail function after padding.

## v48 diagnostics

- `[FADECMD]` at retail `0x24540` records the service object plus packed target
  and raw step argument.
- `[FADEUPDATE]` at retail `0x24700` records current four channels, target four
  channels, step, and completion flag.
- `[FADEUPDATE]` at internal retail `0x24945` also records the exact completion
  decision count held in ECX.
- `[FADEUPDATE]` at retail `0x24962` records the same state at update exit.
- `[POSTCACHECFG]` now also reports raw detected starts across
  `0x24540..0x24970` before JSRF boundary pinning.
- all new probes are read-only.
- runtime startup identifies itself as `JSRF Native v48`.

## Regression coverage

`tests/test_v48_fade_service_diagnostics.py` covers:

- generated instrumentation at command, update entry, completion decision, and
  update exit;
- probe idempotence;
- current/target/step/completion diagnostic fields;
- read-only probe behavior;
- post-codegen pipeline ordering;
- unambiguous v48 runtime banner.

The v47 historical banner regression was generalized to accept v47 or later so
future diagnostic releases do not invalidate historical coverage.

## Verification

Fresh pre-package verification:

- focused v48 regression: **4/4 passed**;
- full Python suite: **155 passed**;
- Python `py_compile`: **passed**;
- `recomp_manual.c` C syntax check: **passed**;
- fresh CMake/Ninja native configure/build: **passed**;
- native CTest suite: **20/20 passed**.

Windows runtime confirmation is still required before making any behavioral
fade-service correction.
