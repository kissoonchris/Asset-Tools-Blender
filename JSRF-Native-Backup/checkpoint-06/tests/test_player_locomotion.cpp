#include "player_locomotion.h"

#include <bit>
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
constexpr float rf(std::uint32_t bits) {
    return std::bit_cast<float>(bits);
}
}

int main() {
    using namespace jsrf;

    constexpr float speedQuarter = rf(0x3e6d097b);
    constexpr float speedHalf = rf(0x3eed097b);
    constexpr float speedFull = rf(0x3f6d097b);
    constexpr float speedMaxBand = rf(0x3f9425ed);
    constexpr float blend432 = rf(0x408a3d71);
    constexpr float blend216 = rf(0x400a3d71);
    constexpr float blend144 = rf(0x3fb851ec);

    const RetailPairedMotionBlend lowPair = selectRetailPlayerPairedMotionBlend9D260(speedHalf - 0.01f);
    check(lowPair.firstFrom == 4 && lowPair.firstTo == 4 &&
          lowPair.secondFrom == 6 && lowPair.secondTo == 6 &&
          near(lowPair.blend, 0.0f),
          "0x9D260 should use direct motion slots 4 and 6 below 25/54");

    const RetailPairedMotionBlend halfBoundary = selectRetailPlayerPairedMotionBlend9D260(speedHalf);
    check(halfBoundary.firstFrom == 4 && halfBoundary.firstTo == 5 &&
          halfBoundary.secondFrom == 6 && halfBoundary.secondTo == 7 &&
          near(halfBoundary.blend, 0.0f),
          "0x9D260 equality at 25/54 should enter the retail blend branch");

    const float pairMidSpeed = (speedHalf + speedFull) * 0.5f;
    const RetailPairedMotionBlend midPair = selectRetailPlayerPairedMotionBlend9D260(pairMidSpeed);
    check(midPair.firstFrom == 4 && midPair.firstTo == 5 &&
          midPair.secondFrom == 6 && midPair.secondTo == 7 &&
          near(midPair.blend, (pairMidSpeed - speedHalf) * blend216),
          "0x9D260 paired motion blend should use the retail (speed-25/54)*2.16 factor");

    const RetailPairedMotionBlend fullBoundary = selectRetailPlayerPairedMotionBlend9D260(speedFull);
    check(fullBoundary.firstFrom == 5 && fullBoundary.firstTo == 5 &&
          fullBoundary.secondFrom == 7 && fullBoundary.secondTo == 7 &&
          near(fullBoundary.blend, 0.0f),
          "0x9D260 equality at 25/27 should use direct motion slots 5 and 7");

    const RetailMotionBlendBand negativeBand = selectRetailPlayerMotionBand9D260(-1.0f);
    check(negativeBand.from == 0 && negativeBand.to == 0 && near(negativeBand.blend, 0.0f),
          "0x9D260 must compare the signed CPlayer+0xCD4 speed directly, not its magnitude");

    const RetailMotionBlendBand belowQuarter = selectRetailPlayerMotionBand9D260(speedQuarter - 0.01f);
    check(belowQuarter.from == 0 && belowQuarter.to == 0 && near(belowQuarter.blend, 0.0f),
          "0x9D260 should hold band 0 below 25/108");

    const RetailMotionBlendBand quarterBoundary = selectRetailPlayerMotionBand9D260(speedQuarter);
    check(quarterBoundary.from == 0 && quarterBoundary.to == 1 && near(quarterBoundary.blend, 0.0f),
          "0x9D260 equality at 25/108 should enter the 0-to-1 blend branch");

    const float quarterMidSpeed = (speedQuarter + speedHalf) * 0.5f;
    const RetailMotionBlendBand quarterMid = selectRetailPlayerMotionBand9D260(quarterMidSpeed);
    check(quarterMid.from == 0 && quarterMid.to == 1 &&
          near(quarterMid.blend, (quarterMidSpeed - speedQuarter) * blend432),
          "0x9D260 first locomotion interval should use the retail 4.32 multiplier");

    const RetailMotionBlendBand halfBandBoundary = selectRetailPlayerMotionBand9D260(speedHalf);
    check(halfBandBoundary.from == 1 && halfBandBoundary.to == 2 && near(halfBandBoundary.blend, 0.0f),
          "0x9D260 equality at 25/54 should enter the 1-to-2 band");

    const float halfMidSpeed = (speedHalf + speedFull) * 0.5f;
    const RetailMotionBlendBand halfMid = selectRetailPlayerMotionBand9D260(halfMidSpeed);
    check(halfMid.from == 1 && halfMid.to == 2 &&
          near(halfMid.blend, (halfMidSpeed - speedHalf) * blend216),
          "0x9D260 middle locomotion interval should use the retail 2.16 multiplier");

    const RetailMotionBlendBand fullBandBoundary = selectRetailPlayerMotionBand9D260(speedFull);
    check(fullBandBoundary.from == 2 && fullBandBoundary.to == 3 && near(fullBandBoundary.blend, 0.0f),
          "0x9D260 equality at 25/27 should enter the 2-to-3 band");

    const float upperMidSpeed = (speedFull + speedMaxBand) * 0.5f;
    const RetailMotionBlendBand upperMid = selectRetailPlayerMotionBand9D260(upperMidSpeed);
    check(upperMid.from == 2 && upperMid.to == 3 &&
          near(upperMid.blend, (upperMidSpeed - speedFull) * blend432),
          "0x9D260 upper locomotion interval should use the retail 4.32 multiplier");

    const RetailMotionBlendBand maxBoundary = selectRetailPlayerMotionBand9D260(speedMaxBand);
    check(maxBoundary.from == 3 && maxBoundary.to == 3 && near(maxBoundary.blend, 0.0f),
          "0x9D260 equality at the 0x1CD0B8 speed threshold should collapse to band 3");

    const RetailMotionBlendBand altLow = selectRetailPlayerMotionBand9D5E0(speedHalf - 0.01f);
    check(altLow.from == 2 && altLow.to == 2 && near(altLow.blend, 0.0f),
          "0x9D5E0 should hold band 2 below 25/54");

    const RetailMotionBlendBand altHalfBoundary = selectRetailPlayerMotionBand9D5E0(speedHalf);
    check(altHalfBoundary.from == 2 && altHalfBoundary.to == 3 && near(altHalfBoundary.blend, 0.0f),
          "0x9D5E0 equality at 25/54 should enter its 2-to-3 blend branch");

    const float altMidSpeed = (speedHalf + speedMaxBand) * 0.5f;
    const RetailMotionBlendBand altMid = selectRetailPlayerMotionBand9D5E0(altMidSpeed);
    check(altMid.from == 2 && altMid.to == 3 &&
          near(altMid.blend, (altMidSpeed - speedHalf) * blend144),
          "0x9D5E0 should use the retail (speed-25/54)*1.44 factor");

    const RetailMotionBlendBand altMax = selectRetailPlayerMotionBand9D5E0(speedMaxBand);
    check(altMax.from == 3 && altMax.to == 3 && near(altMax.blend, 0.0f),
          "0x9D5E0 equality at the upper threshold should collapse to band 3");

    constexpr float turnScale = rf(0x392495f6);
    check(near(retailPlayerMotionType4TurnBlend(0), 0.0f),
          "zero retail signed contact/direction angle should produce zero motion-type-4 blend");
    check(near(retailPlayerMotionType4TurnBlend(-1000), 1000.0f * turnScale),
          "motion-type-4 turn blend should use absolute signed angle before scaling");
    check(near(retailPlayerMotionType4TurnBlend(0x18e2), 0x18e2 * turnScale),
          "motion-type-4 turn blend should scale the final value below the retail 0x18E3 clamp");
    check(near(retailPlayerMotionType4TurnBlend(0x18e3), 1.0f),
          "motion-type-4 turn blend should clamp to one at exactly 0x18E3");
    check(near(retailPlayerMotionType4TurnBlend(-0x18e3), 1.0f),
          "motion-type-4 turn blend should apply the same clamp to negative angle deltas");

    constexpr float angleResponseScale = rf(0x38878946);
    check(near(retailPlayerAngleResponse98090(0), 0.0f), "0x98090 should return zero at a zero angle");
    check(near(retailPlayerAngleResponse98090(0x38e), 0.0f), "0x98090 should include +0x38E in the retail deadzone");
    check(near(retailPlayerAngleResponse98090(-0x38e), 0.0f), "0x98090 should include -0x38E in the retail deadzone");
    check(near(retailPlayerAngleResponse98090(0x38f), 1.0f * angleResponseScale), "0x98090 should begin positive response one angle unit above the deadzone");
    check(near(retailPlayerAngleResponse98090(-0x38f), -1.0f * angleResponseScale), "0x98090 should begin negative response one angle unit below the deadzone");
    check(near(retailPlayerAngleResponse98090(0x2000), (0x2000 - 0x38e) * angleResponseScale), "0x98090 should linearly scale positive in-range angles with the retail scalar");
    check(near(retailPlayerAngleResponse98090(-0x2000), (-0x2000 + 0x38e) * angleResponseScale), "0x98090 should linearly scale negative in-range angles with the retail scalar");
    check(near(retailPlayerAngleResponse98090(0x4000), (0x4000 - 0x38e) * angleResponseScale), "0x98090 should still use the linear branch at exactly +0x4000");
    check(near(retailPlayerAngleResponse98090(-0x4000), (-0x4000 + 0x38e) * angleResponseScale), "0x98090 should still use the linear branch at exactly -0x4000");
    check(near(retailPlayerAngleResponse98090(0x4001), 1.0f), "0x98090 should saturate to +1 beyond +0x4000");
    check(near(retailPlayerAngleResponse98090(-0x4001), -1.0f), "0x98090 should saturate to -1 beyond -0x4000");

    const RetailPlayerSpeedUpdate smooth = retailPlayerSpeedFilter98867(1.0f, 0.0f, 0.25f, 0.2f, 2.0f, 1.0f, false);
    check(near(smooth.adjustment, 0.1f) && near(smooth.signedSpeed, 0.35f),
          "0x98867 should approach target adjustment by (delta/denominator)*numerator");

    const RetailPlayerSpeedUpdate upperClamp = retailPlayerSpeedFilter98867(2.0f, 0.0f, 0.9f, 1.0f, 1.0f, 1.25f, false);
    check(near(upperClamp.adjustment, 2.0f) && near(upperClamp.signedSpeed, 1.25f),
          "0x98867 should clamp non-bypass signed speed to the positive retail limit");

    const RetailPlayerSpeedUpdate lowerClamp = retailPlayerSpeedFilter98867(-2.0f, 0.0f, -0.9f, 1.0f, 1.0f, 1.25f, false);
    check(near(lowerClamp.adjustment, -2.0f) && near(lowerClamp.signedSpeed, -1.25f),
          "0x98867 should clamp non-bypass signed speed to the negative retail limit");

    const RetailPlayerSpeedUpdate bypass = retailPlayerSpeedFilter98867(2.0f, 0.0f, 0.9f, 1.0f, 1.0f, 1.25f, true);
    check(near(bypass.signedSpeed, 2.9f),
          "0x98867 should skip the final speed clamp when CPlayer+0xE60 is active");

    const RetailPlayerSpeedUpdate zeroLimitDeadzone = retailPlayerSpeedFilter98867(0.05f, 0.0f, 0.0f, 0.1f, 1.0f, 0.0f, false);
    check(near(zeroLimitDeadzone.adjustment, 0.0f) && near(zeroLimitDeadzone.signedSpeed, 0.0f),
          "0x98867 should zero adjustment when limit is zero and abs(adjustment) is below response numerator");

    const RetailPlayerSpeedUpdate zeroLimitBoundary = retailPlayerSpeedFilter98867(1.0f, 0.0f, 0.0f, 0.1f, 1.0f, 0.0f, true);
    check(near(zeroLimitBoundary.adjustment, 0.1f),
          "0x98867 deadzone comparison should be strict, using the post-filter adjustment");

    check(near(retailPlayerInputScalar7F820(0.375f, 0, 0), 0.375f),
          "0x7F820 should return controller +0x8C0 while both override counters are inactive");
    check(near(retailPlayerInputScalar7F820(0.375f, 1, 0), 1.0f),
          "0x7F820 should force one while CPlayer+0x1170 is positive");
    check(near(retailPlayerInputScalar7F820(0.375f, 0, 1), 1.0f),
          "0x7F820 should force one while CPlayer+0x1174 is positive");
    check(near(retailPlayerInputScalar7F820(0.375f, -1, -1), 0.375f),
          "0x7F820 should only treat positive override counters as active");

    RetailPlayerSpeedConfig98800 speedConfig{};
    speedConfig.normalTargetScale = 2.0f;
    speedConfig.normalTargetBias = -0.25f;
    speedConfig.specialTargetScale = 3.0f;
    speedConfig.specialTargetBias = 0.5f;
    speedConfig.responseNumerator = 0.25f;
    speedConfig.responseDenominator = 1.5f;
    speedConfig.specialLimitField = 2.0f;

    const RetailPlayerSpeedUpdate normalStep = retailPlayerSpeedStep98800(0.5f, 0.0f, 0.2f, false, speedConfig);
    const float normalTarget = 0.5f * 2.0f - 0.25f;
    const float normalAdjustment = (normalTarget / 1.5f) * 0.25f;
    check(near(normalStep.adjustment, normalAdjustment) && near(normalStep.signedSpeed, 0.2f + normalAdjustment),
          "0x98800 normal branch should use config +0x118/+0x10C then movement-row +0xB4/+0xB8");

    const RetailPlayerSpeedUpdate normalClampStep = retailPlayerSpeedStep98800(1.0f, 0.0f, 1.45f, false, speedConfig);
    check(near(normalClampStep.signedSpeed, speedConfig.responseDenominator),
          "0x98800 normal branch should use movement-row +0xB8 as its final symmetric speed limit");

    const RetailPlayerSpeedUpdate specialStep = retailPlayerSpeedStep98800(0.5f, 0.0f, 1.45f, true, speedConfig);
    const float specialTarget = 0.5f * 3.0f + 0.5f;
    const float specialAdjustment = (specialTarget / 1.5f) * 0.25f;
    check(near(specialStep.adjustment, specialAdjustment) && near(specialStep.signedSpeed, 1.45f + specialAdjustment),
          "0x98800 special branch should use config +0x110/+0x114 and bypass the final speed clamp");

    if (failures) return 1;
    std::cout << "player locomotion tests passed\n";
    return 0;
}
