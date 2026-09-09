# JSRF v51 - packed 16-bit NV2A clear colour

The observed Smilebit background colour provides a direct graphics-semantic
fingerprint.

JSRF runtime evidence:
- colour surface: `0x00084000`
- pitch: 1280 bytes for 640 pixels => 2 bytes/pixel
- surface-format parameters include `0x00000113` / `0x00000123`; low colour
  nibble is `3`, the NV097 R5G6B5 format
- a later retail clear value is `0x0000FFFF`

Pinned xboxrecomp behaviour before v51:

```c
uint16_t v = (uint16_t)(((s_gpu.clear_color >> 8) & 0xF800)
                      | ((s_gpu.clear_color >> 5) & 0x07E0)
                      | ((s_gpu.clear_color >> 3) & 0x001F));
```

That treats `0x0000FFFF` as 32-bit RGB `(0,255,255)`, hence cyan.

NV2A/xemu reference behaviour for R5G6B5 instead extracts R/G/B directly from
the packed clear register bits: R=`bits 11..15`, G=`bits 5..10`, B=`bits 0..4`.
Thus `0xFFFF` is white.

v51 preserves the packed register value for 16-bit targets:

```c
uint16_t v = (uint16_t)s_gpu.clear_color;
```

This is an evidence-based executor correction, not a title-specific replacement
colour.
