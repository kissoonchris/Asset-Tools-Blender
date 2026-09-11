# JSRF v64 — ADXT request return-path diagnostic

## Why v64 exists

v62 corrected the merged-CMP flag bug in the retail ADXT watchdog. v63 then
moved the investigation upstream to the compressed-source cancel boundary. The
remaining question is no longer whether the host can read `title.adx`: the host
read succeeds. The question is where the accepted request count stops being a
positive value before the source updater decides to cancel its ring reservation.

## Boundaries observed

v64 instruments generated retail code at three layers without changing it:

- `0x001403B0` — WX file-device request implementation, phase 0 at entry and
  phase 1 immediately before every generated return;
- `0x0013DD70` — CRI request wrapper, phase 0 at entry and phase 1 before every
  generated return;
- `0x0013C070` — source updater, phase 2 immediately after its direct call to
  `0x0013DD70` when xboxrecomp keeps that call boundary in the same generated
  function.

`[ADXTREQ]` records include the live EAX return value, general registers, the
first four guest stack arguments, and selected read-only source/WX fields. The
v63 `[ADXTSRC]` record at `0x0013FBC0` remains active, so the same run can compare
what the request layers returned with what source `+0x20` contains at cancel.

## Interpreting the next run

The first divergence identifies the next root-cause boundary:

- WX return positive, wrapper return non-positive: investigate wrapper/vcall
  return propagation or generated control flow in `0x0013DD70`.
- WX and wrapper returns positive, source phase-2 EAX non-positive: investigate
  direct-call return propagation into `0x0013C070`.
- Source phase-2 EAX positive but `[ADXTSRC] read_result` non-positive: inspect
  the source updater's store/branch path between the call and cancellation.
- All request results are positive but the ring still cancels: the cancellation
  is being selected by a later source-state condition rather than request
  rejection.

The optional source phase-2 hook may be unavailable if xboxrecomp splits the
caller at that boundary; that is non-fatal because the wrapper return and v63
cancel checkpoints still bracket the value.

No guest register, guest memory, file result, source state, ring accounting,
ADXT status, title state, or graphics state is modified by this diagnostic.
