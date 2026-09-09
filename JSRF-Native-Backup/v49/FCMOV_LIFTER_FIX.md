# JSRF v49 - x87 FCMOV fade reset root cause

## Symptom isolated by v48

The logo object reaches state 2 after its 120-frame timer and waits on the
service-6 fade completion flag. v48 probes show that the fade update computes
the correct first increment but loses it before returning:

- update entry (`0x24700`): current channel 0 = `0x00000000`;
- internal decision (`0x24945`): current channel 0 = `0x3C088889`;
- update exit (`0x24962`): current channel 0 = `0x00000000`;
- target channel 0 = `0x3F800000` (1.0);
- step = `0x3C088889` (~1/120);
- completion count = 3.

The pattern repeats for thousands of frames. Therefore the update's compare,
step and clamp-to-target branch are not the fault; something after `0x24945`
undoes the result.

## Retail path after the completion decision

The retail code at `0x24954` pushes the current-channel array, sets `ecx` to the
fade object and calls `0x000A4CF0`. The helper processes four floats in place,
clamping each to `[0,1]`, and returns a packed color value stored at `+0xBC`.

Its upper/lower clamp calls reach CRT helpers at `0x0014C870` and
`0x0014C850`. Their key instructions are x87 conditional moves:

```text
0x0014C850 ... fcom st(1) ... test ah,1 ... fcmovne st,st(1) ...
0x0014C870 ... fcom st(1) ... test ah,1 ... fcmove  st,st(1) ...
```

These are min/max-style selectors. In the lower clamp, a positive fade value
versus 0 must select the positive source. If `fcmovne` is skipped, `st(0)`
remains zero and the helper writes zero back into the fade channel.

## Pinned xboxrecomp behavior

At revision `e3caa3718c80a241b31768852fa752aa6577b66c`, the lifter dispatches any
mnemonic beginning with `f` to `_lift_fpu()`. Its fallback for unknown x87
instructions is comment-only output. No `fcmove` or `fcmovne` handler exists,
so JSRF's two conditional moves have no runtime effect.

The block lifter already has robust ordinary `CMOVcc` condition reconstruction
from the live flag producer. v49 reuses that machinery rather than inventing a
JSRF-specific fade override.

## v49 fix

The project-side patch:

1. preserves EFLAGS across `fcmove`/`fcmovne`;
2. maps `fcmove -> cmove` and `fcmovne -> cmovne` for the existing condition
   builder;
3. identifies the x87 source operand as the final `st(i)` operand;
4. emits `if (condition) fp_top() = source;`;
5. runs before generated C is produced;
6. is idempotent and pinned-source guarded.

This restores the retail x87 instruction semantics. It does not touch the fade
object, completion flag, timer, title state, or logo state directly.

## Expected Windows proof

With v49, `[FADEUPDATE]` should show channel 0 accumulating rather than resetting
to zero: roughly `1/120`, `2/120`, ... until `1.0`. Once all four channels match,
retail code should set the completion flag and the logo state machine should be
able to leave state 2. That remains a hypothesis until a Windows runtime log
confirms it.
