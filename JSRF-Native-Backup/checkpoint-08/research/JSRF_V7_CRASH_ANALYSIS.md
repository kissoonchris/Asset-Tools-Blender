# JSRF v7 native crash analysis

Observed from the 2026-09-06 Windows v7 run after the manual retail memmove replacement was enabled.

## Progress confirmed

- The old unresolved ICALL to `0x0017D15C` is gone. That address is an internal switch-case label in retail `memmove` (`0x0017CEC0`), not a function entry.
- Runtime reaches 476 indirect calls and six post-CRT heap allocations before the access violation.

## Crash signature

The exception reports:

- guest `ECX = 0x7559C085`
- read fault at guest VA `0x7559C0A5`
- difference `+0x20`
- guest `ESI = 0x0017C937`
- guest return address at the top of the stack = `0x0017C937`

Retail code at `0x0014B800` begins with reads from `[ecx]`, `[ecx+4]`, `[ecx+8]`, then `[ecx+0x20]`. The fourth read exactly matches the exception, so the fault is localized to this SSE geometry helper being reached with a bad structure pointer.

There is a stronger clue: the four retail bytes at guest VA `0x0017C937` are `85 C0 59 75`, which as little-endian data are exactly `0x7559C085`, the value in ECX at the crash. `0x0017C937` is the return address immediately following `call 0x0017C900` at `0x0017C932`. Thus the bad pointer is literally instruction bytes from the current return site, not an arbitrary heap address.

Immediately before the crash xboxrecomp reports bad indirect targets:

- null target, beginning at ICALL #447 and recurring through #474
- `0x89E85596` at ICALL #476

The old logger grouped by target only and did not record the guest return address, so the exact call site that produced those bad pointers is missing.

## Next diagnostic

The next build records the guest return address and register state for every unique `(bad target, return address)` pair, enables xboxrecomp ABI checks, keeps MSVC PDB information in the Release build, and reports the host RIP symbol and source line from DbgHelp. This is evidence gathering only; no speculative game logic or kernel stubs are added.
