# JSRF Native v61

Purpose: correct a retail CRI ADXT state-machine CFG split proven by the v60
Windows run.

Changes:
- pins the full retail ADXT cluster `0x0013CE30..0x0013D4E0` as seven adjacent,
  independently callable functions;
- prevents internal watchdog destinations `0x0013D3C6` / `0x0013D3F5` from
  surviving as standalone translated function starts;
- restores a stable generated entry for the state-2 decoder/output updater
  `0x0013D080`;
- adds all seven retail starts to `jsrf_runtime_seeds.json`;
- keeps the v58-v60 read-only ADXT diagnostics for the next Windows run;
- updates the runtime banner to `JSRF Native v61`;
- generalizes historical v55-v60 banner tests so later numbered builds remain
  valid regression targets.

Why this is a correction rather than a bypass: v60 recorded the retail `-1`
watchdog write with `stall6a=1` and `+0x38=60`. The retail instructions can
only take that write after the counter exceeds 300 (state 3) or 1200 (other
active states). Entering it at 1 therefore proves a translated CFG ownership
error. v61 changes only function boundaries/seeds; it does not modify the
retail status/counter/branch logic.