# JSRF v63 — CRI compressed-source read return diagnostic

## Runtime evidence from v62

v62 fixed the premature ADXT watchdog branch. The retail error is now written
only after `stall6a` reaches 1201. The remaining problem is earlier in the CRI
ADX input path:

- `D:\Media\Z_ADX\BGM\title.adx` opens successfully.
- The host read at offset 0 requests and receives all 51,200 bytes with status 0.
- Decoder ring `0x00277180` nevertheless remains `filled=0`.
- Source object `0x0027BD20` repeatedly reserves the full writable ring span and
  immediately enters ring cancel/release `0x0013FBC0`.
- No source-side `0x0013FAD0` commit is observed.

## Retail control flow

`0x0013C070` is the CRI source updater. On the no-pending path it reserves a
writable ring span and calls `0x0013DD70` with the file wrapper, sector count and
buffer pointer. The return is stored at source `+0x20`.

- return > 0: source `+2` becomes 1 and the reservation stays outstanding while
  the asynchronous read completes;
- return <= 0: the updater calls ring method `+0x1C` (`0x0013FBC0`) to cancel the
  reservation.

`0x0013DD70` dispatches the file device's vtable `+0x20` request method. The
runtime's default CRI device is WX; its vtable request entry is retail
`0x001403B0`. That function records the accepted sector count in implementation
`+0x20`, sets async offset/length at `+0x140/+0x144`, enters state 2, and returns
the accepted sector count.

The successful host read proves an asynchronous request reaches the lower file
layer, while the immediate source cancellation says the upper source updater
observes a non-positive request result. The missing evidence is the state at
that exact boundary.

## v63 diagnostic

v63 extends the already-stable `jsrf_adxt_ring_probe()` at `0x0013FBC0` only
when:

- ring mode is 0 (producer/write reservation), and
- `ESI` identifies an entry in the retail CRI source pool
  `0x0027BD20..0x0027C720`.

Sparse `[ADXTSRC]` records include:

- source flags/state/pending/retry and source `+0x20` request result;
- reserved pair pointer/length and source progress fields;
- file wrapper, implementation object and vtable read/status targets;
- WX implementation state, current accepted-sector count, byte offset/length and
  async flags;
- first 16 bytes at the reserved buffer.

The buffer bytes are especially useful after the successful host read: if the
ADX header (`80 00 00 20 ...`) is physically present while source `+0x20` is
still zero, the failure is return/control-flow propagation rather than the I/O
copy itself.

This probe performs guest reads only. It does not alter file results, CRI source
state, ring counters, ADXT status, watchdog values, or title state.
