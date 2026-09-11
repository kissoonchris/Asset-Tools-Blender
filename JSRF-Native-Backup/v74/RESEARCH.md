# JSRF v74 - GameObj execution-tree boundary

## What v73 proved

The v73 Windows run moves beyond both the old ADX completion stall and the cleanup-stack corruption. Immediately before the new crash, the main ADXT ring reports 51,164 filled bytes and a read position of 51,200 bytes. The audio path is therefore producing data rather than remaining at state 2 / position 0 with an empty ring.

The one-click runner also observes one unresolved callback on its first pass and independently validates `0x0013B750` against the supported retail XBE. After rebuilding with that callback entry, the next run reaches a different deterministic boundary. v74 keeps `0x0013B750` as a permanent evidence-based seed so subsequent tests do not need to rediscover it.

## New deterministic crash

The second pass faults inside retail `0x00011070` (`recursiveExecDefault`) while reading Xbox VA `0xFFFF0004`.

At the fault:

- `ESI = ECX = 0x81688000`
- `EAX = 0xFFFF0000`
- the generated source is at the indirect dispatch after `eax = MEM32(esi)`
- the next operation is effectively the vtable-slot read `MEM32(eax + 4)`

`0x81688000` is in the Xbox contiguous-memory window, but its first dword is not a plausible GameObj vtable. The fault is therefore downstream evidence; simply skipping the object or treating `0xFFFF0000` as a valid vtable would conceal the writer or link that placed this address into the execution tree.

The same retail routine traverses two explicit links:

- `+0x28`: child pointer used for recursive execution
- `+0x30`: sibling/next pointer used to continue the local loop

The v73 crash log does not preserve the immediately preceding parent/edge strongly enough to say which one introduced `0x81688000`.

## v74 evidence capture

v74 instruments only generated `sub_00011070` and does not alter its guest behavior. Read-only `[GOBJ]` probes are inserted at:

- `0x00011070`: function entry / current object
- `0x00011083`: immediately before current-object vtable dispatch
- `0x0001108A`: after loading child link `+0x28`
- `0x00011096`: before following sibling link `+0x30`

Suspicious records include the current object, target edge, vtable, state field, nearby link fields `+0x24..+0x38`, target vtable/state, ESP, and guest registers. Duplicate identical edges are rate-limited but the first four are retained.

## Decision rule for the next Windows run

- If `[GOBJ] site=0001108A` shows `target=81688000`, the bad pointer comes directly from a parent's `+0x28` child field.
- If `[GOBJ] site=00011096` shows `target=81688000`, it comes directly from a `+0x30` sibling field.
- If entry `00011070` first sees `obj=81688000`, use the immediately preceding child/sibling record to identify the source edge.
- If a normal low-memory object reaches `00011083` with `vtable=FFFF0000`, then the object itself was overwritten before traversal rather than merely linked incorrectly.

Once the source object and field are known, the next checkpoint can watch writes to that exact link/object boundary and trace the corruption back to its producer. v74 intentionally stops one step earlier: provenance first, correction second.
