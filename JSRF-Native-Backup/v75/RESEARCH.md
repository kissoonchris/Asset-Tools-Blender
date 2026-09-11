# JSRF v75 - Recursive GameObj parent mutation

## v74 evidence

The main ADXT producer ring remains healthy immediately before the crash (`filled=51164`, `read_pos=51200`). The new GameObj trace reports:

- `site=00011096 obj=013D1010 target=81688000`
- parent `+0x30 = 0x81688000`
- parent state at that moment is already corrupt (`vtable=80008000`, `state4=8C818000`, multiple implausible link values)
- target `0x81688000` begins with `0xFFFF0000`, and the next vtable slot read faults at `0xFFFF0004`

The timing is decisive. The v74 helper also probes `0x00011083` before current-object virtual dispatch and `0x0001108A` after loading `+0x28`. It logs any high object/target or bad vtable. There is no earlier suspicious record for `0x013D1010`, so the parent is still plausible before recursive child execution and becomes corrupt only after that child subtree returns to `0x00011096`.

## v75 capture

v75 places a per-thread bounded snapshot stack around the real recursive call:

- begin at `0x00011091`: save parent, child, depth, parent vtable/state, links `+0x24..+0x38`, and child vtable;
- end at `0x00011096`: match the current parent to the top snapshot, re-read its fields, and emit only if something changed;
- paths that reach `0x00011096` without taking a child edge do not pop an ancestor snapshot.

A `[GOBJMUT]` record includes exact before/after field values and the child pointer whose subtree caused the mutation. Because nested recursion has its own snapshot frame, the deepest emitted mutation is the tightest currently provable culprit boundary.

## Next-run decision rule

Use the deepest `[GOBJMUT]` entry. Its `child=` value identifies the recursive subtree that changes the parent. If that child itself mutates the parent during its own virtual dispatch before recursing, instrument the child dispatch boundary; if a deeper recursive descendant is responsible, the nested snapshot stack should expose it during unwind without another broad trace.
