# JSRF v43 — cache-state callback boundary and diagnostic

## What v42 changed

The v42 Windows run is the first post-SEGA run in this sequence that remains
stable without an unresolved indirect target or access violation. The user can
also close the framebuffer and the runner terminates cleanly with code 5.

The title still does not advance visually. The key difference from the earlier
SEGA stalls is that this is not an idle process: NV2A pushbuffer positions keep
moving and the software executor keeps rasterising while the game repeatedly
opens its numbered cache-completion markers.

## Cache loop visible in the runtime

After the mark/title setup phase, the runtime repeatedly accesses:

`Z:\Media\Cache\JSRF_CACHE_COMPLETE00.CMP`
through
`Z:\Media\Cache\JSRF_CACHE_COMPLETE08.CMP`

The exact retail source names

`D:\Media\Cache\DmCache%02d.tbl`
and
`D:\Media\Cache\Cache%02d.tbl`

do not appear in the v42 path trace.

The numbered completion strings are owned by the retail cache manager around
`0x24E70..0x25040`; forcing those marker files would bypass real game logic and
is therefore not used.

## Retail state flow

`sub_00025040` is the cache manager state machine already pinned by v35. Its
state 6 starts resource category 31 through `0x256A0`; state 7 waits for that
category, obtains its result through `0x25700`, and either enters the per-item
cache path or advances to another slot.

The category callbacks are indirect through arrays in retail `.data`:

- init array at `0x001EC068`;
- update array at `0x001EC0F0`;
- status array at `0x001EC178`;
- result array at `0x001EC200`;
- cleanup array at `0x001EC288`.

For update indices 30..32, the exact pointers stored in the retail image are:

- `0x001EC168 -> 0x00026170`;
- `0x001EC16C -> 0x000262B0`;
- `0x001EC170 -> 0x000263C0`.

Category 31 (`0x262B0`) formats `D:\Media\Cache\DmCache%02d.tbl` and calls the
asynchronous D:/Z: loader at `0x25DD0`. Category 32 has the same shape with the
adjacent `Cache%02d.tbl` string. The complete update bodies end immediately
before their status accessors at `0x26200`, `0x26390`, and `0x264A0`.

## Why v43 pins these callbacks

These are vtable/data-table-only state-machine callbacks rather than ordinary
direct-call functions. Category 30 contains an internal computed jump table and
all three callbacks contain multiple early return paths. Earlier JSRF failures
(v35/v37/v39/v40) established that an incomplete recovered boundary can preserve
apparently valid execution while dropping a required state path or stack frame.

v43 therefore pins:

- `[0x00026170, 0x00026200)`;
- `[0x000262B0, 0x00026390)`;
- `[0x000263C0, 0x000264A0)`.

This is a structural correction only. No cache result, marker existence result,
or state transition is forced.

## Diagnostic added for the next Windows run

The exact reason the v42 path trace never reaches `DmCacheXX.tbl` cannot be
proved from the old log alone. v43 therefore observes the relevant retail state
without modifying it:

- `[CACHESTATE]` reports cache-manager enabled/slot/item/state fields;
- `[CACHEDISPATCH]` reports the shared resource dispatcher's busy/category and
  requested operation;
- `[CACHETABLE]` reports category-31 state and every sampled call into the
  D:/Z: table loader, including the guest path string;
- `[CACHEPREFLIGHT]` records whether the corresponding retail `DmCacheXX.tbl`
  and `CacheXX.tbl` source files are physically present in the selected
  extracted game directory.

The next run can therefore distinguish three cases directly: the cache manager
never requests category 31, the shared dispatcher rejects/blocks the request, or
category 31 reaches the loader and a concrete source/cache file operation fails.
