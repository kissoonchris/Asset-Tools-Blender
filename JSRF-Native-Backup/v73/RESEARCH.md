# JSRF v73 - caller-cleaned ICALL failure recovery

## What v72 proved

The first real v72 Windows run validated the deferred APC change. During the title ADX read, xboxrecomp queues the file APC instead of dispatching it inline. Retail async helpers `0x00146375` and `0x001407E0` both return with `wants_update=1`; only after those returns does the queued APC drain into callback `0x001401B0`.

## New deterministic failure

The run then reaches the middleware cleanup loop and reproduces the old secondary fault around `0x0013B610`. The direct-call ABI probe records entry ESP `0x011EAF88` and return ESP `0x011EAFA8`. A translated call has already pushed its guest return address before `RECOMP_ABI_CALL`, so a normal cdecl return should produce `_ap + 4 = 0x011EAF8C`. The observed return is 28 bytes higher.

The retail bytes explain the exact 28-byte excess:

- At `0x0013B620..0x0013B63C`, retail pushes four arguments, performs an indirect vtable call returning at `0x0013B636`, then executes `add esp,0x10`.
- At `0x0013B732..0x0013B742`, retail pushes three arguments, performs another indirect vtable call returning at `0x0013B73F`, then executes `add esp,0x0C`.
- The generated code wraps both calls in `RECOMP_ICALL_SAFE`.
- On an invalid/unresolved target, the pinned `RECOMP_ICALL_SAFE` restores ESP to `_icall_esp`, which is captured before those arguments are pushed.
- Retail caller cleanup still executes afterward, so the first failure double-pops 16 bytes and the second double-pops 12 bytes. Total excess cleanup: 28 bytes, exactly the runtime delta.

The subsequent epilogue therefore restores EBX/ESI/EDI from the wrong guest stack slots. The cleanup loop later calls `0x0013B610` with a corrupted object pointer and eventually attempts to push at `0x011EAFFC`, beyond worker stack top `0x011EAFF0`.

## Generic correction

The v73 patch changes the pinned translator's `_fixup_icall_esp_save` post-pass. Before inserting an `_icall_esp` save, it conservatively recognizes the local caller-cleaned shape: the first post-call ESP adjustment must exactly equal four bytes times the number of arguments pushed for that call. Such sites are rewritten to ordinary `RECOMP_ICALL`, whose failed-target path removes only the synthetic return address and leaves argument removal to the caller's existing `add esp,N`.

Indirect calls without that exact local caller cleanup remain `RECOMP_ICALL_SAFE`, preserving stdcall-style recovery. The correction contains no JSRF addresses and does not force guest state.
