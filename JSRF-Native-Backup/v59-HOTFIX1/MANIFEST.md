# JSRF Native v59 HOTFIX1

Fixes the Windows generation failure `Generated ADXT ring entry label(s) missing: 0x0013FAD0`.

Root cause: the v59 injector incorrectly required every retail target to have a redundant `loc_XXXXXXXX` label. xboxrecomp can emit a real function entry as `void sub_XXXXXXXX(void)` without a matching entry label. The injector now uses the exact function opening brace as a fallback anchor when the entry label is absent.

No guest state, ADXT state, dirty-disc behavior, or graphics behavior is modified by this hotfix; it only makes the diagnostic insertion robust.

Fresh verification on the packaged copy:
- Python: 213 passed
- Native CMake/Ninja build: passed
- CTest: 20/20 passed
- ZIP integrity: passed

Easy ZIP SHA-256: `746fb932df7179aef075f227e1073b791f2599569c5697e37fda63ccf2ad466b`
Patch SHA-256: `8c2757752b1e6fd98380ecfe2de60dbfb44ee1260f6911bc88f3fd31feea7c94`
