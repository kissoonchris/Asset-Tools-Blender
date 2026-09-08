# JSRF v31 — kernel thunk dispatch slot must be guest-thread-local

## What v30 captured

v30 stopped before the worker could overwrite the TIB above its simulated
stack.  The protected worker was created with stack top `0x011EAFF0` and TIB
`0x011EB000`.  At the guard:

- write address: `0x011EAFF4`
- ESP after the push decrement: `0x011EAFF4`
- stack top: `0x011EAFF0`
- `fs:[4]`: still valid at `0x011EB054`
- generated source: `recomp_0004.c:119002`
- retail instruction represented there: `0x0013B1EB: push ecx`

The push is therefore the first *write* beyond the ceiling, not necessarily the
instruction that originally moved ESP upward.

## Backward trace

The worker routine is retail `0x0013B1C0`.  Its loop calls `0x00141E50`, whose
registered callback chain reaches:

`0x00141E50 -> 0x00141F10 -> 0x0013A9E0 -> 0x0013D690 ->`
`0x0013C460 -> 0x00141B60 -> 0x0013B0A0`.

The v30 ABI diagnostics capture a decisive state while that chain is active:

`0x00147D12` entered with guest ESP `0x011EAF64` and returned with
`0x011EAF74`, while ESI changed to `0x0013B0B1`.

Retail `0x0013B0A0` calls `0x00147D12` at `0x0013B0AC`; `0x0013B0B1` is the
literal return address immediately after that call.  Retail `0x00147D12` saves
ESI, calls these kernel exports in order, then pops ESI:

1. ordinal 246 `ObReferenceObjectByHandle` (3 stack args)
2. ordinal 124 `KeQueryBasePriorityThread` (1 stack arg)
3. ordinal 250 `ObfDereferenceObject` (fastcall; object in ECX, **0 stack args**)
4. `pop esi`

If the third kernel dispatch advances the simulated stack eight bytes too far,
the `pop esi` reads the caller's return address.  That predicts ESI becoming
`0x0013B0B1` and the containing call returning eight bytes too high — exactly
what v30 recorded.

## Runtime root cause

The pinned xboxrecomp kernel bridge routes every synthetic kernel thunk through
one common C function.  `recomp_lookup_kernel()` first stores the selected slot
in:

```c
static int g_kernel_dispatch_slot = -1;
```

and then returns `kernel_thunk_dispatch`.  The dispatcher reads that shared
integer later and uses the resulting slot both to choose the ordinal and to
apply `g_slot_arg_bytes[slot]` to the simulated guest stack.

That selector is process-global even though guest registers, ESP, TIB state,
and spawned workers are `RECOMP_TLS`.  With multiple guest host threads running
at once, thread A can resolve slot 52 (ordinal 250, zero stack args), thread B
can overwrite the global selector with another slot, and thread A then enters
the common dispatcher using thread B's ordinal/stack-cleanup size.  A wrong
8-byte cleanup produces the exact v30 stack movement described above.

The current upstream default branch still contains the same process-global
selector, so this is not fixed by merely moving from the v29 pinned revision to
today's upstream head.

## v31 fix

JSRF's pinned-toolkit transform changes only the dispatch selector storage:

```c
static RECOMP_TLS int g_kernel_dispatch_slot = -1;
```

No kernel ABI sizes, retail game functions, D3D behavior, audio behavior, or
worker stack sizes are altered.  Each host thread now carries the kernel thunk
slot that belongs to its own guest register/ESP state.

The v30 first-write stack guard remains enabled for the next Windows run.  If
this diagnosis is complete, the old TIB-boundary guard must stop firing there;
if another independent stack bug exists, the guard will catch that next write
without allowing TIB corruption.
