# JSRF v69 — XECI completion handoff evidence

## Proven by the v68 Windows run

The request-side defect fixed in v67 remains corrected. Retail WX request
`0x001403B0` returns 25 accepted sectors and the CRI wrapper preserves that
value. The host then reads all 51,200 requested bytes from `title.adx`.

The v68 `[WXSTAT]` probe changes the location of the investigation. The getter at
`0x001405B0` simply returns the signed state byte at object +1, and runtime
samples consistently return `2`. The first WX object itself remains in state 2
with current sector position 0, request count 25, transfer length 51,200, and
both +0x148/+0x14C bookkeeping words zero after the host read. Thus the bad
state is upstream of the getter: completion is not being promoted into object
progress/source-ring data.

The producer ring stays at zero filled bytes until ADXT reaches its real 1201
stall threshold. The v68 run continues into the title's fatal/error path and is
then closed by the user; the earlier stack-guard symptom does not reproduce in
this run, so it is not modified in v69.

## Why v69 captures 0x00140130 and XAPI ReadFile

Retail WX wait code calls `0x00140130` after `SleepEx` while waiting for its
per-object asynchronous bookkeeping field. This makes `0x00140130` the nearest
observed server/update boundary between successful host I/O and the stuck state
byte. Separately, XAPI `ReadFile` is the last wrapper before the host/kernel file
bridge and can determine which completion/status fields the CRI/XECI layer sees.

v69 therefore snapshots before instrumentation:

- `0x00140000..0x001403B0` — XECI/WX internal server/update region, including `0x00140130`.
- `0x00145F80..0x00146080` — XAPI ReadFile wrapper region.

A sparse `[XECISRV]` probe is inserted at generated `0x00140130` entry and every
generated retail RET. It only reads the already-known first WX object slot
`0x00273780` and records state, buffer, sector size, total sectors, current
position, request count, transfer byte offset/length, and the +0x148/+0x14C
bookkeeping words together with live registers.

## Constraint

v69 is evidence-only. It does not write the completion fields, increment sector
position, set ring fill, alter a return value, or otherwise substitute a guessed
CRI/XECI completion semantic. The next correction must be justified by the
captured retail instructions and generated C.
