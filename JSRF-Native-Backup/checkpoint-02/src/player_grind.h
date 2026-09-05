#pragma once

#include "stage_runtime.h"

#include <cstdint>

namespace jsrf {

struct PlayerInputFrame {
    std::uint32_t actionMask{};
    std::uint16_t direction{};
    float magnitude{};
};

[[nodiscard]] PlayerInputFrame makePlayerInputFrame(
    std::uint32_t currentActions,
    std::uint32_t previousActions,
    std::uint16_t direction,
    std::uint16_t analogMagnitude) noexcept;

[[nodiscard]] float applyRetailGrindEntrySpeed(
    float signedSpeed,
    float characterGrindMultiplier) noexcept;

struct GrindRuntimeState {
    float signedSpeed{};
    int balanceAccelFrames{};
    float balanceAccelPerFrame{};
    int trickTimer{};
    int trickWindowStart{};
    int trickWindowEnd{};
};

void beginRetailGrind(
    GrindRuntimeState& state,
    float signedSpeed,
    float characterGrindMultiplier) noexcept;

enum class GrindFrameAction {
    None,
    JumpType2,
    JumpType3,
    SpecialAction,
    TrickA,
    TrickB,
};

struct GrindFrameContext {
    std::uint32_t actionMask{};
    bool alternateJump{};
    bool specialActionEnabled{};
    bool specialActionGate{};
    float signedSpeed{};
};

[[nodiscard]] GrindFrameAction decideRetailGrindAction(
    const GrindFrameContext& context) noexcept;

void applyRetailGrindTrick(
    GrindRuntimeState& state,
    GrindFrameAction action) noexcept;

[[nodiscard]] Aabb retailPlayerGrindAcquireBounds(Vec3 center) noexcept;

}  // namespace jsrf
