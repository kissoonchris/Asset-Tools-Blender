# JSRF v47 — post-cache title-state diagnostics

## What the v46 Windows run proved

v46 resolves the heap-lifetime failure exposed by v45. Original-Xbox ordinal
199 now releases guest allocations through the guest heap and returns success.
The run records 32 `[VMFREE]` releases and no `xbox_HeapAlloc: out of memory`
message.

More importantly, the retail cache manager now finishes its work rather than
stalling inside it:

- numbered cache work advances through slots 0..8;
- the manager reaches `slot=9`, `state=5`;
- `JSRF_CACHE_COMPLETE.CMP` is created successfully;
- after that point the cache manager's `0x25040` state callback is no longer
  observed, consistent with retail removing the completed cache object;
- the process remains alive and continues GPU clear/draw work until the user
  closes the framebuffer window.

So the next blocker is above cache enumeration/loading and above the v46 guest
heap fix.

## Retail post-completion control flow

Direct disassembly of the supported retail XBE shows state 5 in
`0x25040..0x252E0` creates the completion marker through `0x24E70`. When the
current slot is 9, successful marker creation immediately calls the generic
object-tree removal routines `0x11BE0` and `0x11C20`. This explains why
`[CACHESTATE]` stops after the master marker: the cache object is finished and
retail deliberately detaches it.

The relevant higher-level state paths are now:

- `0x7BDD0`: main title/bootstrap state dispatcher. The state index is object
  field `+0x48`, dispatched through the retail pointer table at `0x20D2B8`.
- `0x7E360`: update method installed in vtable `0x1CCFB8` at slot `+4`
  (`0x1CCFBC`). It drives a 0..15 logo/cache-activation state in `+0x98`, a
  timer in `+0x9C`, skip/input state in `+0xA0`, and one-shot cache activation
  state in `+0xA4`.
- `0x7E360..0x7E550` contains an embedded computed jump table at `0x7E524`
  plus its compact state-byte table at `0x7E540`; `0x7E550` is the next retail
  function start.

This last point matters because earlier JSRF bring-up failures were caused by
generic function recovery splitting internal switch cases out of their owner.
The current evidence does not yet prove that happened here, so v47 does not pin
or rewrite this function speculatively.

## v47 diagnostics

v47 is deliberately read-only at the new observation points:

1. Before the existing JSRF boundary pinner changes `functions.json`, the build
   prints `[POSTCACHECFG]` entries for raw disassembler starts in
   `0x7BDD0..0x7BE30` and `0x7E360..0x7E550`.
2. Generated `0x7BDD0` receives a `[TITLESTATE]` entry probe that logs the
   object and `+0x48` state only when it changes, for the first few calls, and
   at power-of-two intervals.
3. Generated `0x7E360` receives a `[LOGOSTATE]` entry probe that logs `+0x98`,
   `+0x9C`, `+0xA0`, `+0xA4` and related resource/object fields with the same
   rate limiting.

The probes read guest state only. They do not write registers, guest memory,
cache markers, title state, timers, or renderer state.

## What the next Windows run must answer

The next log should tell us two things directly:

- whether the raw disassembler has split `0x7E360` at one of its internal
  switch-case destinations; and
- which actual retail title/logo state remains active after
  `JSRF_CACHE_COMPLETE.CMP` is created.

That is enough to distinguish a CFG-boundary defect from an authentic state
waiting on a specific retail condition without forcing either outcome.