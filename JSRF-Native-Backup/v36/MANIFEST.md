# JSRF Native v36 manifest

## Purpose

v36 fixes the next static-recompilation boundary defect exposed by the user's v35 Windows run: the vtable method at `0x0007E180` returns without consuming the virtual call's synthetic guest return address, which then corrupts the caller `0x00011070`.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v35 Windows evidence

v35 still confirms the previously fixed DirectSound path and progresses through the real retail startup code. The new stable failure is more precise:

- `0x0007E180` ABI entry/exit ESP: `0x00F7FEE8 -> 0x00F7FEE8`
- immediately afterward `0x00011070` returns with ESI changed from `0x01080FA0` to `0x0001108A`
- `0x0001108A` is the retail instruction immediately after `call dword ptr [eax+4]` inside `0x00011070`
- the caller then reaches a null vtable target and later faults in `sub_00011070` with a wild ESI pointer

This establishes that `0x00011070` is downstream: its vtable slot `+4` callee left return address `0x0001108A` on the guest stack.

## Root cause addressed by v36

Retail `0x0007E180` is one method with three real plain-`ret` exits at `0x0007E196`, `0x0007E241`, and `0x0007E256`; padding follows until the next function at `0x0007E260`.

Independent retail object evidence confirms the entry:

- constructor `0x0007E2A4` installs vtable `0x001CCF78`
- vtable slot `+4` at `0x001CCF7C` contains literal pointer `0x0007E180`
- runtime caller return address is `0x0001108A`

v36 pins `[0x0007E180, 0x0007E260)` and removes false internal starts before recompilation.

## Generated-code safety check

The Windows pipeline now refuses to run unless generated `sub_0007E180` contains all three retail `esp += 4; return; /* ret */` exits.

## Regression coverage

`tests/test_v36_vtable_method_boundary.py` was written first. Against v35 it failed **4/4** because the retail boundary was absent, an internal `0x0007E242` split survived, and the generated-contract verifier did not know about the method. After the v36 implementation the focused suite passes **4/4**.

## Fresh local verification

- Focused v36 regression: **4 passed**
- Full Python suite: **95 passed**
- Python compile check: **passed**
- Fresh native CMake configure/build: **passed**
- Native CTest suite: **20/20 passed**
- Windows runtime verification is still required. This manifest does not claim the access violation is fixed until the user's v36 run confirms `0x0007E180` consumes its return address and `0x00011070` preserves ESI.
