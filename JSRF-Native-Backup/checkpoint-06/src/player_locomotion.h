#pragma once

#include <cstdint>

namespace jsrf {

// Small, evidence-grounded pieces of the retail CPlayer motion compositor.
// Names retain the retail function addresses because the semantic animation
// labels for these slots have not yet been proven from the XBE.
struct RetailMotionBlendBand {
    int from{};
    int to{};
    float blend{};
};

struct RetailPairedMotionBlend {
    int firstFrom{};
    int firstTo{};
    int secondFrom{};
    int secondTo{};
    float blend{};
};

// CPlayer routine @ 0x9D260, first speed-dependent paired motion selection.
[[nodiscard]] RetailPairedMotionBlend selectRetailPlayerPairedMotionBlend9D260(
    float signedSpeed) noexcept;

// CPlayer routine @ 0x9D260, speed-dependent 0..3 motion-band selection.
[[nodiscard]] RetailMotionBlendBand selectRetailPlayerMotionBand9D260(
    float signedSpeed) noexcept;

// Companion CPlayer routine @ 0x9D5E0, branch beginning @ 0x9D67B.
[[nodiscard]] RetailMotionBlendBand selectRetailPlayerMotionBand9D5E0(
    float signedSpeed) noexcept;

// CPlayer motion-type-4 path @ 0x9EB14. The retail code takes the absolute
// signed value stored at CPlayer+0x1120, scales it below 0x18E3, and clamps
// to 1.0 at and above that threshold.
[[nodiscard]] float retailPlayerMotionType4TurnBlend(
    std::int32_t signedAngleDelta) noexcept;

// CPlayer helper @ 0x98090: exact signed angle response used by player
// movement-state code. It has a +/-0x38E deadzone and saturates only beyond
// +/-0x4000.
[[nodiscard]] float retailPlayerAngleResponse98090(
    std::int32_t signedAngle) noexcept;

struct RetailPlayerSpeedUpdate {
    float adjustment{};
    float signedSpeed{};
};

// CPlayer speed-update tail @ 0x98867. targetAdjustment is the x87 value
// produced by the immediately preceding character/input-stat calculations.
[[nodiscard]] RetailPlayerSpeedUpdate retailPlayerSpeedFilter98867(
    float targetAdjustment,
    float previousAdjustment,
    float baseSignedSpeed,
    float responseNumerator,
    float responseDenominator,
    float signedSpeedLimit,
    bool bypassSpeedClamp) noexcept;

// CPlayer helper @ 0x7F820. The controller-owned +0x8C0 magnitude is
// overridden to 1.0 only while either CPlayer+0x1170/+0x1174 is positive.
[[nodiscard]] float retailPlayerInputScalar7F820(
    float controllerMagnitude,
    std::int32_t overrideCounter1170,
    std::int32_t overrideCounter1174) noexcept;

struct RetailPlayerSpeedConfig98800 {
    float normalTargetScale{};      // player config +0x118
    float normalTargetBias{};       // player config +0x10C
    float specialTargetScale{};     // player config +0x110
    float specialTargetBias{};      // player config +0x114
    float responseNumerator{};      // movement row +0xB4
    float responseDenominator{};    // movement row +0xB8
    float specialLimitField{};      // movement row +0xAC
};

// CPlayer @ 0x98800 from the already-decoded input scalar through the
// recovered smoothing/clamp tail. In normal mode +0xB8 is both response
// denominator and symmetric speed limit; special mode uses +0xAC for the
// zero-limit test and bypasses the final clamp exactly as the retail branch.
[[nodiscard]] RetailPlayerSpeedUpdate retailPlayerSpeedStep98800(
    float inputScalar,
    float previousAdjustment,
    float baseSignedSpeed,
    bool specialMode,
    const RetailPlayerSpeedConfig98800& config) noexcept;

}  // namespace jsrf
