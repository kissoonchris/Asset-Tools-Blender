# JSRF v40 — `readMarkDefault` CFG boundary

## Windows evidence from v39

The v39 Windows run keeps the genuine retail **Presented by SEGA** framebuffer
visible, confirming that the v39 `0x0002DBE0 .. 0x0002E170` correction moved
execution past the earlier post-logo corruption.

The first new causal control-flow break in the v39 runtime log is:

- `sub_00013F80` calls the retail frame path normally;
- indirect-call #72899 surfaces target `0x0002D1F8` as unresolved;
- the immediately enclosing `sub_0002D080` path returns with guest ESP exactly
  `0x100` bytes below its expected value;
- that same `-0x100` stack displacement propagates through `0x00025310`,
  `0x00011070`, `0x000123E0`, and `0x00013A80`;
- the corrupted frame then produces null indirect calls at return sites
  `0x0001194A` and `0x00011FEA`;
- while those null calls repeat, NV2A `DMA_PUT` and `DMA_GET` settle together at
  `0x0007AD2C`, so the unchanged SEGA frame is downstream of the CPU/CFG break,
  not evidence of a new GPU wait.

The high-frequency kernel ordinals 129/161 observed late in the run are
therefore downstream activity after the frame corruption, not the first cause.

## Independent retail function boundary

The public JSRF-Decompilation symbol table for the supported North American
standalone executable identifies adjacent retail functions:

- `readMarkDefault` — `0x0002D080`
- `readMarkPressOrTex` — `0x0002DBE0`

Therefore `0x0002D1F8` lies inside `readMarkDefault`; it is not a standalone
retail function entry. Promoting it as a new independent callback would repeat
the same class of CFG error fixed in v37/v39.

## v40 correction

v40 adds the measured interval

`0x0002D080 .. 0x0002DBE0`

to `JSRF_RETAIL_FUNCTION_BOUNDARIES`.

The existing boundary pinner removes every generic false start strictly inside
that range, including the runtime-observed `0x0002D1F8`, while preserving the
adjacent v39 `0x0002DBE0` function as its own entry.

Generated-code verification now additionally requires `0x0002D1F8` to exist as
an internal label inside generated `sub_0002D080` before a Windows runtime is
launched. This catches a future regression where xboxrecomp splits the same
computed destination out again.

No game return value, resource-read result, title-state decision, framebuffer
content, GPU command, or kernel result is forced.
