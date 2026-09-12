# JSRF Native v79 Checkpoint

## Status
The v78 Windows run proves a valid-to-invalid transition for sibling target `0x013D1010`. It was previously a valid GameObj (`vtable=0x001CB0A8`, `state4=0x00010007`, `+0x30=0x015A0870`) and is later reached from predecessor `0x01591F40` with `vtable=0x80008000`, `state4=0x8C818000`, and `+0x30=0x81688000`. The following sibling contains the `FFFF0000/55555555` pattern and faults at `0xFFFF0004`. Title ADX remains healthy at `filled=51164`, `read_pos=51200`.

v79 adds a diagnostic-only exact generated-store watch. The pinned xboxrecomp lifter emits a post-write guard for non-stack guest stores. Once v78's sibling tracker has seen a target as valid, `[GOBJWRITE]` fires if a generated store overlapping its four-byte vtable word changes it to invalid. The probe reports the write address/size, previous sibling observation, indirect-call counts, and exact generated C source file/line. Partial writes at target+1/+2/+3 are covered. No guest write or state is modified.

## Verification
- v79 TDD red phase: 4 expected failures before implementation
- focused v79: 4/4 passed
- preserved v74-v79 GameObj suite: 22/22 passed
- finalized source tree: 300/300 Python tests passed
- Python compile: passed
- C11 syntax: passed
- source-tree Release build: passed
- source-tree CTest: 20/20 passed
- exact ZIP fresh extraction: 300/300 Python + syntax passed
- exact ZIP Release build: passed
- exact ZIP CTest: 20/20 passed
- clean distributable: 227 files
- root `START JSRF TEST.bat`: present
- retail `default.xbe`: absent
- build/Python caches: absent
- fresh v78 + patch -> v79: 227 files, missing 0, extra 0, changed 0

## Artifacts
- `JSRF-Native-v79-CLEAN-PROJECT.zip`
  - SHA256 `04c81d3979f2097b68e5a034e604decc2655fdf4ccedb19844512422c2535183`
- `JSRF-v78-CLEAN-to-v79.patch`
  - SHA256 `d70e1eb23dc7a30c42e7a6609c38d30dfa433ea72838d95c738d13444a215054`
