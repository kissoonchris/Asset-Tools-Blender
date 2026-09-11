# JSRF Native v60 artifacts

Final one-click/source package (byte-identical ZIPs):
- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v60.zip`
- `JSRF-Native-Source-Checkpoint-08-FIXED-v60.zip`
- size: 478,141 bytes
- SHA-256: `29a2126c48ec7afc5606766aa8f60c5ae52f369a84f45ec81a3bba11ee6e792a`

Reconstruction patches:
- `JSRF-v59-HOTFIX1-to-v59-HOTFIX2.patch`
  - SHA-256: `f9dff5cb1b4ebaf0a4f8aa41c1ccd5ef976aa719917caf0e1aa11899cfd16114`
- `JSRF-v59-HOTFIX2-to-v60.patch`
  - SHA-256: `309af520e8662c640e89f449f5d563de9cefef812332c9ac86498572a180d212`
- combined gzip+base64 transport `PATCH-CHAIN.gz.b64`
  - SHA-256: `e4277cb6e3598d2c2778e9961e8e5cc24f6b8d0152eafba2e19c310dbe0b6fec`

The v60 backup branch is based on `jsrf-native-backup-v59-hotfix1-stage`.
Apply the two reconstruction patches in the order listed above to recover the
v60 source changes represented by this backup.
