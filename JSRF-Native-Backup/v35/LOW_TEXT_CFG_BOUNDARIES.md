# JSRF v35 — low-.text CFG boundary recovery

## What v34 proved

The corrected DirectSound DSP mailbox is now confirmed. Every v34 pass reports:

`[APU] DSP doorbell 0x803C0810: command 0x00000003 acknowledged (async)`

So the DirectSound GP mailbox wait that blocked v33 is no longer the active failure.

The easy-test callback feedback also discovered two real retail indirect-call targets during the first two passes:

- `0x00173DB0`, first reached from return `0x00173643`
- `0x00175300`, reached from return `0x00178594`

Both were independently validated by the automatic retail-pointer check and allowed the next pass to continue. v35 keeps them in `recomp/jsrf_runtime_seeds.json` from the start.

## Stable v34 failure

After those callbacks were available, PASS2 and PASS3 converged on the same failure sequence:

1. `0x00025040` violates the guest ABI: its ESP returns eight bytes below the ABI-call entry.
2. `0x00011070` immediately violates the ABI: ESI is cleared and ESP again returns eight bytes below its ABI-call entry.
3. `0x00011070` then reaches a node value `0x0FE80022`, reads a null vtable target, and the safe ICALL path skips it.
4. The higher-level caller later invokes `0x00011E40` with corrupted ECX/EDI `0xD80C2444`, causing the access violation at `edi+4`.

The final fault in `0x00011E40` is therefore downstream. The first stable evidence is lost callee-save/stack cleanup in the two earlier low-.text methods.

## Retail boundary: 0x00011070

Retail code is one contiguous traversal routine at `0x00011070..0x0001109F`, with the next function beginning at `0x000110A0`.

It starts with `push esi`, walks a node tree/list, recursively calls itself through node `+0x28`, follows sibling/next through `+0x30`, and exits with `pop esi; ret`.

There is a direct retail call at `0x000124BE`, so the entry has independent call evidence.

v34 generated code nevertheless returned with ESI changed from `0x01080FA0` to zero and ESP `0x00F7FEF0 -> 0x00F7FEE8`. v35 pins the exact retail interval `[0x00011070, 0x000110A0)` so generic recovery cannot merge/split away its epilogue.

## Retail boundary: 0x00025040

`0x00025040` is a switch/state-machine virtual method. Retail code starts:

```asm
00025040  push esi
00025041  mov  esi,ecx
00025043  mov  eax,[esi+54]
00025046  cmp  eax,7
00025049  ja   000252B7
0002504F  push ebx
00025050  push edi
00025051  jmp  dword ptr [eax*4 + 000252BC]
```

The eight-entry jump table occupies `0x000252BC..0x000252DB`, followed by padding; the next function begins at `0x000252E0`. Treating internal switch destinations such as `0x25078`, `0x2515E`, or `0x25263` as separate function starts destroys the push/pop ownership of ESI/EBX/EDI and explains the v34 ESP mismatch.

The retail constructor at `0x00024C90` installs vtable `0x001C4D58`. Vtable slot `+4` at `0x001C4D5C` is the literal pointer `0x00025040`, and the v34 runtime independently reached `0x00025040` as a resolved indirect target.

v35 pins `[0x00025040, 0x000252E0)`, including its computed jump table.

## Build-time contract guard

v35 extends `verify_jsrf_generated_contracts()` so the Windows build refuses to run if code generation does not preserve:

- exactly one plain `ret` exit in `sub_00011070`
- all nine retail plain `ret` exits in `sub_00025040`

This turns the v34 silent ABI corruption into a build-time diagnostic if the pinned CFG is ever weakened again.

## Scope

No retail JSRF gameplay, file, DirectSound, D3D, allocator, or object data is replaced. v35 only supplies retail-proven function boundaries/callback seeds to the static recompiler and validates the generated cleanup contracts.
