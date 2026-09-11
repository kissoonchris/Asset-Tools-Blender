# JSRF v67 — result-ZF merge correction

## v66 evidence

The v66 codegen capture finally placed the retail bytes and untouched generated
C for WX request `0x001403B0` side by side.  The request has already accepted
25 sectors (`WX+0x20 = 25`) and the host completes the full 51,200-byte read,
but the generated request method returns zero.

The first incorrect control-flow point is the alignment check around retail
`0x001404CB..0x00140508`.

Retail executes:

- `and eax, 0x800007ff`
- `jns 0x001404db`
- negative-only normalization ending in `inc eax`
- `jne 0x00140524`

and then repeats the same shape for EDI:

- `and edi, 0x800007ff`
- `jns 0x001404f1`
- negative-only normalization ending in `inc edi`
- `je 0x00140508`

For the title ADX request, the transfer length is 51,200 bytes and the file
offset is zero.  Thus the EAX masked result is zero and the first JNE correctly
falls through; the EDI masked result is also zero and the second JE must branch
to `0x00140508`, which then validates the buffer alignment and reaches the
success return at `0x00140539` (`mov eax,[esi+0x20]`).

## Translator defect

The pinned xboxrecomp translator carries a static `flag_state` tuple between
basic blocks.  v62 already taught it how to retain compatible CMP/TEST snapshots
when different predecessor instructions feed the same branch.

This WX routine exposes the same class of defect for result-based flag setters.
At `0x001404f1`, one predecessor's flags come from `and edi,0x800007ff`; the
other predecessor's flags come from `inc edi`.  Both define ZF from the same
final EDI result, but the static setter tuples differ.  The merge therefore
drops the state and the generated C emits:

`if (_flags /* je: equal / zero */) goto loc_00140508;`

`_flags` is only a fallback local initialized to zero, so that JE is always
false.  Execution falls into the error-reporting path at `0x001404f3` and
returns zero even though the request is valid.  The earlier EAX JNE at
`0x001404db` has the same lost-provenance shape.

## v67 correction

`patch_xboxrecomp_result_zf_merge_state()` extends the translator's CFG merge
rule narrowly:

- the merged block must immediately consume only ZF (`JE/JZ/JNE/JNZ`);
- every incoming state must come from a result-based arithmetic/logical setter;
- every setter must write the same register at the same operand width.

When those conditions hold, retaining any incoming result-setter state is safe
for a ZF-only branch because `_make_condition()` evaluates JE/JNE from the
current destination value (`result == 0` / `result != 0`) for all allowed
setters.  Different registers, unknown states, and non-ZF consumers continue to
fall back conservatively.

This is a generic xboxrecomp control-flow correction.  It does not hardcode the
JSRF WX return value, sector count, file result, ring state, ADXT state, title
state, or graphics state.
