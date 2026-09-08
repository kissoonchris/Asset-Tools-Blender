# JSRF v30 — first-write worker stack guard

## What the v29 Windows run proved

v29 removed the per-vblank host-thread/TIB allocation storm by using xboxrecomp's persistent timer-thread vblank implementation. The title then ran to more than one million kernel calls before failing in retail `sub_00146429`, the TLS/LastError-style helper.

The affected worker was created with:

- guest stack top `0x011EAFF0`
- TIB `0x011EB000`
- initial `fs:[4] = 0x011EB054`

At the crash the same TIB base remained selected, but `fs:[4]` was zero. The crash stack also showed retail code addresses in memory at `0x011EB000` and `0x011EB008`, which are TIB header locations immediately above that worker stack. Those values are stack-shaped evidence, but the crash occurs after the overwrite, so changing the TIB or adding stack padding would only hide the source.

## v30 diagnostic

v30 does not enlarge the worker stack and does not modify retail JSRF behavior. Instead it patches the pinned xboxrecomp runtime/code generator before lifting:

1. `PUSH32` checks the destination before every translated push/call stack write.
2. ESP/EBP-relative translated memory stores check their destination before writing.
3. Only spawned guest workers are watched.
4. A write whose range reaches beyond that worker's recorded stack top, within the immediately-adjacent region, prints `[STACKGUARD]`, the current guest ESP, stack top, TIB state, and generated C `source <file>:<line>`.
5. The runtime exits with diagnostic code 4 *before* performing the corrupting write.

The existing beginner runner harvests generated source context from `source <file>:<line>`, so the next Windows test should identify the exact lifted instruction responsible for the first crossing rather than only the later TLS crash.

## Scope

This is evidence-gathering instrumentation, not a claimed root-cause fix. The next Windows run determines whether the first crossing is a push/call stack imbalance, an ESP/EBP-relative store beyond the frame, or another path that needs narrower tracing.
