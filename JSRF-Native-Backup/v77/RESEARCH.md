# JSRF v77 - GameObj virtual-dispatch boundary

## v76 Windows result

The v76 generation hotfix restored native execution. The runtime again reached retail `0x00011070` (`recursiveExecDefault`) and reproduced the same final edge:

- parent at `loc_00011096`: `0x013D1010`
- parent vtable by then: `0x80008000`
- parent `+0x30`: `0x81688000`
- target `0x81688000` first dword: `0xFFFF0000`
- final fault: read of `0xFFFF0004` while resolving vtable slot `+4`

Audio remains beyond the previous blocker: the main ADXT ring reaches `filled=51164` and `read_pos=51200`.

## Rejected hypothesis

v75/v76 wrapped the real recursive `+0x28` child call with a nested-safe parent snapshot. The v76 runtime log contains no `[GOBJMUT]` record before the parent is observed corrupt. Therefore the recursive child subtree is not the operation that changes the parent.

This corrects the inference made after v74. The absence of a suspicious `[GOBJ]` record at an earlier site was not sufficient to prove the parent remained unchanged across every intervening operation because `[GOBJ]` intentionally logs only suspicious states.

## Remaining state-changing boundary

The lifted sequence is:

1. `loc_00011083`: read parent vtable, set `ecx = esi`.
2. indirect call to `MEM32(vtable + 4)`.
3. `loc_0001108A`: read `parent + 0x28` child.
4. optional recursive call.
5. `loc_00011096`: read `parent + 0x30` sibling.

With the recursive call excluded, the vtable `+4` dispatch is the remaining state-changing operation capable of explaining the observed transition.

## v77 diagnostic

v77 brackets that indirect call. `jsrf_gameobj_vcall_begin()` records the original parent, dispatch vtable, resolved target, and parent header/link fields. `jsrf_gameobj_vcall_end()` always re-reads the **original parent address**, not merely returned `ESI`, and separately compares returned `ESI` to the original parent.

`[GOBJVCALL]` is emitted only when:

- parent memory changed/unreadable after the call, or
- returned `ESI` differs from the original parent.

This distinguishes two materially different root causes in one Windows run:

- `memory_changed=1, register_changed=0`: the called retail method (or something it invokes) overwrote/freed/reused the object.
- `register_changed=1`: the indirect-call path violated the expected callee-saved object-register contract; the memory comparison still shows whether the original object was also modified.

No guest state is modified by the diagnostic.
