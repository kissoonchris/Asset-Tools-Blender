# JSRF v50: 0x0007B8D0 SEH vtable-method boundary

## Windows evidence from v49

v49 fixes the x87 `FCMOVE`/`FCMOVNE` fade reset and the retail startup proceeds
through the later logos to the graffiti disclaimer. Immediately before the
new crash, the title object is in state 11.

The first new ABI violation is the virtual target `0x0007B8D0`, dispatched by
the already-known object traversal at the retail return site `0x0001108A`:

- ABI-call entry ESP: `0x00F7FEE0`
- return ESP: `0x00F7FECC`
- deficit: `0x14` bytes

The next ABI report for `0x00011070` carries the same `0x14` deficit. Execution
then reaches `sub_00011DA0` with `EDI=0xFB0AFB0A`; its retail load from
`[EDI+4]` faults at `0xFB0AFB0E`. The wild pointer is therefore downstream
stack corruption, not the first failure.

## Retail executable evidence

The supported retail XBE is SHA-256
`fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`.
Direct disassembly of retail `.text` proves `0x0007B8D0..0x0007BC03` is one
SEH-framed method:

- prologue pushes `-1`, handler data, old `fs:[0]`, then installs `esp` into
  `fs:[0]`;
- all exits restore `fs:[0]` and execute `add esp, 0x10`;
- there are seven plain `ret` exits at `0x7B93B`, `0x7B955`, `0x7B977`,
  `0x7BA18`, `0x7BB62`, `0x7BBBE`, and `0x7BC03`;
- `0x7BC04..0x7BC0F` is NOP padding;
- `0x7BC10` begins the next independent SEH-framed function.

The literal dword `0x0007B8D0` occurs exactly once in the retail XBE. It is
stored at `.rdata` VA `0x001CCE34`, slot +4 of the vtable beginning at
`0x001CCE30`. Constructors in retail code install that vtable. This is
independent evidence that `0x7B8D0` is a callable method start.

## Root cause

Generic function recovery split the SEH method at internal early-return / CFG
landmarks. On the path exercised after the graffiti disclaimer, the generated
method returns without the owning SEH frame cleanup, leaving ESP exactly
`0x14` bytes too low. The corruption propagates into the caller and eventually
turns an object-list pointer into `0xFB0AFB0A`.

This is the same class of static-recompiler boundary failure previously proven
for the slot +4 method at `0x0007E180`.

## v50 correction

v50 pins the complete retail interval `[0x0007B8D0, 0x0007BC10)` with:

- caller evidence `0x0001108A`;
- vtable evidence `0x001CCE34`.

The generated-code contract additionally requires all seven retail plain-RET
exits to survive code generation. No title state, menu transition, or object
pointer is synthesized or forced.

## Flicker

The v49 Windows run also visibly flickers during the logo sequence. That is
tracked separately. The current NV2A path still reports substantial unhandled
method coverage, but the v49 logs do not yet prove which missing graphics
semantic causes the flicker. v50 therefore fixes only the deterministic crash
rather than adding speculative GPU behavior.
