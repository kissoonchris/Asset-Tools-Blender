# JSRF v66 — WX request translation evidence

## v65 Windows result

The corrected v65 return probes resolve the compressed-source request boundary:

- retail WX request `0x001403B0` is entered with object `0x00273780`, requested
  sector count `25`, and destination `0x012FBF80`;
- the WX object records `+0x20 = 25`, state `2`, and a 51,200-byte request;
- the host read at offset zero completes all 51,200 bytes with status zero and
  the destination begins with the expected ADX header bytes;
- nevertheless every observed generated WX return leaves `EAX = 0`;
- CRI wrapper `0x0013DD70` then returns the same zero;
- source `0x0027BD20 + 0x20` remains zero and the reserved producer ring span is
  cancelled through `0x0013FBC0`.

This places the first proven bad value inside the translated WX request method.
It is no longer useful to instrument the outer CRI wrapper until the WX return
path is understood.

## Why v66 does not force 25

The observed WX object state makes `25` a strong expected success result, but
forcing that value would hide whether the real defect is a bad lifted branch,
register value, function-boundary split, or another translator state merge. The
project has already found one generic translator flag-provenance defect in this
bring-up, so v66 preserves the exact translation evidence before changing
behavior.

## v66 evidence capture

Before any post-codegen JSRF instrumentation edits the generated files,
`write_jsrf_wxreq_codegen_report()` writes `JSRF_WXREQ_CODEGEN_LOG.txt` containing:

1. the exact 0x200 bytes from retail `0x001403B0..0x001405B0`; and
2. every untouched generated `sub_XXXXXXXX` whose entry lies in that same
   address range, including split CFG/function entries.

`Easy-Test-JSRF.ps1` appends that file to the normal `JSRF_TEST_LOG.txt` under
`WX REQUEST CODEGEN EVIDENCE`, so no extra user workflow is required.

The phase-1 runtime probe also uses the preserved guest stack argument to recover
the WX object after ECX has become scratch. This makes the next return record
show the accepted-sector and async-request fields at the exact generated RET.

No guest state or return value is modified.
