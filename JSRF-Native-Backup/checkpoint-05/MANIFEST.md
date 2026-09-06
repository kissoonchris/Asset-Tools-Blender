# JSRF Native checkpoint 05 — retail Garage slice

Local source commit: `6adc828d18e20c85819971b0c3f77b2e0a11d503`

Fresh verification with the extracted retail Media tree: **20/20 CTest tests passing**.

Validated source ZIP SHA-256: `74166f2030af8351d0e94e3c980a8d9d4ffa9a0b8c28af0cd6d71d8d58305eda`.

New in this checkpoint:
- Retail Garage Stage_Model visual decoding and strip-boundary reconstruction.
- Retail Garage DXT1/DXT3 texture decoding.
- Retail Beat + Garage visuals + authored grind paths in one runtime.
- Recovered multi-segment CPlayer grind traversal integrated into the Garage slice.
- Native `jsrf_garage` target and Win32 interactive window.
- `Build-And-Run-Garage-Windows.bat` plus a dedicated one-click Windows build script.
- Win32 camera orbit (RMB drag or arrows), R restarts the retail rail run, Esc exits.

The source delta from checkpoint 02 (`c2228ac`) is backed up as three UTF-8-safe Base64 parts so it is not vulnerable to the binary ZIP truncation seen in checkpoint 03:
- `patch.gz.b64.part-00` — SHA-256 `fae07f116fd8943a851039a8a886bb5b0c0d0e2f378b95331a6134cbac7e1a7d`
- `patch.gz.b64.part-01` — SHA-256 `679361a77d38095b8834451f0264bc577f028def8c0ef7e9e9eb5d5ec8489f42`
- `patch.gz.b64.part-02` — SHA-256 `e5c1be893f329bc3593ab15e1bdc0789c82086423e70a5399939952da90f2795`

Concatenate the three files in numeric order, Base64-decode the result, then gunzip it to recover `0001-Retail-Garage-playable-slice.patch`. The original patch SHA-256 is `06d8fc14a995dc6b638aacd1fe34186652a1cf307f6ebc88c5b418f3a984a95a`.

Current scope: this is a retail-driven rail/visual milestone, not yet full JSRF locomotion. The old free-movement prototype remains separate and is not being represented as authentic gameplay.
