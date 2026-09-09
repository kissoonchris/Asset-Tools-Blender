# JSRF Native v42 manifest

## Purpose

v42 addresses the two concrete unresolved retail virtual-method entries exposed
by the user's v41 Windows run and fixes the framebuffer-window close race that
could immediately create a replacement window.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v41 Windows evidence

The v41 run is materially different from the earlier crash path:

- the genuine `Presented by SEGA` framebuffer remains visible;
- the GPU command stream continues advancing for thousands of rendered clears
  and triangles instead of stopping at the earlier mark-loader corruption;
- exactly two unresolved indirect targets are reported in the measured run:
  `0x0003E230` from return site `0x0002FE4D`, then `0x00069F90` from return site
  `0x0003003F`;
- execution then repeatedly checks the retail cache-completion files while the
  GPU continues consuming pushbuffer work;
- no replacement game result, framebuffer, or title-state decision is injected.

## Exact retail-XBE proof

The supported retail XBE SHA-256 remains:

`fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`

Direct analysis of that exact image proves both runtime targets are genuine
callable entries:

### `0x0003E230 .. 0x0003E3C0`

- starts after padding with its own `sub esp, 8` prologue;
- has one `ret 0x0C` exit at `0x0003E3B3`;
- is padded through `0x0003E3BF`, with the next function beginning at
  `0x0003E3C0`;
- retail vtable slot `0x001CA494` contains literal pointer `0x0003E230`.

### `0x00069F90 .. 0x0006A100`

- starts after padding with its own `sub esp, 8` prologue;
- has two `ret 0x0C` exits at `0x0006A042` and `0x0006A0FA`;
- is padded through `0x0006A0FF`, with the next function beginning at
  `0x0006A100`;
- retail vtable slot `0x001CC130` contains literal pointer `0x00069F90`.

v42 adds both entries to the runtime seed set and pins their exact retail
function boundaries. Generated-code verification refuses to launch Windows if
those `ret 0x0C` stack-cleanup contracts are lost.

## Framebuffer close correction

The pinned xboxrecomp framebuffer presenter used `s_fb_running` for both
"window worker is alive" and "a new worker may start". `WM_CLOSE` cleared that
flag before the old worker had completely torn down, so a concurrent display
update could call `xbox_FramebufferWindowStart()` and create another HWND.

v42 stages a small runtime-source correction before building:

- a separate `s_fb_user_closed` latch prevents recreation;
- `WM_CLOSE` records the explicit user close and exits the diagnostic runtime
  with code 5;
- the PowerShell wrapper recognizes code 5 and preserves the normal diagnostic
  flow instead of leaving a replacement framebuffer window running.

This affects only the PC diagnostic window lifecycle; it does not change retail
JSRF rendering or title logic.

## Regression coverage

`tests/test_v42_runtime_vtable_seeds.py` covers:

1. both measured v41 targets are present in the runtime seed set;
2. both exact retail boundaries and independent vtable-pointer evidence;
3. removal of false internal starts without swallowing the adjacent functions;
4. generated `ret 0x0C` cleanup for both methods;
5. rejection of incorrect stack cleanup;
6. framebuffer user-close latching and duplicate-window prevention;
7. clean diagnostic termination on framebuffer close.

## Fresh local verification

- Focused v42 regression: **9/9 passed**
- Full Python suite: **125 passed**
- Fresh native CMake/Ninja configure/build: **passed**
- Native CTest suite: **20/20 passed**
- Exact retail XBE SHA-256: **verified**
- Retail vtable pointers at `0x001CA494` and `0x001CC130`: **verified**
- Retail return contracts: `0x3E3B3 -> ret 0x0C`, `0x6A042/0x6A0FA -> ret 0x0C`: **verified**

Windows runtime confirmation is still required before claiming the title has
advanced past the SEGA/cache-loading state.
