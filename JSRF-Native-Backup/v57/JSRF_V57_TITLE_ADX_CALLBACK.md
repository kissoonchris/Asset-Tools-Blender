# JSRF v57 — title ADX callback promotion

## What the v56 Windows run proves

The first v56 pass gets past the shared-resource dirty-disc condition: no
`[DISCDIAG]` event is emitted and the kind-1 `[PLAYERLOAD]` state machine keeps
advancing. The first new deterministic failure is instead an unresolved
indirect call to `0x0013D840`, returning to retail call site `0x00140904`.
The skipped call is followed by downstream register/stack damage and the run
ends in an access violation inside `sub_00011070`.

The one-click launcher's existing conservative promotion pass accepts exactly
one callback. Acceptance requires two independent checks against the user's
supported retail XBE: the target must be in raw-backed executable-image bytes
and the XBE must contain the address as a literal function/callback pointer.

With `0x0013D840` seeded automatically, pass 2 does not repeat that unresolved
call or crash. The shared resource dispatcher finishes the observed sequence
through kind 14 and reaches `busy=0`. The title then opens
`\\Device\\CdRom0\\Media\\Z_ADX\\BGM\\title.adx` successfully and reads the
first 51,200-byte request completely. GPU work continues until the framebuffer
window is closed by the user.

## v57 change

Make the auto-recovered `0x0013D840` entry a permanent seed so a clean v57
install starts with the same callable set that v56 discovered on its second
pass. No function end, return value, stack cleanup, file status, or title state
is guessed; xboxrecomp still discovers the function extent from the retail
image.

The historical trace-function list is also left intact but its print budget is
defaulted to zero for the normal one-click run. v56 still emitted large numbers
of `[TRACE]`/`[ESP]` lines from old bring-up probes even after the corresponding
startup failures were solved. `RECOMP_TRACE_BUDGET` remains user-overridable,
so a targeted debugging run can opt back in. Unresolved-call logging, crash
logging, GPU logging, title/resource probes and the automatic callback promoter
are independent of that budget.

## Next Windows evidence

A clean v57 run should no longer need an automatic rebuild for `0x0013D840`.
The important behavioral evidence is what the framebuffer shows after the
startup sequence and whether the title can be interacted with. State 12 is not
forced or bypassed; the supplied v56 pass already shows resource loading and
`title.adx` continuing while that higher-level state is active.
