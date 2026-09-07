# JSRF native checkpoint v13

The v12 Windows run reproduced the later `0x0019E4DC` / `0xFFFFFFFC` crash, but its ABI trace exposed an earlier concrete corruption: retail target `0x00160080` returns with ESI/EDI changed and guest ESP four bytes lower than it entered. The very next measured event is the first null indirect call at guest return `0x0015FC99`, so this is upstream of the previously chased `0x00176630` failure.

Retail analysis proves `0x00160080` is a standalone method in the constructor-installed vtable at `0x001E1900`; constructors at `0x00160C4E` and `0x00161718` install that table. Every retail exit from `0x00160080..0x00160106` uses `ret 0x0C`, so the lifted body must return with `esp += 16; return; /* ret 12 */` including the synthetic return address.

v13 therefore pins `0x00160080..0x00160106` as a strong retail function boundary, seeds all 21 code pointers from vtable `0x001E1900`, and audits generated C before compilation. The build now aborts if either the new `0x00160080` `ret 0x0C` contract or the existing `0x00178A00` `ret 0x18` contract is lost. This changes only function-boundary recovery; no gameplay behavior is replaced.

Artifacts persisted in ChatGPT Library:
- `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v13.zip` (`libfile_223a42fe54c081918764934b17e8900c`), SHA-256 `43f152d7e7f6ae8f10339c04522912a7f4e4a7ece22bf3a45aaa9f4ab28f2495`
- `/JSRF/JSRF-Native-Source-Checkpoint-08-HOTFIX12.zip` (`libfile_06848cb8742881919ee9c6c1f144b130`), SHA-256 `9ae27f1927a809da9f1b2eaa3591f6a5999f3c00b2bbc7d28cfb901bb55ca430`
- `/JSRF/JSRF-V12-to-V13.patch` (`libfile_dfb40fc3727c8191839ce516a45a4df9`), SHA-256 `c139ed754d2c1cd2576ed39e7c808ae4ef36440449d1357d5f105958dbd31546`

Fresh verification before packaging: 20/20 Python diagnostic/launcher/integration tests, 20/20 CTest tests, and ZIP integrity passed. The Windows xboxrecomp runtime itself still requires the user's Windows test run.