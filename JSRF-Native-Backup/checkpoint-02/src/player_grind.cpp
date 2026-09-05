#include "player_grind.h"

namespace jsrf {
namespace {
constexpr std::uint32_t kActionBits = 0x3Fu;
constexpr std::uint32_t kPressedJump = 1u << 16u;
constexpr std::uint32_t kPressedTrick26To48 = 1u << 19u;
constexpr std::uint32_t kPressedTrick20To41 = 1u << 20u;
constexpr std::uint32_t kPressedSpecial = 1u << 21u;
constexpr float kAnalogScale = 1.0f / 65535.0f;
constexpr float kMinimumGrindSpeed = 25.0f / 27.0f;
constexpr float kBalanceAccelPerFrame = 0.00771604944f;
constexpr float kAcquireExtent = 20.0f;
}

PlayerInputFrame makePlayerInputFrame(
    std::uint32_t currentActions,
    std::uint32_t previousActions,
    std::uint16_t direction,
    std::uint16_t analogMagnitude) noexcept {
    const std::uint32_t held = currentActions & kActionBits;
    const std::uint32_t previous = previousActions & kActionBits;
    const std::uint32_t pressed = held & ~previous;

    return {
        held | (pressed << 16u),
        direction,
        static_cast<float>(analogMagnitude) * kAnalogScale,
    };
}

float applyRetailGrindEntrySpeed(
    float signedSpeed,
    float characterGrindMultiplier) noexcept {
    const float multiplied = signedSpeed * characterGrindMultiplier;
    if (multiplied >= 0.0f) {
        return multiplied < kMinimumGrindSpeed ? kMinimumGrindSpeed : multiplied;
    }
    return multiplied > -kMinimumGrindSpeed ? -kMinimumGrindSpeed : multiplied;
}

void beginRetailGrind(
    GrindRuntimeState& state,
    float signedSpeed,
    float characterGrindMultiplier) noexcept {
    state.signedSpeed = applyRetailGrindEntrySpeed(signedSpeed, characterGrindMultiplier);
    state.balanceAccelFrames = 0;
    state.balanceAccelPerFrame = kBalanceAccelPerFrame;
    state.trickTimer = 60;
    state.trickWindowStart = 0;
    state.trickWindowEnd = 0;
}

GrindFrameAction decideRetailGrindAction(
    const GrindFrameContext& context) noexcept {
    if ((context.actionMask & kPressedJump) != 0) {
        return context.alternateJump ? GrindFrameAction::JumpType3 : GrindFrameAction::JumpType2;
    }

    if ((context.actionMask & kPressedSpecial) != 0 &&
        context.specialActionEnabled &&
        context.signedSpeed > 0.0f &&
        context.specialActionGate) {
        return GrindFrameAction::SpecialAction;
    }

    if ((context.actionMask & kPressedTrick20To41) != 0) {
        return GrindFrameAction::TrickA;
    }
    if ((context.actionMask & kPressedTrick26To48) != 0) {
        return GrindFrameAction::TrickB;
    }
    return GrindFrameAction::None;
}

void applyRetailGrindTrick(
    GrindRuntimeState& state,
    GrindFrameAction action) noexcept {
    if (action == GrindFrameAction::TrickA) {
        state.balanceAccelFrames += 30;
        state.trickWindowStart = 20;
        state.trickWindowEnd = 41;
    } else if (action == GrindFrameAction::TrickB) {
        state.balanceAccelFrames += 30;
        state.trickWindowStart = 26;
        state.trickWindowEnd = 48;
    }
}

Aabb retailPlayerGrindAcquireBounds(Vec3 center) noexcept {
    return {
        {center.x - kAcquireExtent, center.y - kAcquireExtent, center.z - kAcquireExtent},
        {center.x + kAcquireExtent, center.y + kAcquireExtent, center.z + kAcquireExtent},
    };
}

}  // namespace jsrf
