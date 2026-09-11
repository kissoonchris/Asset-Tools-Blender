# JSRF v71 WX async completion path

## v70 Windows evidence

v70 preserved the v67 request fix: WX `0x001403B0` and its CRI wrapper return 25 sectors and the host completes the 51,200-byte `title.adx` read. The object still remains state 2 at sector position 0 and the ADXT source ring remains unfilled until the retail watchdog reaches 1201 stalls.

The new generic `ReadFile`-caller probe did not capture the title transition because its global 128-hit budget was exhausted by unrelated startup file traffic before the title ADX request entered the interesting caller.

## Proven retail path

The untouched generated code identifies the relevant asynchronous chain:

- `0x001407E0`: WX worker. It clears `+0x148`, sets `+0x14C`, and when global mode `0x002615C0` is zero it calls `0x00146375` with callback `0x001401B0`.
- `0x00146375`: XAPI asynchronous read helper.
- `0x001401B0`: completion callback. On success it writes state byte 3 and always clears the WX `+0x14C` update flag.

This is the exact state transition that v71 observes. v71 does not force it.

## v71 diagnostic

`write_jsrf_wx_async_completion_report()` saves exact retail bytes and untouched generated C for those three functions before instrumentation. `instrument_jsrf_wx_async_completion_path()` adds entry/return probes only to those exact functions.

`[WXASYNC]` logs only while the known title WX object is active (`state==2`, read/update flags nonzero, 51,200-byte transfer active, or callback context resolves to the object). Counters are per-site/per-phase, so unrelated startup I/O cannot consume the title evidence budget.

The callback probe also records six guest stack arguments and resolves its third argument's `+0x10` object pointer when readable. All probes are read-only.
