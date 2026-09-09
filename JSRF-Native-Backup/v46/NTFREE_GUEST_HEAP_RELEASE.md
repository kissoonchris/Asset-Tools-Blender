# JSRF v46 — NtFreeVirtualMemory guest-heap release

## What v45 proved

v45 changes the problem materially. The corrected ordinal-207 ABI passes the
actual marker mask to the directory backend, so JSRF selects cache slot 1,
starts resource category 31, receives a cache table with 174 entries, processes
those entries, reaches cache state 5, and creates
`JSRF_CACHE_COMPLETE01.CMP`.

That means the old marker-enumeration loop is solved. The remaining failure is
later in the authentic retail cache-building path.

## Heap failure

During slot-1 copying the guest heap reaches roughly 48.5 MB used and allocation
requests begin failing. The returned run contains 2,385
`xbox_HeapAlloc: out of memory` messages. Slot 1 still reaches its final marker,
but when slot 2 starts, category 31 eventually cannot obtain even the small
buffer needed for the next cache table, so later slots fail and are retried.

This is not evidence that JSRF really needs a larger Xbox heap. It is evidence
that short-lived allocations are not being returned.

## Ordinal 199

Retail wrapper `sub_00145A5D` calls original-Xbox kernel ordinal 199 with the
three documented arguments:

- pointer to a 32-bit guest `BaseAddress` field;
- pointer to a 32-bit guest `FreeSize` field;
- `FreeType`.

The exact retail cleanup sites measured for this path use `MEM_RELEASE`
(`0x8000`) and a zero free size.

The v45 Windows log directly records four rate-limited ordinal-199 calls and all
four return `0xC0000001` instead of success.

## xboxrecomp defect

The pinned bridge was:

```c
static void bridge_NtFreeVirtualMemory(void)
{
    uint32_t base_ptr = STACK_ARG(0);
    uint32_t size_ptr = STACK_ARG(1);
    uint32_t free_type = STACK_ARG(2);

    g_eax = (uint32_t)xbox_NtFreeVirtualMemory(
        XBOX_TO_NATIVE(base_ptr), XBOX_TO_NATIVE(size_ptr), free_type);
}
```

On a 64-bit host, that is not a width-neutral translation. `base_ptr` and
`size_ptr` point at four-byte Xbox fields. The host helper interprets them as
native pointer-sized objects and ultimately uses Win32 `VirtualFree`, even
though these allocations were handed out by xboxrecomp's guest `xbox_HeapAlloc`.

xboxrecomp already has the correct ownership mechanism for those allocations:
`xbox_HeapFree`, backed by the same guest block table as `xbox_HeapAlloc` and
capable of free-block reuse/coalescing. The missing piece is routing the Xbox
API to that guest allocator rather than to host virtual memory.

## v46 correction

v46 reads the two IN/OUT fields with `BRIDGE_MEM32`, validates the observed
`MEM_RELEASE` contract, and returns the allocation through `xbox_HeapFree`.
`MEM_DECOMMIT` remains a separate non-release operation because the runtime's
flat guest mapping has no per-reservation host page commitment to tear down.

A rate-limited `[VMFREE]` line records the guest base and known heap block size.
The diagnostic is read-only except for the API's own expected IN/OUT fields and
the actual guest allocation release.

## What the next Windows run must prove

The first v46 run should show successful `[VMFREE]` releases while slot 1 is
being built. The important outcomes are whether the heap stops climbing to
exhaustion, whether slots 2–8 can load their cache tables and complete, and what
the next authentic title state is after the cache manager finishes. No marker
or state should be synthesized to force that outcome.
