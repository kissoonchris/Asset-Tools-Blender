# JSRF Native v17 — lock xadd flag-tracking correction

Date: 2026-09-06
Base backup branch: `jsrf-native-backup-v16-stage`
Retail XBE SHA-256: `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`
Pinned xboxrecomp: `4d337526dc4ab892483aad0246aee9d1c33d1b81`

## v16 Windows root cause

The v16 Windows trace proves the heap corruption is a one-node self-cycle. At
retail allocator loop `0x00149B28`, free-list node `0x0105DE68` has both next
and prev links pointing to itself while the large-list sentinel also points to
that node. The loop repeats unchanged into the billions until the watchdog.

Tracing backward identifies the corruption source before the heap loop:
`sub_0015FD40` is a COM-style Release method. The first observed Release enters
with refcount 2 and nonetheless takes the zero-count deleting-destructor path.
A second Release sees refcount 1 and again deletes the same object, inserting
its heap block a second time and producing the self-linked free-list node.

Retail `sub_0015FD40` uses `lock xadd [refcount], -1` followed by `jne`. The
pinned xboxrecomp revision lowers the atomic operation correctly, but its flag
tracker classifies literal mnemonic `lock xadd` as flags-undefined while the
JCC evaluator only understands `xadd`. As a result the retail `jne` can take the
wrong path even though the atomic refcount update itself is correct.

## v17 correction

`scripts/jsrf_xboxrecomp.py` now applies an idempotent, pinned-toolkit source
patch before code generation which:

- tracks `lock xadd` as an EFLAGS-setting instruction;
- removes `lock xadd` from the flags-undefined set;
- evaluates `lock xadd` branches through the existing xadd result-based JCC
  path.

No JSRF gameplay function is replaced and no allocator/manual gameplay override
is introduced. The v16 heap probe remains enabled for the next Windows run so
we can prove whether the corrupt free-list path disappears.

## Verification

Fresh v17 verification:

- `python -m py_compile scripts/jsrf_xboxrecomp.py` — pass
- `python -m pytest -q tests` — 27 passed
- CMake configure/build — pass
- CTest — 20/20 passed
- Easy-test ZIP integrity (`unzip -t`) — pass
- `START JSRF TEST.bat` is present at archive root

## Artifacts

- `JSRF-Native-Source-Checkpoint-08-HOTFIX16.zip`
  - SHA-256 `f365f43318b0eb46148072544a093569cc7282f58e486cdb9ce2c62e720ede01`
- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v17.zip`
  - SHA-256 `f365f43318b0eb46148072544a093569cc7282f58e486cdb9ce2c62e720ede01`
- `JSRF-V16-to-V17.patch`
  - SHA-256 `17bf8f45a6be9228738b39ec5687e52dfd255ed1d305a089b8910a199d91c3cb`

## Next Windows evidence

Run `START JSRF TEST.bat` from the v17 easy-test package and return the generated
JSRF test/runtime/stackwalk logs. The retained HEAPPROBE output will show whether
the duplicate free and self-cycle are gone, and where execution reaches next.
