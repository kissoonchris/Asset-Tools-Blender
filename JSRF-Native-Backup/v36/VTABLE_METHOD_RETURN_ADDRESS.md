# JSRF v36 — vtable method return-address leak at 0x0007E180

## What v35 proved

v35 kept the exact retail boundaries for `0x00011070` and `0x00025040`, and its generated-code verifier confirmed their retail plain-`ret` exits were present before the Windows executable ran.

The Windows run still failed, but the ordering changed the diagnosis. Immediately before `0x00011070` reports corruption, the ABI checker reports:

- `0x0007E180`: ESP `0x00F7FEE8 -> 0x00F7FEE8`
- `0x00011070`: ESI `0x01080FA0 -> 0x0001108A`, ESP `0x00F7FEF0 -> 0x00F7FEF0`

The unchanged ESP at `0x0007E180` means that lifted method returned to native C without consuming the synthetic guest return address that xboxrecomp pushed for the virtual call.

## Why 0x0001108A is decisive

Retail `0x00011070` contains this call sequence:

```asm
00011083  mov eax,[esi]
00011085  mov ecx,esi
00011087  call dword ptr [eax+4]
0001108A  mov ecx,[esi+28]
```

`0x0001108A` is therefore exactly the return address of the object's vtable slot `+4` method.

The v35 log then shows ESI becoming `0x0001108A`. That is what happens if the virtual method leaves its synthetic return address on the guest stack: `0x00011070` eventually executes its own `pop esi` and pops the leaked `0x0001108A` instead of the ESI value it saved at entry.

The later null virtual call and the final bad pointer in `sub_00011070` are downstream of that first stack error.

## Retail boundary and vtable evidence

The retail XBE proves `0x0007E180` is a standalone vtable method:

```asm
0007E180  push esi
0007E181  mov  esi,ecx
...
0007E195  pop  esi
0007E196  ret
...
0007E240  pop  esi
0007E241  ret
...
0007E255  pop  esi
0007E256  ret
```

`0x0007E257..0x0007E25F` is padding, and the next function begins at `0x0007E260`.

The constructor at `0x0007E2A4` installs vtable `0x001CCF78`. The retail dword at vtable slot `+4`, address `0x001CCF7C`, is exactly `0x0007E180`. The runtime independently reaches `0x0007E180` through the virtual call at return address `0x0001108A`.

## v36 correction

v36 pins the complete interval:

`[0x0007E180, 0x0007E260)`

with both runtime caller evidence `0x0001108A` and retail vtable evidence `0x001CCF7C`. Internal recovered starts inside the method are removed before code generation.

The generated-code verifier now requires `sub_0007E180` to retain all **three** retail plain-`ret` exits. The Windows build stops before launch if code generation loses any of them.

No JSRF gameplay, object data, DirectSound behavior, or render behavior is replaced. This is only a retail function-boundary/code-generation correction.
