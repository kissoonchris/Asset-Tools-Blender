# JSRF v55 dirty-disc source diagnostics

The v54 Windows screenshot makes the retail error readable: `There's a problem with the disc you're using. It may be dirty or damaged.` The message is selected by retail helper `0x0006F730` (English table entry `+0x2C`).

A complete direct-transfer scan of the supplied retail `.text` finds only four direct paths to that helper:

```
0002537E  jmp  0006F730
000255AD  call 0006F730   ; return 000255B2
000664C3  call 0006F730   ; return 000664C8
00116EA8  call 0006F730   ; return 00116EAD
```

`0x2537E` follows a 15,000,000-tick watchdog around a table-dispatched operation. `0x255AD` is another 15,000,000-tick timeout while a file/cache object remains pending. `0x664C3` fires after a phase watchdog is latched and `0x257B0` returns >=2. `0x116EA8` loops four media/stream objects and fires when `sub_0013AA50` returns a nonzero signed 16-bit status from stream `+0x60`.

Retail `0x0013D300` has two explicit status writes: `0xFFFE` (-2) when stream progress remains unchanged beyond `5 * object[+0x38]`, and `0xFFFF` (-1) when its output/fullness watchdog counter exceeds its threshold.

v55 injects one read-only observer at `loc_0006F730` before the helper changes ESP. Pinned xboxrecomp preserves direct-call guest return addresses, so `[DISCDIAG]` can distinguish the three CALL sites exactly. A nonmatching return is reported as the `0x2537E` tail-JMP candidate or an unknown indirect caller. The probe also logs the relevant pending/file fields, phase-watchdog fields, or all four stream statuses/progress/counters/thresholds.

No retail state or result is modified. The next Windows run is diagnostic evidence only; it does not bypass the dirty-disc path.
