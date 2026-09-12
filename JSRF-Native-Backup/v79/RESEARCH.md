# JSRF v79 — GameObj exact-writer watch

## v78 result
The v78 sibling tracker resolves the central lifetime question. Crash target `0x013D1010` was previously observed as a valid GameObj header with vtable `0x001CB0A8`, state `0x00010007`, child `+0x28 = 0`, and sibling `+0x30 = 0x015A0870`.

At indirect-call count 495896, predecessor `0x01591F40` still points to `0x013D1010`, but the target now contains vtable `0x80008000`, state `0x8C818000`, `+0x24 = 0x8C818000`, `+0x28 = 0xA1FC97CB`, `+0x2C = 0x80008D97`, `+0x30 = 0x81688000`, `+0x34 = 0x80478000`, and `+0x38 = 0x8EBC9819`. The following sibling `0x81688000` contains the repeating `FFFF0000/55555555` pattern and crashes on the vtable+4 read.

## Interpretation
The same storage address transitions from a valid object header to data that no longer resembles a GameObj. This narrows the root cause to either a generated guest write overwriting live GameObj storage, or legitimate object deallocation/reuse while a stale sibling link remains. Existing kernel heap/VM logs do not name a free of `0x013D1010`, but JSRF's internal allocator is separate, so that absence is not proof of overwrite.

The replacement vtable `0x80008000` could be produced by partial 16-bit stores, so a writer watch must cover writes beginning within the full four-byte vtable word.

## v79 diagnostic
v79 patches the pinned xboxrecomp lifter's central memory-write formatter after the existing stack-write guard. Non-stack guest stores execute unchanged and then run `RECOMP_GAMEOBJ_WRITE_GUARD(address, width)`.

The runtime observer hashes only sibling targets already maintained by v78. A write is interesting only if it overlaps the four-byte vtable word of a target previously observed as a valid sibling. After the store, the helper re-reads the target; if the vtable changes from valid to invalid, `[GOBJWRITE]` reports the exact generated C file/line, target, address, size, previous parent, and previous/current indirect-call counts.

No write is blocked or repaired. If v79 reproduces the v78 transition without `[GOBJWRITE]`, the overwrite is likely outside ordinary generated scalar guest stores, such as a bulk/manual copy or allocator reuse path.
