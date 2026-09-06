#include "player_locomotion.h"

#include <bit>
#include <cstdint>
#include <cmath>

namespace jsrf {
namespace {
constexpr float rf(std::uint32_t bits) noexcept {
    return std::bit_cast<float>(bits);
}

// Literal retail XBE .rdata constants used by CPlayer @ 0x9D260/0x9D5E0.
constexpr float kSpeedQuarter = rf(0x3e6d097b);  // 0x1CD28C
constexpr float kSpeedHalf = rf(0x3eed097b);     // 0x1CD10C
constexpr float kSpeedFull = rf(0x3f6d097b);     // 0x1CD470
constexpr float kSpeedMaxBand = rf(0x3f9425ed);  // 0x1CD0B8
constexpr float kBlend432 = rf(0x408a3d71);      // 0x1CD500
constexpr float kBlend216 = rf(0x400a3d71);      // 0x1CD504
constexpr float kBlend144 = rf(0x3fb851ec);      // 0x1CD508
constexpr float kTurnScale = rf(0x392495f6);     // 0x1CD4FC
}

RetailPairedMotionBlend selectRetailPlayerPairedMotionBlend9D260(
    float signedSpeed) noexcept {
    // Retail uses fcomps + fnstsw/test/jp. For ordered values the branch is
    // taken on >=, so equality belongs to the next interval.
    if (signedSpeed < kSpeedHalf) {
        return {4, 4, 6, 6, 0.0f};
    }
    if (signedSpeed < kSpeedFull) {
        return {4, 5, 6, 7, (signedSpeed - kSpeedHalf) * kBlend216};
    }
    return {5, 5, 7, 7, 0.0f};
}

RetailMotionBlendBand selectRetailPlayerMotionBand9D260(
    float signedSpeed) noexcept {
    if (signedSpeed < kSpeedQuarter) {
        return {0, 0, 0.0f};
    }
    if (signedSpeed < kSpeedHalf) {
        return {0, 1, (signedSpeed - kSpeedQuarter) * kBlend432};
    }
    if (signedSpeed < kSpeedFull) {
        return {1, 2, (signedSpeed - kSpeedHalf) * kBlend216};
    }
    if (signedSpeed < kSpeedMaxBand) {
        return {2, 3, (signedSpeed - kSpeedFull) * kBlend432};
    }
    return {3, 3, 0.0f};
}

RetailMotionBlendBand selectRetailPlayerMotionBand9D5E0(
    float signedSpeed) noexcept {
    if (signedSpeed < kSpeedHalf) {
        return {2, 2, 0.0f};
    }
    if (signedSpeed < kSpeedMaxBand) {
        return {2, 3, (signedSpeed - kSpeedHalf) * kBlend144};
    }
    return {3, 3, 0.0f};
}

float retailPlayerMotionType4TurnBlend(std::int32_t signedAngleDelta) noexcept {
    // Avoid signed overflow for INT_MIN while preserving the retail NEG result
    // as an unsigned magnitude; values that large immediately hit the clamp.
    const std::uint32_t raw = static_cast<std::uint32_t>(signedAngleDelta);
    const std::uint32_t magnitude = signedAngleDelta < 0 ? (0u - raw) : raw;
    if (magnitude >= 0x18e3u) {
        return 1.0f;
    }
    return static_cast<float>(magnitude) * kTurnScale;
}

float retailPlayerAngleResponse98090(std::int32_t signedAngle) noexcept {
    constexpr float kAngleResponseScale = rf(0x38878946);  // 0x1CD468

    if (signedAngle < -0x38e) {
        if (signedAngle < -0x4000) {
            return -1.0f;
        }
        return static_cast<float>(signedAngle + 0x38e) * kAngleResponseScale;
    }
    if (signedAngle > 0x38e) {
        if (signedAngle > 0x4000) {
            return 1.0f;
        }
        return static_cast<float>(signedAngle - 0x38e) * kAngleResponseScale;
    }
    return 0.0f;
}

RetailPlayerSpeedUpdate retailPlayerSpeedFilter98867(
    float targetAdjustment,
    float previousAdjustment,
    float baseSignedSpeed,
    float responseNumerator,
    float responseDenominator,
    float signedSpeedLimit,
    bool bypassSpeedClamp) noexcept {
    float adjustment =
        ((targetAdjustment - previousAdjustment) / responseDenominator) *
            responseNumerator +
        previousAdjustment;

    // 0x98889..0x988C0: when the selected limit field is exactly zero,
    // the retail code removes sub-responseNumerator residual adjustment.
    if (signedSpeedLimit == 0.0f && std::fabs(adjustment) < responseNumerator) {
        adjustment = 0.0f;
    }

    float signedSpeed = baseSignedSpeed + adjustment;
    if (!bypassSpeedClamp) {
        // Preserve the retail comparison order instead of std::clamp.
        if (signedSpeed > signedSpeedLimit) {
            signedSpeed = signedSpeedLimit;
        }
        if (signedSpeed < -signedSpeedLimit) {
            signedSpeed = -signedSpeedLimit;
        }
    }

    return {adjustment, signedSpeed};
}

float retailPlayerInputScalar7F820(
    float controllerMagnitude,
    std::int32_t overrideCounter1170,
    std::int32_t overrideCounter1174) noexcept {
    if (overrideCounter1170 > 0 || overrideCounter1174 > 0) {
        return 1.0f;
    }
    return controllerMagnitude;
}

RetailPlayerSpeedUpdate retailPlayerSpeedStep98800(
    float inputScalar,
    float previousAdjustment,
    float baseSignedSpeed,
    bool specialMode,
    const RetailPlayerSpeedConfig98800& config) noexcept {
    if (specialMode) {
        const float targetAdjustment =
            inputScalar * config.specialTargetScale + config.specialTargetBias;
        return retailPlayerSpeedFilter98867(
            targetAdjustment,
            previousAdjustment,
            baseSignedSpeed,
            config.responseNumerator,
            config.responseDenominator,
            config.specialLimitField,
            true);
    }

    const float targetAdjustment =
        inputScalar * config.normalTargetScale + config.normalTargetBias;
    return retailPlayerSpeedFilter98867(
        targetAdjustment,
        previousAdjustment,
        baseSignedSpeed,
        config.responseNumerator,
        config.responseDenominator,
        config.responseDenominator,
        false);
}

}  // namespace jsrf
