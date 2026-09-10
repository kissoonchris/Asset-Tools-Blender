# JSRF Native v58 — CRI ADXT error watchdog diagnostic

## v57 Windows evidence

The v57 Windows run does not repeat the old shared-resource timeout. The common retail dirty-disc helper `0x0006F730` is entered with return address `0x00116EAD`, which identifies the direct call at `0x00116EA8` inside the title's stream/audio manager. EAX is `0xFFFFFFFF`, so the immediately preceding stream status getter returned -1.

The same run reaches shared dispatcher `busy=0 kind=14 param=1`, opens `\Device\CdRom0\Media\Z_ADX\BGM\title.adx` successfully, and completes its first 51,200-byte read with all 51,200 bytes and status success. This rules out treating the visible message as evidence of a failed host file open/read.

## Exact retail XBE proof

Supported retail XBE SHA-256: `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`.

Direct disassembly shows `0x0013AA50` is a three-instruction status getter: load its ADXT argument, sign-extend word `[object+0x60]`, return. The stream manager loops over four ADXT handles and calls the dirty-disc helper whenever that getter is nonzero.

The ADXT state updater at `0x0013D300` has two explicit error writes. The one matching the observed -1 is `0x0013D3F5`, which writes `0xFFFF` to ADXT `+0x60` after output/fullness counter `+0x6A` exceeds its subtype-dependent threshold. Immediately before that counter is advanced, the updater calls the object at ADXT `+0x14` through vtable slot `+0x24` and compares its return against `0x40`. Returns below `0x40`, or a null output object, drive the watchdog toward -1.

The earlier helper probe reads all four ADXT `+0x60` fields as zero after the -1 return has already reached the common error helper. That can be explained by a concurrent clear, but it also leaves translation of the tiny getter as a question. v58 therefore probes both ends rather than assuming either explanation.

## v58 instrumentation

`[ADXTERR]` is injected at three retail points without changing guest memory or registers: `0x0013AA50` getter entry; `0x0013D3C9` immediately after the output vtable `+0x24` call and stack cleanup while EAX still contains the call return; and `0x0013D3F5` immediately after retail writes error -1.

The host observer logs ADXT pointer, signed/raw error, `+0x38` threshold, `+0x64` progress, `+0x68/+0x6A` watchdog counters, `+0x6D/+0x72` mode bytes, ADXT `+0x14` output object, its vtable and slot `+0x24` target, and the captured call result. Low-output logging is rate-limited to avoid recreating timing perturbation from old broad traces.

No file result, ADXT error code, timeout threshold, title state, or dirty-disc branch is changed.