# JSRF Native v56 artifacts

## Easy test package
- File: `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v56.zip`
- Size: 451442 bytes
- SHA-256: `6c23fd6bd90ff7c50cb0edd5ae17ddcd88d2b6b003ff583e4505a47cd6fdc53b`

## Source checkpoint
- File: `JSRF-Native-Source-Checkpoint-08-FIXED-v56.zip`
- Size: 451442 bytes
- SHA-256: `6c23fd6bd90ff7c50cb0edd5ae17ddcd88d2b6b003ff583e4505a47cd6fdc53b`
- Byte-identical to the easy-test package.

## v55 -> v56 patch
- File: `JSRF-V55-to-V56.patch`
- Size: 21900 bytes
- SHA-256: `c10ab4863536fc7979a7b30b69bde1474b2987b1efaa075c069454d630cb5596`
- GitHub transport: `JSRF-V55-to-V56.patch.gz.b64`

## Fresh final-package verification
The actual v56 easy-test ZIP was extracted into a clean directory after packaging and verified:
- ZIP integrity: passed
- root `START JSRF TEST.bat`: present
- Python regression suite: 197 passed
- Python compile check: passed
- fresh CMake configure/build: passed
- native CTest: 20/20 passed

Windows JSRF runtime confirmation is still required. v56 intentionally does not force the dirty-disc branch or resource completion; it adds targeted diagnostics and reduces high-volume default tracing around the retail timeout.
