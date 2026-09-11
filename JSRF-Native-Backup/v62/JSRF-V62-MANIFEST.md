# JSRF Native v62

v62 corrects a generic xboxrecomp EFLAGS provenance bug proven by the v61 JSRF
runtime capture.

The retail ADXT watchdog reaches its shared `jle` with runtime values `1` and
`1200`, but the generated branch falls through and writes error `-1`. The pinned
translator was throwing away CMP provenance at a basic-block merge because the
two predecessor CMP instructions use different static operands. The lifter then
used an always-zero `_flags` fallback.

Changes in v62:

- adds `patch_xboxrecomp_merged_cmp_flag_state()` to the one-click pipeline;
- allows same-setter, same-width CMP/TEST predecessor states to share the
  runtime `_fa/_fb/_fas/_fbs` snapshot across a CFG merge;
- preserves the old conservative behavior for incompatible or unknown states;
- adds v62 regression tests covering the exact differing-CMP merge and refusing
  mixed-width / mixed-setter merges;
- updates the runtime banner to `JSRF Native v62`;
- leaves ADXT data/status, retail watchdog logic, audio ring contents, file I/O,
  and graphics untouched.

See `research/JSRF_V62_MERGED_CMP_FLAG_STATE.md` for the proof and scope.
