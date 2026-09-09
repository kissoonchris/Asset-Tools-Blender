# JSRF Native v50 manifest

## Purpose

v49 fixes the x87 conditional-move defect and, on Windows, advances through the
startup logos to the graffiti disclaimer. The next deterministic failure is a
stack-corrupting access violation during the transition toward the main menu.

v50 fixes the newly proven static-recompiler function-boundary defect at retail
vtable method `0x0007B8D0`. It does not force the title state or main-menu
transition.

## v49 Windows evidence

At the final logged title update, retail title state is 11. The first new ABI
break is target `0x0007B8D0`, whose guest ESP returns `0x14` bytes below its
ABI-call entry. The already-pinned `0x00011070` traversal then carries the same
`0x14` deficit. The process finally faults in `sub_00011DA0` while reading
`[EDI+4]` with `EDI=0xFB0AFB0A`.

This orders the failures: `0x7B8D0` stack corruption is upstream; the wild
pointer in `0x11DA0` is downstream damage.

## Retail proof

Direct disassembly of the supported retail XBE proves:

- `0x7B8D0` begins an SEH-framed vtable method;
- the method has seven plain-RET exits;
- every exit restores `fs:[0]` and removes the 16-byte SEH/local frame;
- padding follows the last RET through `0x7BC0F`;
- `0x7BC10` is the next real function start;
- `.rdata` slot `0x001CCE34` contains the unique literal pointer `0x0007B8D0`.

## v50 correction

`scripts/jsrf_xboxrecomp.py` now pins:

`[0x0007B8D0, 0x0007BC10)`

with caller evidence `0x0001108A` and vtable evidence `0x001CCE34`.
`verify_jsrf_generated_contracts()` has a v50 contract requiring all seven
plain-RET exits, and the production pipeline enables that contract after code
generation. Runtime startup identifies itself as `JSRF Native v50`.

## Regression coverage

`tests/test_v50_7b8d0_seh_vtable_boundary.py` covers:

- exact retail start/end/caller/vtable evidence;
- absorption of internal false starts;
- rejection of a generated method with only six RET exits;
- acceptance of all seven retail RET exits;
- unambiguous v50 runtime banner.

The v50 tests were written and observed failing against v49 before the
production change, then passed after the minimal boundary/contract correction.

## Flicker status

The user also reported substantial flickering throughout the startup logos.
This is not claimed fixed in v50. Current logs show the graphics path remains
partial, but do not isolate one proven NV2A semantic responsible for the
flicker. Graphics diagnosis remains separate from this deterministic crash fix.

## Verification

Final package verification is recorded in `JSRF-V50-ARTIFACTS.md`. Windows
runtime confirmation is still required before claiming the main menu is
reached.
