# JSRF Native v53 artifacts

Fresh verification from a clean extraction of the final easy-test ZIP:

- focused v53 Python tests: 6 passed
- complete Python suite: 180 passed
- Python compile check: passed
- fresh CMake/Ninja configure and build: passed
- native CTest suite: 20/20 passed
- root `START JSRF TEST.bat`: present
- retail `.xbe` payload scan: clean
- ZIP integrity: passed
- flip-patch C fixture compiled with `-Wall -Wextra -Werror`: passed
- v52-to-v53 patch applied cleanly to a fresh v52 source extraction

Artifacts:

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v53.zip` — 445,308 bytes — SHA-256 `33c00ddd9a01cfc2b5c616c93310b5ec2285da709096298a2d7abade2028fb75`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX52.zip` — 445,308 bytes — SHA-256 `33c00ddd9a01cfc2b5c616c93310b5ec2285da709096298a2d7abade2028fb75`
- `JSRF-V52-to-V53.patch` — 28,021 bytes — SHA-256 `c00278048fe2b11a45be616b8cf5c0035dc7965978104d91e87f7d4ad9432269`

The two ZIPs are byte-identical full source packages.
