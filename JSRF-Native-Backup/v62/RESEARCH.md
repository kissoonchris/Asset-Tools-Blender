# JSRF Native v62 — merged CMP flag-state correction

## Runtime proof from v61

The v61 Windows capture reaches the same CRI ADXT watchdog in retail function
`0x0013D300`. Immediately before the error write, the output-availability call
returns below `0x40`, so retail increments watchdog word `ADXT+0x6A` from 0 to 1.
The diagnostic at `0x0013D3F5` then records:

- `stall6a = 1`
- base threshold `ADXT+0x38 = 60`
- mode/state byte `ADXT+0x6D = 2`
- `edx = 1`
- `ecx = 0x000004B0` (1200)

For state != 3, the retail instructions at `0x0013D3E8..0x0013D3F3` compute
`ecx = threshold * 20`, load the signed watchdog count into `edx`, execute
`cmp edx, ecx`, then `jle 0x0013D3FB`. With the measured values, `1 <= 1200`
is true. Retail execution therefore cannot fall through to the `mov word
[esi+0x60], 0xffff` at `0x0013D3F5` on that first watchdog increment.

## Translator defect

The pinned xboxrecomp translator propagates a Python `flag_state` tuple between
basic blocks. At a CFG merge it keeps that state only when every predecessor's
static tuple is exactly identical. The shared JLE at retail `0x0013D3F3` has two
legitimate predecessors:

- state 3: `cmp eax, edx`
- other states: `cmp edx, ecx`

Those tuples differ, so the translator discards the incoming state. The lifter's
standalone-Jcc fallback then tests local `_flags`, which is initialized to zero
and is not populated by ordinary CMP. The generated JLE is consequently false
regardless of the runtime operands. That exactly matches the v61 observation.

## v62 correction

`patch_xboxrecomp_merged_cmp_flag_state()` changes the pinned translator's merge
rule only for compatible runtime snapshots:

- all incoming states must be CMP or all must be TEST;
- all must have the same operand width;
- every predecessor still has to have a known outgoing state.

CMP/TEST already emit `_fa/_fb/_fas/_fbs` at the point they execute. Therefore
at runtime those locals contain the operands from the predecessor that was
actually taken. Keeping a representative CMP/TEST provenance at the merge lets
`_make_condition()` consume those runtime snapshots instead of the dead
`_flags` fallback. Different setters, different widths, unknown predecessors,
and back-edge uncertainty retain the previous conservative behavior.

This is a generic xboxrecomp control-flow correction. It does not modify JSRF's
ADXT status, watchdog count, threshold, decoded samples, ring accounting, file
I/O results, title state, or graphics output.
