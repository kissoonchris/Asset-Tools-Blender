# JSRF Native v61 — CRI ADXT state-machine boundary correction

## v60 runtime evidence

The v60 Windows run removes the ambiguity around the title-music failure. `title.adx` still reaches the ADXT object at `0x0027D120`, then the retail low-output watchdog records status `-1` (`0xFFFF`). The decisive detail is the counter state at the exact retail `0x0013D3F5` write: `stall6a == 1` while ADXT `+0x38 == 60`.

That state is impossible in the original retail control flow. Retail `0x0013D3D2..0x0013D3F5` only reaches the `-1` write after `+0x6A` exceeds `5 * +0x38` for state 3, or `20 * +0x38` for the other active states. With `+0x38 == 60`, the minimum legal trigger is therefore 301 or 1201, never 1. The generated control flow is entering an internal watchdog destination as if it were an independent function/block boundary.

v60 also instruments every generated entry/block in the retail `0x0013D080..0x0013D2FF` range. The run observes only the `0x0013D250` cleanup body after the error; it never observes the state-2 updater entry `0x0013D080`. Retail dispatcher `0x0013D470` directly calls `0x0013D080` when ADXT state byte `+1` is 2, so allowing the detector to merge/split this cluster can suppress the actual decode/output update while leaving the watchdog alive.

## Retail XBE boundary proof

The supported retail XBE shows one contiguous CRI ADXT state-machine cluster made of seven independent functions:

- `0x0013CE30..0x0013D080` — state-1 setup/update; writes state 2 at `0x13D06C`
- `0x0013D080..0x0013D1A0` — state-2 decoder/output update
- `0x0013D1A0..0x0013D210` — state-3 update
- `0x0013D210..0x0013D250` — state-4 update
- `0x0013D250..0x0013D300` — mode-2 cleanup
- `0x0013D300..0x0013D470` — error/progress watchdog
- `0x0013D470..0x0013D4E0` — per-object state dispatcher

The boundaries are RET/tail-jump plus padding delimited or direct-call delimited. The dispatcher has direct calls to the state-specific bodies, and the ADXT task loop at `0x0013AA16` directly calls the dispatcher.

## v61 correction

v61 pins all seven measured retail intervals as indivisible function bodies and adds their starts to the runtime seed set. Internal destinations such as `0x0013D3C6` and `0x0013D3F5` are removed if generic recovery emitted them as standalone function starts. This restores the retail branch ownership instead of bypassing the watchdog or forcing its result.

The existing read-only `[ADXTDEC]`, `[ADXTRING]`, `[ADXTERR]`, and `[DISCDIAG]` probes remain available. With `0x0013D080` pinned, `[ADXTDEC]` can now observe the real state-2 updater rather than only the post-error cleanup body.

No ADXT status, counter, ring fill level, file result, audio data, dirty-disc branch, or title state is forced by v61.