# JSRF native checkpoint v11

Local source commit: `5051c79479caa1c57c13764a73c6bca21f21d598` (`Recover JSRF DirectXFile vtable method boundary`).

v10 runtime evidence identified an upstream guest-stack corruption before the later `0x0019E4DC` crash. ABI diagnostics named retail function `0x00176630`; static retail resolution of its interface/vtable path proves the indirect call at `0x00176673` targets standalone vtable-only method `0x00178A00` via vtable `0x001E4020` slot `+0x0C`. Every retail exit from `0x00178A00` uses `ret 0x18`, matching the six 32-bit arguments pushed by the caller. v11 seeds `0x00178A00` before function construction so that cleanup is preserved.

Artifacts persisted in ChatGPT Library:
- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v11.zip` (`libfile_7cc5581be7208191b1e3aa6d7e04c6d7`), SHA-256 `3974813d3bce3df7e94a0d6fb1cb6d56b92f2741f531392a87b134e4cbb63f62`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX10.zip` (`libfile_89890525ac1481919b65d0b1eb28b01a`)
- `/JSRF/JSRF-Native-Checkpoint-08-HOTFIX10.git.bundle` (`libfile_3683c9e0d45081918042932fc1e1326d`), SHA-256 `290b36f332a0f3ac3a6d823ce174e47fb2ea50b192b8c23ad12d293737e74d40`
- `/JSRF/JSRF-Checkpoint-08-HOTFIX10.patch` (`libfile_522a50bd63c88191b725c7a1ca2d4a0c`), SHA-256 `294a4db845aaa19f33188a56032c9d369b87e81251148055fd6750876d9ce146`

Fresh verification before packaging: 16/16 Python diagnostic/launcher tests, 20/20 CTest tests, ZIP integrity passed, git bundle verified, working tree clean.
