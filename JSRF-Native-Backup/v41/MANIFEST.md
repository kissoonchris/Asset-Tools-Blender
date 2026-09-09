# JSRF Native v41 manifest

## Purpose

v41 corrects an over-broad retail function boundary introduced by v40.
v40 correctly established that `0x0002D1F8` is an internal switch case, but it
incorrectly treated every address through `0x0002DBE0` as part of the same
function. The v40 Windows run proves that `0x0002D240` and `0x0002D460` are
real callable entries that were being deleted from the generated dispatch map.

Pinned xboxrecomp revision remains:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v40 Windows evidence

- Indirect target `0x0002D460` is unresolved immediately before the failure.
- The next indirect target `0x0002D240` is also unresolved.
- ABI corruption follows through `0x00025390`, `0x000256A0`, and `0x0007E180`.
- A null virtual call from `0x0001108A` follows, then the process faults in
  `sub_00011070` while reading a corrupted object pointer.
- `DMA_PUT == DMA_GET` at the failure, so the last visible frame is downstream
  of CPU/object corruption rather than a blocked GPU queue.
- The automatic runner reports four safe callback additions, yet
  `0x0002D240`/`0x0002D460` remain unresolved on later passes because v40's
  post-pass removes every start inside `0x0002D080..0x0002DBE0`.

## Direct retail-XBE correction

Direct analysis of the exact supported retail XBE proves that the public symbol
list used by v40 omitted unnamed functions between `readMarkDefault` and
`readMarkPressOrTex`.

v41 pins the actual adjacent cluster:

- `0x0002D080..0x0002D240` — data pointer `0x001EC158`; contains internal case
  `0x0002D1F8`; four plain-RET exits.
- `0x0002D240..0x0002D460` — data pointer `0x001EC0CC`; own prologue/switch;
  one `ret 4` exit.
- `0x0002D460..0x0002D470` — data pointer `0x001EC1DC`; one `ret 4` exit.
- `0x0002D470..0x0002D480` — data pointer `0x001EC264`; one `ret 0x0c` exit.
- `0x0002D480..0x0002D570` — direct call evidence at `0x0002D58E`.
- `0x0002D570..0x0002D8D0` — direct call evidence at `0x0002DD62`.
- `0x0002D8D0..0x0002D950` — direct call evidence at `0x0002DEC1`.
- `0x0002D950..0x0002D9D0` — direct call evidence at `0x0002E0C2`.
- `0x0002D9D0..0x0002DA70` — direct call evidence at `0x0002DC5A`.
- `0x0002DA70..0x0002DBE0` — direct call evidence at `0x0002DE48`.

The separately proven v39 `0x0002DBE0..0x0002E170` boundary remains intact.

Generated-code verification now checks the measured RET/stack-cleanup contract
for every function in the corrected cluster before launching Windows runtime.
No game or resource behavior is substituted.

## Regression coverage

`tests/test_v41_mark_resource_boundaries.py` was written before the correction
and failed against v40. It verifies:

1. all ten corrected retail intervals and their independent entry evidence;
2. switch-case absorption without deleting real function entries;
3. measured generated RET cleanup across the cluster;
4. rejection of incorrect `ret 4`, `ret 0x0c`, and `ret 8` cleanup;
5. activation of the v41 generated-code contract in the production pipeline.

The v40 regression was refined to retain its valid invariant — `0x0002D1F8`
must remain inside `0x0002D080` — while using v41's now-proven physical end at
`0x0002D240`.

## Fresh local verification

- Focused v39/v40/v41 regressions: **16/16 passed**
- Full Python suite: **116 passed**
- Python compile check: **passed**
- Fresh native CMake/Ninja configure/build: **passed**
- Native CTest suite: **20/20 passed**
- Exact retail XBE SHA-256: **verified**
- Four literal data-pointer entries and six direct-call entry proofs: **verified**

Windows runtime confirmation is still required before claiming the SEGA-screen
path advances beyond this corrected mark-resource cluster.
