# JSRF Native v57 manifest

## Purpose

v56 removed the previous dirty-disc timing failure from the supplied Windows
run and exposed the next real omitted callback. Pass 1 reaches unresolved
indirect target `0x0013D840` from return site `0x00140904`, then later crashes
from downstream corrupted state. The easy tester validates that target against
the exact retail XBE, promotes one safe callback, rebuilds automatically, and
pass 2 remains alive until the user closes the framebuffer window.

## v56 pass-2 evidence

After automatic promotion:

- no repeat unresolved call to `0x0013D840` is observed;
- no `[DISCDIAG]` dirty-disc trigger is observed;
- the kind-1 player/resource loader advances rather than remaining stuck;
- the shared dispatcher later reaches `busy=0 kind=14 param=1`;
- `\\Device\\CdRom0\\Media\\Z_ADX\\BGM\\title.adx` opens with status success;
- its first 51,200-byte read returns all 51,200 bytes with status success;
- NV2A DMA/draw activity continues until the framebuffer window is closed.

The automatic promotion itself is conservative: it accepts a runtime target
only when the address maps to raw-backed executable code and the retail XBE
independently contains that exact address as a literal callback/function
pointer. Therefore the permanent v57 seed records evidence already validated by
the user's own retail executable instead of guessing a new callable address.

## v57 changes

1. Permanently seeds `0x0013D840`, recording v56's return site `0x00140904` and
   retail-pointer validation. Function extent remains discovered by xboxrecomp.
2. Keeps the historical trace-function list available but defaults
   `RECOMP_TRACE_BUDGET=0` for one-click runs when the user has not supplied an
   override. This stops old `[TRACE]`/`[ESP]` printing from perturbing real-time
   resource loading while retaining opt-in debugging.
3. Runtime banner advances to `JSRF Native v57`.
4. Retains v56 `[PLAYERLOAD]`/`[DISCDIAG]`, automatic safe callback promotion,
   all earlier retail boundary/runtime fixes, and v54 NV2A alpha/blend support.

No title state, callback return value, timeout, file result, resource completion
flag, input state, framebuffer contents, or menu transition is forced.

## Verification

TDD red phase against the v56 baseline: the three v57 regressions failed for
exactly the absent seed, absent trace-budget default, and old v56 banner.
After implementation the focused v57 regression passes.

Final full-suite/build/package verification is recorded after packaging.
Windows runtime behavior is not claimed by this manifest; the next clean v57
run is the validation.
