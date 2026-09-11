# JSRF Native v60 — ADXT decoder/update state diagnostic

## Runtime evidence from v59 HOTFIX2

The v59 HOTFIX2 Windows run reaches title state 12 and remains there while the
first ADXT object (`0x0027D120`) repeatedly reports status `-1` from the retail
low-output watchdog.  The media read itself succeeds; `title.adx` returns the
full 51,200-byte request.

The output ring at `0x00277180` is alive and its reserve/query methods execute,
but its produced/filled count remains zero.  The consumer asks for 4096-byte
spans while no bytes are available.  The v59 log contains no observed entry at
the retail producer/commit helper `0x0013FAD0` (nor `0x0013FCA0` in that run),
so the next useful boundary is upstream of the ring itself.

## v60 diagnostic

v60 instruments every generated basic block whose retail VA lies in
`0x0013D080..0x0013D2FF`, the ADXT decoder/update region immediately preceding
the already-instrumented `0x0013D300` watchdog updater.  It does not assume one
specific CFG label: both `loc_XXXXXXXX` blocks and split `sub_XXXXXXXX` entries
are accepted.  Missing optional coverage prints a warning and never aborts the
one-click build.

Each `[ADXTDEC]` record snapshots the title ADXT object and the decoder pointer
stored at ADXT `+0x04`, including the decoder's first twelve dwords, output
object, error/status, mode, threshold, progress and live guest registers.  The
probe performs guest reads only.  Per-basic-block logging is rate-limited to the
first eight hits and powers of two.

The older `[ADXTRING]` target-ring and `[ADXTERR]` error-write diagnostics are
also rate-limited in v60 so a long black-screen run does not bury the new decoder
control-flow evidence in tens of thousands of repeated lines.

No dirty-disc branch, ADXT error field, decoder state, ring counter, file result,
or audio data is modified by v60.
