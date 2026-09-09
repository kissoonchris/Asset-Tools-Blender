# JSRF v37 — 0x304F0 switch-frame stack leak

## What v36 proved

The user's v36 Windows run cleared the old `0x0007E180`/`0x00011070` return-address crash and progressed far enough for retail D3D to call `AvSetDisplayMode`:

`[AV] SetDisplayMode mode=0x0801010D format=0x00000011 pitch=1280 fb=0x001B2000`

The process did not produce the old access violation or stack-guard failure. It continued for more than 770,000 kernel calls.

## First stable corruption in v36

The first new unresolved control-flow target is:

`[ICALL] unresolved target 0x0003050F ... recent ... 0x000304F0`

Immediately afterward the ABI checker reports:

`target 0x000304F0 ... esp 00F7FEE0->00F7FDDC`

That is exactly `0x104` bytes of lost guest stack. Every higher caller then returns with the same `0x104` deficit.

## Retail CFG

Retail `0x000304F0` is one switch/state-machine routine:

```asm
000304F0  sub  esp,104h
000304F6  push esi
...
00030508  jmp  dword ptr [eax*4 + 00030618]
```

The four jump-table entries at `0x00030618` are:

- `0x0003050F`
- `0x00030592`
- `0x000305B9`
- `0x00030601`

All cases remain inside the same routine and converge on one of two epilogues:

```asm
00030606  pop esi
00030607  add esp,104h
0003060D  ret

00030610  pop esi
00030611  add esp,104h
00030617  ret
```

The table occupies `0x00030618..0x00030627`, padding follows, and the next function starts at `0x00030630`.

Independent data evidence exists at `0x001EC0F0`, which contains the literal function pointer `0x000304F0` used by retail dispatch code.

## Root cause

Generic function recovery split the switch case `0x0003050F` away from its owner. The lifted computed jump therefore became an unresolved indirect target instead of an internal CFG edge. Once the target was skipped, execution missed the common `add esp,104h` epilogue and leaked the complete local frame.

## v37 correction

v37 pins retail interval `[0x000304F0, 0x00030630)` and removes false internal function starts from the interval before lifting.

The generated-code verifier now refuses to run unless `sub_000304F0` contains both retail plain-RET exits.

No retail gameplay, D3D state, framebuffer data, allocator state, or object data is replaced.
