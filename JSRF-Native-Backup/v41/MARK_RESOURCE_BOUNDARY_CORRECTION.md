# JSRF v41 — correct the v40 mark-resource function cluster

## v40 Windows result

v40 removed the v39 `0x0002D1F8` split, but the next Windows run exposed a
more precise problem in the boundary assumption used to do that.

Immediately before the crash the retail path attempts two unresolved indirect
calls:

- `0x0002D460` from return site `0x000256EA`;
- `0x0002D240` from return site `0x000253B7`.

The failure then propagates through ABI violations in `0x00025390`,
`0x000256A0`, and `0x0007E180`, reaches a null virtual call from `0x0001108A`,
and finally faults in `sub_00011070` while dereferencing a corrupted object
pointer. `DMA_PUT` and `DMA_GET` are equal at the failure, so this is not a GPU
progress stall.

The easy tester attempted automatic callback promotion, but the two targets
remained unresolved on later passes. v40 itself explains why: it pinned the
entire interval `0x0002D080..0x0002DBE0` and removed every function start inside
it after the seed pass.

## Why v40's interval was too broad

v40 used the public symbol names `readMarkDefault @ 0x0002D080` and
`readMarkPressOrTex @ 0x0002DBE0` as if they were adjacent physical functions.
They are adjacent *named* functions in that symbol source, but the exact retail
XBE contains several unnamed methods between them.

Direct disassembly of the supported retail XBE
SHA-256 `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`
proves the split:

- `0x0002D080` begins `sub esp, 0x100` and dispatches through a five-entry jump
  table at `0x0002D224`; `0x0002D1F8` is the fifth case. The body has four
  plain-RET exits and finishes at `0x0002D220`, followed by its table/padding.
  The next executable body begins at `0x0002D240`.
- `0x0002D240` begins with its own `push ebx; push ebp; push esi` prologue,
  has a separate seven-entry switch table at `0x0002D438`, and exits with
  `ret 4` at `0x0002D435`. Padding ends immediately before `0x0002D460`.
- `0x0002D460` is an independent `mov eax,[ecx+0x188c]; ret 4` method.
- `0x0002D470` is an independent `lea eax,[ecx+0x1888]; ret 0x0c` method.

Retail `.data` independently stores the first four entries as literal function
pointers:

| data VA | function VA |
|---|---|
| `0x001EC158` | `0x0002D080` |
| `0x001EC0CC` | `0x0002D240` |
| `0x001EC1DC` | `0x0002D460` |
| `0x001EC264` | `0x0002D470` |

The remaining padding-delimited functions in the cluster are reached by direct
retail calls, which supplies independent entry evidence:

| function interval | direct caller evidence | retail return exits |
|---|---|---|
| `0x0002D480..0x0002D570` | call `0x0002D58E` | 1 × plain RET |
| `0x0002D570..0x0002D8D0` | call `0x0002DD62` | 4 × `ret 4` |
| `0x0002D8D0..0x0002D950` | call `0x0002DEC1` | 2 × plain RET |
| `0x0002D950..0x0002D9D0` | call `0x0002E0C2` | 2 × plain RET |
| `0x0002D9D0..0x0002DA70` | call `0x0002DC5A` | 2 × `ret 4` |
| `0x0002DA70..0x0002DBE0` | call `0x0002DE48` | 2 × `ret 8` |

`0x0002DBE0` remains the separately proven v39 function beginning the next
cluster.

## v41 correction

v41 replaces the single over-broad `0x0002D080..0x0002DBE0` record with ten
retail-proven adjacent function intervals. This keeps genuine switch targets
such as `0x0002D1F8` and `0x0002D30B` inside their owners while preserving real
callable entries such as `0x0002D240` and `0x0002D460`.

Generated-code verification now checks the measured return cleanup for all ten
functions before the Windows runtime may launch. In particular, it requires
`0x0002D240` and `0x0002D460` to retain `ret 4`, `0x0002D470` to retain
`ret 0x0c`, and `0x0002DA70` to retain both `ret 8` exits.

No guest return value, resource result, title-state decision, framebuffer
content, GPU command, or kernel result is replaced or bypassed.
