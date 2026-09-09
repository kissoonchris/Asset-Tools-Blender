# JSRF native v42 backup artifacts

GitHub branch: `jsrf-native-backup-v42-stage`

The binary test/source ZIPs are backed up in the ChatGPT Library `/JSRF/` folder. This GitHub directory stores the manifest, runtime analysis, and a reconstructable compressed/base64 v41→v42 source patch.

Artifact SHA-256 values:

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v42.zip` — `7547e9cf2d794914df75c383d22ef5ddbd8e73aae8a73f6920c7f02041c82fb3`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX41.zip` — `7547e9cf2d794914df75c383d22ef5ddbd8e73aae8a73f6920c7f02041c82fb3`
- `JSRF-V41-to-V42.patch` — `08be3ca347dbeeebe8bf28f924e55ad1934b5ad780bfc39e414c921b9fb0ecab`

To reconstruct the patch from `V41-to-V42.patch.gz.b64` on a Unix-like shell:

```sh
base64 -d V41-to-V42.patch.gz.b64 | gzip -d > JSRF-V41-to-V42.patch
```

The v42 package was verified to contain `START JSRF TEST.bat` at archive root and not to contain `default.xbe` or other retail game payloads.
