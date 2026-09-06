# JSRF Native checkpoint 06 — retail player locomotion recovery

Parent backup: checkpoint 05 on branch `jsrf-native-backup`.

Local source commit produced during recovery: `3603f20` (`Recover retail player locomotion pipeline`).

Fresh verification before backup: **19/19 CTest tests passing**.

Local artifact SHA-256 values:
- `JSRF-Native-Source-Checkpoint-06.zip`: `1e6a8ed96b5851aec2db5b621f4b2a2696ff8a06d246097c59a19c6ef078e2ae`
- `JSRF-Native-Checkpoint-06.git.bundle`: `390d06d297b82e84f40c1e6d8898e3228acf6322e943b2dc7edbd75e85ddce66`
- `JSRF-Checkpoint-05-to-06.patch`: `6db7ca53730548c75a06f6ea20f746b811d63fe956c208b7d716792d84979353`

New in this checkpoint:
- Recovered retail controller fields feeding CPlayer: direction at `+0x8BC`, analog magnitude at `+0x8C0`, and action mask at `+0x8C4`.
- Recovered `CPlayer+0xCD4` signed-speed motion-band behavior from `0x9D260` and `0x9D5E0`.
- Preserved the exact retail speed thresholds and boundary behavior (`25/108`, `25/54`, `25/27`, and the upper band threshold).
- Recovered the `CPlayer+0x1120` angle/contact response used by helper `0x98090` and the motion-type-4 turn blend path.
- Recovered the controller scalar helper `0x7F820` and the `0x98800 -> 0x98867` target/smoothing/deadzone/clamp chain.
- Added an evidence-only native module and regression tests; this is deliberately not wired into the older placeholder free-movement prototype yet.

Recovery delta files are stored directly under this checkpoint so the work is recoverable without depending on chat-local files:
- `CMakeLists.txt`
- `src/player_locomotion.h`
- `src/player_locomotion.cpp`
- `tests/test_player_locomotion.cpp`
- `research/RETAIL_PLAYER_LOCOMOTION.md`

Next safe target: recover generation of `CPlayer+0xBB8` base speed plus the contact/state transitions feeding `0x98800`, then replace the placeholder movement step only when the retail chain is complete.
