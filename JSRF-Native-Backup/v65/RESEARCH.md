# JSRF v65 — generated ADXT return-probe shape correction

## v64 Windows evidence

The v64 run reaches both request-layer entry probes repeatedly:

- CRI wrapper `0x0013DD70` phase 0;
- WX request method `0x001403B0` phase 0.

The same run contains no `[ADXTREQ] ... phase=1` records and no source-updater phase 2 records. The existing v63 source checkpoint still shows the ADX header bytes in the reserved buffer while source `+0x20` remains zero and the reservation is cancelled.

This means v64 did not yet observe the request return values; it does not prove that either request function itself returned zero.

## Diagnostic root cause

The v64 post-codegen injector matched only a return statement that began its own line: `return;`.

Pinned xboxrecomp normally emits retail returns with the guest-stack cleanup and C return on the same line, for example `esp += 4; return; /* ret */` or `esp = esp + 4; return; /* ret */`.

The functions therefore executed normally but bypassed v64's textual return matcher, explaining the complete absence of phase-1 records despite repeated phase-0 entries.

## v65 correction

`instrument_jsrf_adxt_request_probe()` now recognizes both standalone returns and generated lines explicitly marked by xboxrecomp's `/* ret */` comment. It inserts the existing phase-1 probe on a separate line immediately before the generated RET line, preserving the pre-cleanup guest stack and live EAX result.

Unrelated conditional C returns are not rewritten. The change is diagnostic only and remains idempotent.

The next Windows run should finally distinguish whether the accepted sector count is lost inside WX, across the CRI wrapper, or later in the source updater.
