# JSRF Native v47 manifest

## Purpose

v47 follows the first successful end-to-end cache-manager run. v46 fixes guest
`NtFreeVirtualMemory`; the returned Windows log shows successful `[VMFREE]`
releases, no guest-heap exhaustion, completion of numbered cache work through
slot 8, and successful creation of the master `JSRF_CACHE_COMPLETE.CMP` marker.
The process then remains alive and continues GPU work until the user closes the
window.

The remaining stall is therefore post-cache. v47 adds only targeted read-only
diagnostics to identify the exact higher-level retail state before making any
new behavioral correction.

## New diagnostics

- `[POSTCACHECFG]` reports raw disassembler function starts for
  `0x7BDD0..0x7BE30` and `0x7E360..0x7E550` **before** the existing JSRF
  boundary pinner runs.
- `[TITLESTATE]` observes the main `0x7BDD0` title/bootstrap dispatcher and its
  `+0x48` state.
- `[LOGOSTATE]` observes the `0x7E360` logo/cache-activation update method and
  its `+0x98/+0x9C/+0xA0/+0xA4` state fields plus associated resource fields.
- logging is state-change/first-hit/power-of-two rate limited.
- no guest register or guest memory is modified by these probes.
- runtime startup identifies itself as `JSRF Native v47`.

## Retail evidence behind the probes

- `0x7BDD0` dispatches `+0x48` through the 64-entry function-pointer table at
  `0x20D2B8`.
- `0x7E360` is vtable `0x1CCFB8` slot `+4` and contains an embedded switch:
  jump table `0x7E524`, compact state table `0x7E540`, next function `0x7E550`.
- after cache slot 9 creates the master completion marker, retail `0x25040`
  calls generic object-removal routines `0x11BE0` / `0x11C20`.

No speculative boundary pin, forced title transition, fabricated marker, or
replacement gameplay/render behavior is added in v47.

## Regression coverage

`tests/test_v47_post_cache_title_diagnostics.py` covers:

- raw pre-pin CFG reporting;
- generated entry instrumentation for both state machines;
- probe idempotence;
- read-only host diagnostic behavior;
- pipeline ordering (CFG report before pinning; runtime instrumentation after
  code generation and before generated contracts);
- unambiguous v47 runtime banner.

## Verification

Fresh pre-package verification:

- focused v47 regression: **5/5 passed**;
- full Python suite: **151 passed**;
- Python `py_compile`: **passed**;
- `recomp_manual.c` C syntax check: **passed**;
- fresh CMake/Ninja native configure/build: **passed**;
- native CTest suite: **20/20 passed**.

The final ZIP was re-extracted and the same verification repeated before release.