# JSRF Native v60

Purpose: isolate why the retail ADXT title-music decoder never reaches the
output-ring producer/commit path after `title.adx` has loaded successfully.

Changes:
- adds read-only `[ADXTDEC]` instrumentation across every generated retail basic
  block in `0x0013D080..0x0013D2FF`;
- snapshots ADXT `0x0027D120`, its decoder object at `+0x04`, the decoder's first
  twelve dwords, output/status/mode/progress state, and live guest registers;
- handles both ordinary `loc_XXXXXXXX` labels and xboxrecomp split
  `sub_XXXXXXXX` entries;
- optional decoder instrumentation warns and continues if future generated CFG
  output omits the region; it cannot abort an otherwise valid build;
- rate-limits the already-proven `[ADXTRING]` target-ring and `[ADXTERR]`
  error-write spam during long state-12/black-screen runs;
- runtime banner identifies `JSRF Native v60`.

No emulation behavior is bypassed or fabricated in this revision.  The retail
dirty-disc decision, ADXT status, file reads, decoder state, and audio ring data
remain authoritative.
