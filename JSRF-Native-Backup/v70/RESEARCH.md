# JSRF v70 - XAPI ReadFile caller evidence

## Windows evidence entering v70

The v69 Windows run preserves the v67 request correction. Retail WX
`0x001403B0` returns 25 accepted sectors and the CRI wrapper propagates that
value. The host then completes the title ADX read at offset zero for all 51,200
requested bytes.

After that read, the first WX object remains in state 2 with sector position 0,
request count 25, and transfer length 51,200. Its `+0x148` field changes from 1
to 0, so the request is being serviced by a worker path. Its `+0x14C` field is
0, and the source ring remains at zero filled bytes until the real ADXT watchdog
reaches 1201 stalls.

The v69 probe at `0x00140130` recorded no hits. Inspection of the untouched
v69 codegen capture corrects the earlier label: `0x00140130` is a timing/wait
helper called by a polling path, not the object-service routine. Lack of a hit
there is therefore expected when `+0x14C` is already zero.

## v70 diagnostic

v70 removes the guessed completion address and discovers the relevant boundary
from generated code itself:

1. scan every untouched `recomp_*.c` function before instrumentation;
2. select every translated function whose body directly references XAPI
   `ReadFile` at `0x00145F8B`, excluding `ReadFile` itself;
3. preserve each caller's untouched generated C and the corresponding retail
   byte range in `JSRF_READ_CALLER_CODEGEN_LOG.txt`;
4. add sparse read-only entry/return probes to those exact discovered callers;
5. at each probe record registers plus the first WX object's state, position,
   embedded OVERLAPPED/IO_STATUS_BLOCK status and Information fields, transfer
   offset/length, and read/update flags.

This is intentionally observation-only. It does not synthesize async completion
or advance any retail object field.

## Decision rule for the next Windows run

- If the discovered caller exits after a successful host read with a wrong
  register/branch result relative to its retail bytes, reproduce that codegen
  defect in a synthetic translator test and fix the generic translator.
- If the caller's translation is correct but the embedded IO status/result does
  not match Xbox/XAPI semantics, fix the generic kernel/XAPI bridge instead.
- If both are correct, follow the caller's exact post-read call/branch target and
  move the probe one boundary deeper. Do not force `position=25`, state, or ring
  fill without retail proof.
