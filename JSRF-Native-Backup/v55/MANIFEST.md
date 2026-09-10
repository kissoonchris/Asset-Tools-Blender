# JSRF Native v55

## Purpose

The corrected v54 Windows run materially improves presentation: the post-logo text is now readable instead of opaque white blocks. The screen is not a menu, however. It is JSRF's retail dirty-disc error: `There's a problem with the disc you're using. It may be dirty or damaged.`

v55 does not suppress that message. It instruments the one retail helper that displays it so the next run proves which real watchdog/error path requested the screen and exposes the state that caused it.

## Retail proof

The supplied retail `default.xbe` contains the localized English message at VA `0x001CB430`. Function `0x0006F730` indexes the English message table at `+0x2C` (entry 11) and passes that string to the existing UI renderer.

A full direct-control-flow scan of retail `.text` finds exactly four direct transfers to `0x0006F730`:

- `0x0002537E`: tail `JMP 0x0006F730` after a 15,000,000-tick watchdog expires;
- `0x000255AD`: `CALL 0x0006F730`, return `0x000255B2`, while a file/cache operation remains pending beyond the same timeout;
- `0x000664C3`: `CALL 0x0006F730`, return `0x000664C8`, after a phase watchdog is latched and `0x000257B0` reports result >= 2;
- `0x00116EA8`: `CALL 0x0006F730`, return `0x00116EAD`, when any of four streaming/media objects reports a non-zero signed 16-bit status at `+0x60`.

Retail `0x0013D300` can set that stream status to `0xFFFE` (-2) for a stalled-progress watchdog or `0xFFFF` (-1) for an output/fullness watchdog. This establishes possible sources, not which one v54 actually hit.

## v55 change

Generated retail helper `sub_0006F730` receives one read-only call at its entry: `jsrf_dirty_disc_probe(0x0006F730u);`

The callback reads the guest return address before the helper changes ESP. Pinned xboxrecomp preserves real guest return addresses for direct calls, so three paths are identified exactly by `0x255B2`, `0x664C8`, or `0x116EAD`. A different return address is reported as the `0x2537E` tail-JMP candidate or an unknown indirect caller rather than being forced into a direct-call bucket.

`[DISCDIAG]` reports the relevant file/cache, phase-watchdog, or four-stream status fields. The callback writes no guest register, memory, title state, IO result, or stream status. v54's NV2A alpha/blend/flip corrections are preserved unchanged. Runtime startup identifies this diagnostic build as `JSRF Native v55`.

## Test instruction

Extract the v55 easy-test ZIP into a fresh folder and run `START JSRF TEST.bat`. Let it reach the dirty-disc screen once, then close the framebuffer window normally so logs flush. Return the generated `JSRF_TEST_LOG` and `JSRF_RUNTIME_LOG` files. The decisive evidence is the first `[DISCDIAG] source=...` line and, for the stream path, the four following `[DISCDIAG] stream[...]` lines.

## Verification

The new v55 tests were written first and failed against the v54 baseline. After the minimal diagnostic implementation: focused v55 regression 5/5 passed; full Python suite 193 passed; Python compile check passed; fresh CMake/Ninja native build passed; native CTest 20/20 passed. Windows behavior beyond the reported v54 dirty-disc screen remains intentionally unclaimed until the v55 diagnostic run is returned.
