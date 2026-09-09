# v52: retail callback 0x521B0 omitted by generic recovery

## Runtime observation

After v51 reaches title state 12, every failed indirect dispatch targets
`0x000521B0` and returns to `0x00051FF9`. The Windows run contains 6,658
occurrences and remains alive rather than crashing.

## Retail dispatch source

`0x00051FC0` loads state `[esi+0x5C]`, bounds-checks it against `0x77`, then
loads a callback from table `0x001F9888` and calls it at `0x00051FF7`.

The retail table entry at index 7 is:

- address of entry: `0x001F98A4`
- stored pointer: `0x000521B0`

Adjacent entries include `0x52150` at index 4 and `0x52350` at index 8, which
also supports `0x52350` being the next callback boundary rather than an
internal label of `0x521B0`.

## Retail body

`0x521B0` starts with a normal frame setup:

    sub esp,0x28
    push ebx
    push esi
    mov esi,ecx

It processes entries from the object at `+0x1040`, calls resource helpers, then
finishes with:

    mov [esi+0x6c],ebx
    mov [esi+0x70],ebx
    mov dword ptr [esi+0x5c],0x8
    pop esi
    pop ebx
    add esp,0x28
    ret

The RET is at `0x52342`; `0x52343..0x5234F` are NOP padding; `0x52350` begins
the next function.

## Correction

Pin `0x521B0..0x52350` as one retail function, retain `0x51FF7` as caller
evidence and `0x1F98A4` as literal-pointer evidence, and require the emitted C
to contain the callback and its plain RET. This restores original retail code
only; it does not synthesize the post-logo transition.
