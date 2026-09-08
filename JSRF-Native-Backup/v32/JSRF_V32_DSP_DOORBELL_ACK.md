# JSRF v32 — acknowledge the retail DirectSound GP DSP command doorbell

## What the v31 Windows run established

v31 removed the worker kernel-dispatch race that had been corrupting guest ESP/TIB state. The run advanced into the millions of kernel calls without the v30 `[STACKGUARD]` trip or the older `fs:[4]` access violation. The main retail thread then stopped inside DirectSound while timer/worker activity continued.

The new blocker is a hardware handshake in the retail DSOUND section, not a guest stack failure.

## Retail command path

The retail GP command sender at `0x001A1769` resolves its command block, then does:

```text
0x001A18C1  push 3
0x001A18C3  pop  eax
0x001A18C6  add  ebx, 0x810
0x001A18CE  mov  [ebx], eax
0x001A18D0  cmp  dword ptr [ebx], 0
0x001A18D3  jne  0x001A18D0
```

So the CPU publishes command `3` at `GP command block + 0x810` and waits until the GP DSP consumes it by writing zero.

The GP object resolves its block through global descriptor `0x001BA798`. Initialization at `0x001A5FEE` supplies the first descriptor with size `0x2000`, alignment `0x1000`, protection `4`, and destination descriptor `0x001BA798`.

`0x001A5C98` calls kernel ordinal 166 (`MmAllocateContiguousMemoryEx`) with those values and stores the returned Xbox VA at `[0x001BA798]`.

The v31 Windows runtime records the matching allocation exactly:

```text
MmAllocateContiguousMemoryEx: size=8192 align=4096 -> Xbox VA 0x8037B000
```

Therefore the retail JSRF GP command doorbell is:

```text
0x8037B000 + 0x810 = 0x8037B810
```

## Existing xboxrecomp mechanism

The pinned xboxrecomp revision already contains a generic passthrough-DSP handshake model in `src/apu/apu_dsp.c`:

```text
RECOMP_APU_DSP_ACK=<guest address>[,<guest address>...]
```

On each APU frame it checks the configured guest dword and clears any non-zero command to zero. This models the observable completion handshake while the GP/EP DSP56300 cores remain stubbed. It does not fabricate DSP output data.

## v32 change

The JSRF Windows runner now supplies the proven retail address when no explicit diagnostic override is present:

```powershell
if ([string]::IsNullOrWhiteSpace($env:RECOMP_APU_DSP_ACK)) {
    $env:RECOMP_APU_DSP_ACK = "0x8037B810"
}
```

No JSRF function is replaced, no spin loop is patched, and no DirectSound HRESULT is forced. The retail code still writes command 3 and waits; the host-side DSP model now performs the hardware acknowledgement that the stubbed DSP otherwise cannot perform.

## Expected next Windows evidence

If this was the only blocker at that point, the main retail thread should return from the `0x001A18D0` wait and continue DirectSound/game initialization. If it stops again, the next runtime log should identify the next real dependency rather than repeating the same GP doorbell spin.
