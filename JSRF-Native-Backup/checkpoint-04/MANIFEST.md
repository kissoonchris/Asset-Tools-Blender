# JSRF Native checkpoint 04

Local source commit: `da03bfe4bf1d4c39410f04dd0b631a63495dfed4`

Fresh verification with all four retail Media archives restored: 18/18 CTest tests passing.

New in this checkpoint:
- Recovered static-stage collision candidate filtering from the retail CollisionManager path.
- One-sided finite segment-vs-triangle contact test matching the retail `0x18260` static-stage branch behavior recovered so far.
- Contact keeps authored collision surface-property bits, world-space point, normal, segment fraction, and start-to-hit distance.
- Closest static contact selection mirrors the retail nearest-distance pass at `0x194B0`.
- Retail Garage integration proves an authored floor contact at world origin near Y=-9 with surface property 1 (`floor`).

Backup format:
- `0001-Recover-retail-static-stage-contact-query.patch` applies this checkpoint's source delta on top of checkpoint 03.
- Full local source archive SHA-256: `8c31345fc4dca236e9fa4adebaab0d4c941c72468a4628e79552b90fd1f84fdf`.

Current limitation: player locomotion has not yet been switched to this contact solver; exact player probe generation/collision response is the next reverse-engineering target.
