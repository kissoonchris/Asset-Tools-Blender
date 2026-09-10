# JSRF Native v59 backup

v59 traces the CRI ADXT output-ring producer/consumer path after the v58 HOTFIX1 Windows capture showed `title.adx` loading successfully while output ring `0x00277180` remained empty and retail ADXT status became `-1`.

New read-only `[ADXTRING]` probes are injected at retail helpers `0x0013F9E0`, `0x0013FAD0`, `0x0013FBC0`, and `0x0013FCA0`. They record ring object/vtable, mode, requested bytes, pair pointer/length, filled/free counters, read/write positions, backing buffer, size/limit, and register/ESP context. No guest state is changed.

Local final package: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v59.zip`
SHA-256: `28d2e89cd4cbd743b19876ffc543443e18778c9596b8e467b501e3dc50323854`

The reconstruction patch bundle in `PATCHES.gz.b64` contains both the v58 -> v58 HOTFIX1 patch and the v58 HOTFIX1 -> v59 patch, gzip-compressed then base64-encoded.

Verification before packaging: 212 Python tests passed, C syntax check passed, native CMake/Ninja build passed, and 20/20 CTest tests passed. The final packaged copy was then extracted and re-run through the same Python/syntax checks and native build/CTest successfully.
