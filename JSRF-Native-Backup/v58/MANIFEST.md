# JSRF Native v58 manifest

## Purpose

v57 proves the current dirty-disc screen is not the earlier kind-1 shared
resource timeout. The active trigger is the retail stream-status call at
`0x00116EA8`, after `ADXT_GetErrCode` returns `-1`. v58 instruments the exact
CRI ADXT error producer so the next Windows run can identify the missing runtime
semantic instead of suppressing the retail error.

## v57 Windows evidence

- dirty helper `0x0006F730` return address: `0x00116EAD`;
- source: stream-status site `0x00116EA8`;
- trigger return: `0xFFFFFFFF` (-1);
- shared resource dispatcher later reaches `busy=0`;
- `title.adx` opens successfully;
- first `title.adx` read requests 51,200 bytes and receives all 51,200 with status `0x00000000`;
- GPU work continues and the process remains alive until the framebuffer is closed manually.

## v58 changes

- adds read-only `jsrf_adxt_error_probe`;
- injects `[ADXTERR]` at `ADXT_GetErrCode` entry `0x0013AA50`;
- injects it after the ADXT output vtable `+0x24` call at `0x0013D3C9`;
- injects it after the matching retail `-1` write at `0x0013D3F5`;
- records the ADXT error/status, threshold, progress, watchdog counters, output object/vtable/slot target and output-call return;
- rate-limits the low-output trace;
- advances the runtime banner to `JSRF Native v58`;
- retains v57's permanent `0x0013D840` callback seed, v56/v55 diagnostics and all earlier runtime/NV2A corrections.

No guest error value, timeout, file return, title transition, framebuffer pixel, or dirty-disc branch is forced. The slightly distorted text is tracked separately and is not speculatively changed in this diagnostic revision.

## Verification

Final source and extracted-package verification: 205 Python tests passed, C syntax check passed, fresh CMake/Ninja build passed, and 20/20 CTest tests passed. The v57-to-v58 patch applies cleanly to a clean v57 tree and reproduces all changed/new files byte-for-byte.