# JSRF Native checkpoint 03 — repaired

The original `JSRF-Native-Source-Checkpoint-03.zip` upload was truncated before the ZIP central directory and was corrupt.

This repaired archive is a valid recovery payload containing the preserved checkpoint-03 Garage visual/texture decoder source:
- `src/stage_visual.cpp`
- `src/stage_visual.h`
- `src/stage_textures.cpp`
- `src/stage_textures.h`

The current JSRF source has progressed beyond checkpoint 03, so later collision/grind integration is backed up separately.

Repaired archive SHA-256: `c29d4a2417a1f6aa77cff6729142d037525357e5038f40fb639662ce460a22bc`
Repaired archive size: 5,085 bytes.

Local verification before upload: `unzip -t` reported no errors. Current preserved working tree also passes 20/20 CTest tests against the extracted retail Media files.

No retail Media files or `default.xbe` are included in this archive.
