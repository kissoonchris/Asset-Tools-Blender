# v59 ADXT ring-flow research

The v58 HOTFIX1 runtime capture showed the shared-resource dispatcher reaching `busy=0`, followed by `D:\Media\Z_ADX\BGM\title.adx` opening. ADXT object `0x0027D120` points to output object `0x00277180`, with vtable `0x0022DB38`. Immediately before the retail error write, the vtable `+0x24` call returns zero; retail then increments watchdog `+0x6A` and writes status `0xFFFF` (`-1`).

Retail XBE disassembly identifies the output-ring vtable helpers as:

- `+0x18 -> 0x0013F9E0`
- `+0x1C -> 0x0013FBC0`
- `+0x20 -> 0x0013FAD0`
- `+0x24 -> 0x0013F9A0`
- `+0x28 -> 0x0013FCA0`

`0x0013F9A0` returns ring counts and is already observed indirectly by the v58 `[ADXTERR]` probe. v59 therefore instruments the other reserve/commit/span helpers at their true generated entry labels. The next Windows run can distinguish whether decoded audio never reserves output space, reserves but never commits, or commits and later accounting/consumption erases the produced count.

The visual text distortion remains a separate NV2A/software-rasterizer issue and is intentionally unchanged in this revision.
