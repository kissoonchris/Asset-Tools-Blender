# Retail player locomotion recovery — checkpoint 06

Target executable:

- Jet Set Radio Future (USA retail) `default.xbe`
- MD5: `f4e366bfdceee551a09f42219c90d84e`
- SHA-256: `fd19055756719893c466302809b433b785ecf5732df0441286f3f605f0f3ef9c`

This checkpoint deliberately keeps recovered functions named with retail virtual
addresses when their higher-level semantic animation/state names are not yet
proven. No guessed acceleration, steering, stair, jump, gravity, or collision
behavior is introduced here.

## Controller object → CPlayer

Retail controller decode around `0xA80D1..0xA8165` establishes:

- controller `+0x8BC`: 16-bit direction
- controller `+0x8C0`: analog magnitude
- controller `+0x8C4`: action mask, with newly pressed action bits mirrored into
  bits 16..21
- analog magnitude is multiplied by literal float bits `0x37800080`
  (`0x1CD5B4`), the single-precision representation of `1/65535`

`CPlayer` helper `0x7F820` returns controller `+0x8C0`, except when either
`CPlayer+0x1170` or `CPlayer+0x1174` is positive, in which case it returns
`1.0f`.

## Signed speed field

`CPlayer+0xCD4` is the signed player speed used throughout player movement and
motion selection.

## Motion compositor @ 0x9D260

Literal XBE constants:

| XBE VA | float bits | value |
| --- | --- | --- |
| `0x1CD28C` | `0x3E6D097B` | `0.2314814776` (`25/108`) |
| `0x1CD10C` | `0x3EED097B` | `0.4629629552` (`25/54`) |
| `0x1CD470` | `0x3F6D097B` | `0.9259259105` (`25/27`) |
| `0x1CD0B8` | `0x3F9425ED` | `1.157407403` |
| `0x1CD500` | `0x408A3D71` | `4.320000172` |
| `0x1CD504` | `0x400A3D71` | `2.160000086` |
| `0x1CD508` | `0x3FB851EC` | `1.440000057` |

The first speed-dependent pair chooses motion slots `4/6`, blends `4→5` and
`6→7`, or chooses `5/7`. Equality at `25/54` enters the blend branch; equality
at `25/27` enters the direct `5/7` branch.

The second selector uses four bands:

- speed `< 25/108`: `0 → 0`, blend `0`
- `25/108 <= speed < 25/54`: `0 → 1`, `(speed - 25/108) * 4.32`
- `25/54 <= speed < 25/27`: `1 → 2`, `(speed - 25/54) * 2.16`
- `25/27 <= speed < 1.157407403`: `2 → 3`, `(speed - 25/27) * 4.32`
- speed `>= 1.157407403`: `3 → 3`, blend `0`

These are signed comparisons against `CPlayer+0xCD4`; negative speed is not
converted to magnitude first.

## Companion compositor @ 0x9D5E0

Branch body `0x9D67B` selects:

- speed `< 25/54`: `2 → 2`, blend `0`
- `25/54 <= speed < 1.157407403`: `2 → 3`,
  `(speed - 25/54) * 1.440000057`
- speed `>= 1.157407403`: `3 → 3`, blend `0`

## Angle/contact response

`CPlayer+0x1120` is not raw controller direction. Call sites such as
`0x87346..0x8735D` feed an orientation angle plus a contact/surface vector to
helper `0x7EC20`, which builds a horizontal direction from the retail sin/cos
tables, projects it through the contact geometry, and returns a 16-bit-angle
space result through the retail `atan2` helper `0x14C7D0`. The result is stored
in `CPlayer+0x1120`.

`0x98090` maps that signed angle to a response with exact thresholds:

- `< -0x4000`: `-1`
- `[-0x4000, -0x38F]`: `(angle + 0x38E) * 0x38878946f`
- `[-0x38E, +0x38E]`: `0`
- `[+0x38F, +0x4000]`: `(angle - 0x38E) * 0x38878946f`
- `> +0x4000`: `+1`

The scalar at `0x1CD468` has bits `0x38878946` (`6.462870806e-05`).

Motion type 4 path `0x9EB14` separately uses `abs(CPlayer+0x1120)` and:

- below `0x18E3`: multiply by float bits `0x392495F6`
  (`0.0001569612359`)
- at or above `0x18E3`: clamp to `1.0`

## Speed target/filter @ 0x98800

`0x98800` calls `0x7F820` for the input scalar.

Normal branch (`CPlayer+0xE60 == 0`):

1. `target = input * playerConfig[+0x118] + playerConfig[+0x10C]`
2. movement-row `+0xB8` is used as the response denominator and final symmetric
   signed-speed limit
3. movement-row `+0xB4` is the response numerator

Special branch (`CPlayer+0xE60 > 0`):

1. `target = input * playerConfig[+0x110] + playerConfig[+0x114]`
2. movement-row `+0xAC` supplies the selected limit field used by the zero-limit
   residual test
3. the final symmetric speed clamp is bypassed

Common tail beginning at `0x98867`:

`adjustment = previous + ((target - previous) / rowB8) * rowB4`

If the selected limit field is exactly zero and `abs(adjustment) < rowB4`, the
adjustment is zeroed. Signed speed is then `CPlayer+0xBB8 + adjustment`, with
normal mode finally clamped to `[-rowB8, +rowB8]`.

## Native implementation

Recovered code lives in:

- `src/player_locomotion.h`
- `src/player_locomotion.cpp`
- `tests/test_player_locomotion.cpp`

The module is intentionally not wired into the older playable prototype yet;
the prototype still contains behavior that predates this retail recovery. The
next safe integration point is to replace the placeholder movement step only
after the remaining base-speed (`CPlayer+0xBB8`) and contact/state transitions
feeding `0x98800` are recovered.
