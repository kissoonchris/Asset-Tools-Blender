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

The patch in this directory is a text backup from checkpoint 02 (`c2228ac`) through this checkpoint, so it is not vulnerable to binary ZIP truncation.

Current scope: this is a retail-driven rail/visual milestone, not yet full JSRF locomotion. The old free-movement prototype remains separate and is not being represented as authentic gameplay.
