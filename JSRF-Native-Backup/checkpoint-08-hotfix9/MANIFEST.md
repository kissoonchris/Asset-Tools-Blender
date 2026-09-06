# JSRF Native Checkpoint 08 Hotfix 9 / Easy Test v10

Local commit: `1329e9ab99531810d05fdffc0b1da9d22e597dec` — Recover JSRF vtable-only function boundaries.

## Root cause fixed

The v9 ABI diagnostics showed a concrete stack-cleanup failure in retail function `0x00154E00`. Its vtable-only callee at `0x00154D70` exits with `ret 0x0C`, but generic late data-pointer recovery was materializing this shape as an alias rather than a true standalone function. That lost the callee's retail cleanup and corrupted the caller stack. Two additional vtable-only entries with the same measured ABI-corruption pattern were also promoted to true function seeds: `0x00155380` and `0x0015A020`.

This is a static function-boundary correction only; it does not replace gameplay behavior.

The tester now also captures DbgHelp-reported generated-C source context into `JSRF_SOURCE_CONTEXT_LOG.txt` and appends it to the combined test log.

## Verification

- Python/integration tests: 16/16 passed
- Native research CTest suite: 20/20 passed
- ZIP integrity: passed
- `git diff --check`: clean

## Artifacts

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v10.zip`
  - SHA-256: `51eb0ef73062398ba3188c8b02cab103c210f5021c43d4d1d9611b63318c31fc`
  - Library: `/JSRF/JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v10.zip`
  - Library ID: `libfile_7a53c7479410819196f6ee134c7c9b11`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX9.zip`
  - SHA-256: `51eb0ef73062398ba3188c8b02cab103c210f5021c43d4d1d9611b63318c31fc`
  - Library ID: `libfile_e4f4bfb372bc8191abda22ec95e21772`
- `JSRF-Native-Checkpoint-08-HOTFIX9.git.bundle`
  - SHA-256: `9db4c3ed34c0d26c9f5bd9bab4a01fb3c655318d6353fdb77fce0cd073f0669c`
  - Library ID: `libfile_8e7cebc6d90481919eff789671d0ccd3`
- `JSRF-Checkpoint-08-HOTFIX9.patch`
  - SHA-256: `3ad5ef952e241c95137b6e0ada437a58b7e31e8b4bb0ee1457e53e4d4e8d29fc`
  - Library ID: `libfile_5b5e53b728dc8191b31945d3e4c2419b`
