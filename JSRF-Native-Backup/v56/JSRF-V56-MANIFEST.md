# JSRF Native v56 manifest

v55 identified the dirty-disc helper as the retail shared-resource timeout at 0x2537E: helper entry retained guest return 0x0001108A from the virtual update call at 0x00011087 while dispatcher object 0x012EE060 remained busy as kind 1.

The last observed resource path before timeout was D:\Media\Sounds\SE\pv_beat.dat, and the runtime read all 267,776 requested bytes with STATUS_SUCCESS. v56 therefore stays diagnostic rather than bypassing the retail timeout.

v56 adds targeted kind-1 [PLAYERLOAD] state transitions and an expanded [DISCDIAG] snapshot of private player-loader state (+0x25C..+0x274), shared D:/Z: loader state (+0x50..+0x64), retained path, and both timeout timestamps. It also stops enabling RECOMP_CS_TRACE_CRT=all and RECOMP_TRACE_ARGS=8 by default because the v55 run emitted tens of thousands of trace lines while this operation was under a real 15-second wall-clock timeout.

No guest completion flag, I/O status, timeout, callback result, title state, or dirty-disc branch is forced.

Pre-package verification: focused v56 4/4; full Python 197/197; Python compile passed; native build passed; CTest 20/20 passed.
