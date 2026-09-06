# JSRF Native checkpoint 07 — retail ground/contact + airborne response

Local source commit: `62e34c57f19c3a534da9adaf4719a44010811562`
Parent checkpoint commit: `3603f206c2d77f35fffeb46eca3474240533551b`

Fresh verification: **19/19 CTest tests passing**.

Target retail XBE:
- MD5 `f4e366bfdceee551a09f42219c90d84e`
- SHA-256 `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`

Newly recovered in this checkpoint:
- exact retail signed-angle delta `0x4A7F0` and shortest-path approach `0x4A820`
- `CPlayer+0xA84` contact-normal inclination semantics via `0x7ECF0`
- corrected `CPlayer+0x1120` semantics: signed contact-plane grade via `0x7EC20`
- `CPlayer+0xBB8` state-entry base-speed seed and `0x992D0` upper-only cap
- grade blend initialization/smoothing (`0x992D0`, `0x987A0`)
- steep-slope side classifier `0x98970`
- controller direction override `0x7F850`
- speed-dependent steering fraction and projected-heading response `0x989D0`
- low/mid/high ground steering decisions and exact boundary behavior `0x993D0`
- low-inclination constraint helper `0x7F9E0`
- contact/state gate `0x836A0`
- vertical-speed integration and conditional airborne orientation interpolation `0x991A6`
- triangular launch acceleration helper `0x99E20`
- steep-surface launch setup and vertical caps/floors `0x996A0..0x9978D`
- air-to-ground speed/base-speed transfer `0x99AA0`

Artifacts:
- `JSRF-Native-Source-Checkpoint-07.zip` — SHA-256 `9c6ef62ad685749fbeb2101a4d273ec4e8c3f803cfc438e809410231782d4d50`
- `JSRF-Native-Checkpoint-07.git.bundle` — SHA-256 `f7fbba87b50596cfafc1f629e1aa6c5eb7a22c2066c314e8ee2e7e223f086157`
- `JSRF-Checkpoint-06-to-07.patch` — SHA-256 `b1bd365f3d6a12d86c971b0b3a945ad13047dfc607b50bd53747839f5368994e`

The older placeholder/free-movement prototype remains intentionally disconnected. The remaining integration blocker is retail position/contact resolution and the unresolved transform front-half around `0x989D0`; recovered movement is not being replaced with invented behavior.
