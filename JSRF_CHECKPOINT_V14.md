# JSRF native checkpoint v14

v13's `0x00160080` boundary was two bytes short. Exact retail bytes show the final `ret 0x0C` is `C2 0C 00` at `0x00160105..0x00160107`; v13 ended the function at `0x00160106`, so generated code restored ESI/EDI but never executed that final return. This left guest ESP 16 bytes low and directly caused the next `0x00000002` indirect call at `0x0015FC99`.

v14 pins the correct exclusive end `0x00160108` and requires generated `sub_00160080` to retain all three retail `ret 0x0C` exits.

Artifacts:
- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v14.zip` — `libfile_9b1040a8f3f48191a5e63dac57b5e70c` — SHA-256 `92933fd44c344739f6759e51923364468e40cac8a39bec93f408a2be8821f3c7`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX13.zip` — `libfile_5cdd44aff8d08191975cc2c81997754a` — same SHA-256
- `/JSRF/JSRF-V13-to-V14.patch` — `libfile_9c8f01fbe8b48191a1069a9c9ba09459` — SHA-256 `f7e9c2a89f744b8e31d77abacb8d2a43f677f570f5a4b7ee3c975f0914cc9c53`

Verification: 17/17 xboxrecomp tests, 4/4 PowerShell launcher tests, 21/21 full Python tests, 20/20 CTest, py_compile, exact retail-byte sanity, ZIP integrity all passed.
