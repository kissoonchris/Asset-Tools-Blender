# JSRF Native v52 manifest

## Purpose

v51 corrected JSRF's packed 16-bit NV2A clear colours and the Windows run now
survives the startup-logo sequence without crashing. After the logos the retail
title dispatcher reaches state 12, but the framebuffer stays black and the
state never advances.

## Windows evidence

The v51 runtime reaches title state 12 at hit 2429 and is still in state 12 at
hits 4096 and 8192. There is no crash. Immediately after entering that state,
the indirect-call bridge repeatedly reports one missing retail target:
`0x000521B0`, always returning to `0x00051FF9`. The run records 6,658 such
unresolved calls.

## Retail root cause

Retail `0x00051FC0` dispatches a 119-entry callback table at `0x001F9888` using
state field `+0x5C`. Table slot 7, stored at `0x001F98A4`, is the literal
function pointer `0x000521B0`. The runtime is therefore not seeing a random
pointer: it is dropping a real retail state callback selected by the original
game.

Direct disassembly of the retail XBE shows `0x000521B0` is one complete
function. It begins after padding with `sub esp,0x28; push ebx; push esi`, runs
resource/table processing, writes state `8` to `[esi+0x5C]`, restores its frame,
and returns at `0x00052342`. Padding continues through `0x0005234F`, and the
next real function begins at `0x00052350`.

Because generic function recovery omitted `0x000521B0` as an emitted callable
entry, the indirect dispatcher skips the callback entirely. That prevents its
real retail state transition from running and leaves the title's post-logo
object alive while title state 12 waits.

## v52 correction

`scripts/jsrf_xboxrecomp.py` now pins the retail-proven function interval:

- start `0x000521B0`
- end `0x00052350`
- indirect caller `0x00051FF7`
- literal pointer evidence `0x001F98A4`

The generated-code contract also requires `sub_000521B0` to exist with its
retail plain-RET exit before a Windows build is accepted.

No menu state, loading state, object lifetime, or framebuffer content is
forced. v52 only restores a retail callback the generic disassembler failed to
emit.

## Regression coverage

`tests/test_v52_521b0_state_callback_boundary.py` covers the exact boundary,
retail evidence/caller metadata, removal of false internal starts, generated
callback contract, pipeline activation, and v52 banner. The focused tests were
observed failing against v51 before the production change and passing after the
minimal boundary recovery.

## Graphics status

v51 fixed the incorrect logo background colours. Startup flicker remains a
separate presentation-timing issue. v52 does not bundle a speculative flicker
change with this post-logo execution fix.
