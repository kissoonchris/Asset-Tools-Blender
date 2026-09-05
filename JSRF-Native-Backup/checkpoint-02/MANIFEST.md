# JSRF Native checkpoint 02

Local source commit: `0d1091f06dcc72d372ce7ffbd1b2bb13722dfd94`

Verified with retail Media files: 17/17 CTest tests passing.

New recovery in this checkpoint:
- Retail six-action input packing and pressed-bit mirroring.
- 16-bit direction and 1/65535 analog normalization.
- CPlayer::grind sign-preserving minimum entry speed 25/27 after character multiplier.
- Grind input priority for jump, gated action, and the two deterministic trick branches.
- Retail trick windows 20..41 and 26..48 with +30 temporary acceleration frames.
- Exact 20-unit player grind acquisition AABB around the recovered query point.
