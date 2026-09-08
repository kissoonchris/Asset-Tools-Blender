# JSRF Native v34 manifest

## Purpose

v34 corrects the JSRF DirectSound GP DSP command mailbox used by the asynchronous acknowledgement model introduced in v33.

Pinned xboxrecomp revision:

`e3caa3718c80a241b31768852fa752aa6577b66c`

## v33 Windows evidence

v33 configured `RECOMP_APU_DSP_ACK=0x8037B810`, but the runtime never printed an asynchronous acknowledgement for that address. DirectSound still reached its late GP setup and then the main retail path stopped visibly advancing while timer/vblank and worker activity continued.

The v33 log also records all allocations from the retail DirectSound descriptor-table allocator call site. Descriptor index 12 is the 0xC000-byte block at `0x803C0000`.

## Root cause

The previous address assumed that the GP command object used descriptor index 0 because that was the first matching 0x2000-byte contiguous allocation.

Retail code proves otherwise:

- `0x001A1712` constructs the command object with type `1`.
- `sub_001A52AF` stores that type at object `+8`.
- `sub_001A52F7` maps type `1` to descriptor offset `0xC0`, i.e. descriptor index 12 in the table at `0x001BA798`.
- `sub_001A1769` loads the selected descriptor's allocation base, adds `0x810`, writes command `3`, and spins until that dword becomes zero.

The correct mailbox is therefore:

`0x803C0000 + 0x810 = 0x803C0810`

## v34 change

`scripts/Build-JSRF-XboxRecomp.ps1` now defaults `RECOMP_APU_DSP_ACK` to `0x803C0810` instead of `0x8037B810`.

The asynchronous DSP worker itself is unchanged. No retail JSRF command, wait, gameplay, D3D, or allocator behavior is bypassed.

## Regression coverage

`tests/test_v34_dsound_gp_descriptor_mailbox.py` was written first and failed against v33 because the runner still targeted descriptor 0. It passes after the mailbox correction and also rejects the old `0x8037B810` address.

The older v32 test now checks only that the generic acknowledgement mechanism and diagnostic environment override remain enabled; exact mailbox selection is owned by the v34 regression.

## Next Windows evidence

Run the v34 easy-test package. The decisive success marker for this correction is an APU log line reporting command `0x00000003` acknowledged at `0x803C0810`, followed by retail execution leaving the `0x001A18D0` wait and exposing the next blocker.

## Fresh local verification

- v34 mailbox regression: passed after failing against v33.
- Full Python suite: **86 passed**.
- Fresh native CMake configure/build: **passed**.
- Native CTest suite: **20/20 passed**.
- Windows runtime verification remains required; this manifest does not claim the GP wait is cleared until the user's v34 run shows the `0x803C0810` command acknowledgement.
