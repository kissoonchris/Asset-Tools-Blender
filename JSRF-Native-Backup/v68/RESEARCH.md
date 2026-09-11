# JSRF v68 — WX completion/status and timeout-cleanup evidence

## v67 Windows result

v67 confirms the result-ZF translator correction fixed the previous request
submission bug. The first title ADX request reaches WX `0x001403B0`, which now
returns `0x19` (25 sectors); `0x0013DD70` propagates the same positive result.
The host then reads 51,200/51,200 bytes from offset zero successfully.

The producer ring no longer cancels immediately, but it also never becomes
filled. After the read, the ring is reserved (`free=0`) while `filled=0`,
`read_pos=0`, and `write_pos=0`. ADXT eventually reaches its legitimate
1201-count watchdog failure. This places the primary blocker after request
submission, in the asynchronous status/completion path.

## Secondary cleanup fault

Once the ADXT timeout path begins cleanup, the generated caller at retail
`0x0013BE4F` pushes one argument and calls `0x0013B610`. Runtime ABI diagnostics
show that call changes ESP from `0x011EAF78` to `0x011EAF98` and also corrupts
callee-saved EBX/ESI/EDI. The next loop iteration reaches generated source line
120416 with ESP already above the worker stack top and the stack guard exits.

This cleanup fault is important, but it occurs after the primary title-audio
stall. Fixing it blindly could hide the actual completion-path problem.

## v68 diagnostic

v68 therefore changes no guest behavior. It captures the untouched codegen and
retail bytes needed to compare both problems:

- WX status/completion: `0x001405B0..0x001406D0`
- cleanup callee: `0x0013B610..0x0013B810`
- cleanup caller: `0x0013BDF0..0x0013BE70`

A sparse `[WXSTAT]` probe is inserted at `0x001405B0` entry and before every
generated retail RET. It records the live return register, original object
argument, state byte, request/progress fields, and asynchronous fields without
writing guest registers or memory.

The next Windows run can therefore answer two concrete questions:

1. Does the status routine observe the host-completed request and return the
   expected state, or is its generated control flow wrong?
2. Does `0x0013B610` contain a genuine callee-cleanup RET/epilogue, or did
   function-boundary/ABI translation make it over-clean the caller stack?
