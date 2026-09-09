# JSRF v39 — post-SEGA title-state switch CFG boundary

## What v38 proved

v38 is the first build to show a real retail JSRF image in the native Win32
framebuffer window: the **Presented by SEGA** screen. The runtime also reports
hundreds of real clears/draws and textured batches from the title's NV2A
pushbuffer. The screen then turns black while the process stays alive.

## First stable post-logo failure

The v38 runtime log eventually reports:

- unresolved indirect target `0x0002E0CD`;
- the immediately owning target `0x0002DBE0` returning with guest ESP changed
  from `0x00F7FEE0` to `0x00F7FDD4` — exactly `0x10C` bytes;
- later `0x00011070` inherits the damaged frame and reaches a null virtual call.

That makes the first causal break `0x0002DBE0`, not the later null call.

## Retail XBE proof

Direct disassembly of the supported retail XBE shows `0x0002DBE0` is one switch
routine:

- `sub esp, 0x100` at entry;
- saves EBX, EBP, ESI and EDI;
- dispatches through the 11-entry jump table at `0x0002E13C`;
- case `0x0002E0CD` is inside that table;
- eight genuine plain-RET exits restore the frame;
- `0x0002E170` is the next real function start after the jump-table data/padding.

The `0x10C` observed runtime loss matches the routine's `0x100` local frame plus
its saved-register/call-frame cleanup footprint. A literal pointer to
`0x0002DBE0` is independently present at retail data VA `0x001EC154`.

## Root cause

Generic CFG recovery split internal jump-table case `0x0002E0CD` out of
`sub_0002DBE0`. Entering that case as if it were an independent translated
function bypasses the owning routine's frame discipline, leaving the caller
`0x10C` bytes below its expected ESP and corrupting subsequent object traversal.

## v39 correction

Pin the retail function interval as one indivisible body:

`0x0002DBE0 .. 0x0002E170`

The boundary pinner removes all eleven internal switch-case starts if generic
recovery emitted them separately. A generated-code contract also requires all
eight retail plain-RET exits before the Windows build is allowed to run.

This does not patch JSRF's title flow, skip the switch, or synthesize graphics.
It only prevents the recompiler from splitting a retail function at an internal
jump-table destination.
