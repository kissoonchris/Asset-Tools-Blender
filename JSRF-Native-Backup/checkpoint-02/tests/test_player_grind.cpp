#include "player_grind.h"

#include <cmath>
#include <cstdint>
#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}
bool near(float a, float b, float eps = 1.0e-6f) {
    return std::fabs(a - b) <= eps;
}
}

int main() {
    using namespace jsrf;

    const PlayerInputFrame input = makePlayerInputFrame(0b001011u, 0b001001u, 0xC123u, 32768u);
    check(input.actionMask == (0b001011u | (0b000010u << 16u)), "newly pressed actions should mirror into bits 16..21");
    check(input.direction == 0xC123u, "retail 16-bit direction should be preserved");
    check(near(input.magnitude, 32768.0f / 65535.0f), "analog magnitude should use 1/65535 normalization");

    const PlayerInputFrame masked = makePlayerInputFrame(0xFFu, 0u, 0u, 65535u);
    check((masked.actionMask & 0xFFFFu) == 0x3Fu, "only six retail action bits survive");
    check((masked.actionMask & 0x3F0000u) == 0x3F0000u, "pressed bits occupy 0x10000..0x200000");
    check(near(masked.magnitude, 1.0f), "maximum analog magnitude should normalize to one");

    constexpr float minGrindSpeed = 25.0f / 27.0f;
    check(near(applyRetailGrindEntrySpeed(0.25f, 1.0f), minGrindSpeed), "slow positive entry -> +25/27");
    check(near(applyRetailGrindEntrySpeed(-0.25f, 1.0f), -minGrindSpeed), "slow negative entry -> -25/27");
    check(near(applyRetailGrindEntrySpeed(0.0f, 1.0f), minGrindSpeed), "zero entry follows positive minimum branch");
    check(near(applyRetailGrindEntrySpeed(1.2f, 1.5f), 1.8f), "fast positive entry preserves multiplied speed");
    check(near(applyRetailGrindEntrySpeed(-1.2f, 1.5f), -1.8f), "fast negative entry preserves multiplied speed");

    GrindRuntimeState grind{};
    beginRetailGrind(grind, 0.25f, 1.0f);
    check(near(grind.signedSpeed, minGrindSpeed), "entry stores retail minimum signed speed");
    check(grind.balanceAccelFrames == 0, "entry clears temporary acceleration frames");
    check(near(grind.balanceAccelPerFrame, 0.00771604944f), "entry uses retail balance acceleration");
    check(grind.trickTimer == 60 && grind.trickWindowStart == 0 && grind.trickWindowEnd == 0, "entry initializes retail timer/window state");

    GrindFrameContext ctx{};
    ctx.actionMask = (1u << 16u) | (1u << 21u) | (1u << 19u);
    ctx.specialActionEnabled = true;
    ctx.specialActionGate = true;
    ctx.signedSpeed = 1.0f;
    check(decideRetailGrindAction(ctx) == GrindFrameAction::JumpType2, "jump has highest priority");
    ctx.alternateJump = true;
    check(decideRetailGrindAction(ctx) == GrindFrameAction::JumpType3, "alternate jump selects type 3");

    ctx.actionMask = (1u << 21u) | (1u << 20u);
    ctx.alternateJump = false;
    check(decideRetailGrindAction(ctx) == GrindFrameAction::SpecialAction, "gated press-bit 5 precedes tricks");
    ctx.specialActionEnabled = false;
    check(decideRetailGrindAction(ctx) == GrindFrameAction::TrickA, "press-bit 4 selects retail 20..41 branch");
    ctx.actionMask = (1u << 19u);
    check(decideRetailGrindAction(ctx) == GrindFrameAction::TrickB, "press-bit 3 selects retail 26..48 branch");

    applyRetailGrindTrick(grind, GrindFrameAction::TrickA);
    check(grind.balanceAccelFrames == 30 && grind.trickWindowStart == 20 && grind.trickWindowEnd == 41, "first trick adds 30 frames and 20..41 window");
    applyRetailGrindTrick(grind, GrindFrameAction::TrickB);
    check(grind.balanceAccelFrames == 60 && grind.trickWindowStart == 26 && grind.trickWindowEnd == 48, "second trick adds 30 frames and 26..48 window");

    const Aabb acquire = retailPlayerGrindAcquireBounds({10.0f, 20.0f, -30.0f});
    check(near(acquire.min.x, -10.0f) && near(acquire.min.y, 0.0f) && near(acquire.min.z, -50.0f) &&
          near(acquire.max.x, 30.0f) && near(acquire.max.y, 40.0f) && near(acquire.max.z, -10.0f), "retail player grind acquisition uses 20-unit extent");

    if (failures) return 1;
    std::cout << "player grind tests passed\n";
    return 0;
}
