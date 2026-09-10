# JSRF v54: NV2A alpha/blend compositing

## Runtime evidence from v53

The supplied v53 Windows run reaches title state 12 and keeps executing a large
retail NV2A pushbuffer after the startup logos. The framebuffer window is no
longer simply black: the returned screenshot contains large opaque white/blocky
regions on black.

The final v53 GPU inventory records a 640x480 surface with 1280-byte pitch at
`0x00084000`, with 7,298 draws, 17,097 rasterised triangles, and 7,223 textured
batches. The same inventory records frequent fixed-function state changes:

- `0x0300` SET_ALPHA_TEST_ENABLE: 4,853
- `0x0304` SET_BLEND_ENABLE: 9,646
- `0x0344` SET_BLEND_FUNC_SFACTOR: 2,417
- `0x0348` SET_BLEND_FUNC_DFACTOR: 2,417
- `0x012C` FLIP_INCREMENT_WRITE: 2,424
- `0x0130` FLIP_STALL: 2,424

A verbose draw captured in the supplied log also reaches the software textured
triangle path with a valid 640x480 texture/surface setup. This is evidence that
the remaining post-logo corruption is downstream of basic pushbuffer execution
and pixel presentation, not evidence that all drawing stopped.

## Executor mismatch

The pinned xboxrecomp software executor samples an ARGB texel, then writes that
value straight to the destination in `put_pixel`. For a 16-bit R5G6B5 surface,
the conversion necessarily discards source alpha. Before v54, the executor also
does not preserve the NV097 alpha-test enable/function/reference or blend
enable/factors/equation/color that JSRF submits repeatedly.

Therefore transparent and partially transparent sampled texels are treated as
ordinary opaque writes by this limited software path. That is a concrete
semantic mismatch between the observed retail command stream and the executor.
It is a plausible cause of the white/blocky result, but the Windows v54 run is
still required to determine how much of the visible corruption it explains.

## v54 correction

v54 adds the missing fixed-function alpha/blend state to the software executor:

- alpha test is evaluated before the destination write;
- standard NV097 source/destination blend factors are evaluated componentwise;
- ADD, SUBTRACT, REVERSE_SUBTRACT, MIN and MAX equations are handled;
- R5G6B5 destination pixels are expanded for blending and repacked afterward;
- bounded `[GPUBLEND]` state snapshots preserve the exact runtime combinations
  for the next returned log.

The texture sample itself is deliberately left unchanged. v54 does **not**
assume a texture-times-diffuse combiner or invent another texture operation,
because JSRF's register-combiner program is not decoded by this software
rasterizer yet. This keeps the correction limited to state whose retail methods
are directly observed.

v54 also ports upstream xboxrecomp's five NV097 flip-state cases to the pinned
revision and adds bounded `[GPUFLIP]` snapshots. The pushbuffer survey names are
corrected so `0x0120..0x0130` and `0x033C..0x035C` are no longer mislabeled in
future logs.

## Boundary of the claim

This is not a full NV2A renderer. Vertex programs, register combiners,
depth/stencil and general 3D scene rendering remain outside the current
software-rasterizer coverage. v54 is an evidence-driven test of the alpha/blend
and flip semantics already present in the supplied retail command stream. A
fresh Windows run is required before claiming that the post-logo visual defect
is fixed.
