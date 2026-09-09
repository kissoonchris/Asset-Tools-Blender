# JSRF v48 — fade-service diagnostics

## Why this is the next boundary

The v47 Windows run removes ambiguity above the completed cache manager. The
main title state reaches 8 and remains there. Its logo/bootstrap child runs a
120-frame state-1 timer, observes the cache-started flag, then transitions to
state 2 and remains there indefinitely.

Retail state 2 calls `0x24650` every update. `0x24650` requests service id 6 and
returns that service object's `+0xC0` field, or 1 if the service does not exist.
The persistent state-2 wait therefore means the real service exists and its
completion condition is not being observed as complete.

## Retail implementation

The fade command `0x24540` receives a packed 32-bit four-channel target and a
second argument used as the step. It clears `+0xC0`, converts each byte of the
packed target to a normalized float, stores targets at `+0xA8..+0xB4`, and
stores the step at `+0xB8`.

The update `0x24700` first exits immediately if `+0xC0` is already nonzero.
Otherwise it processes current channels at `+0x98/+0x9C/+0xA0/+0xA4` against
targets at `+0xA8/+0xAC/+0xB0/+0xB4`. Each channel either already compares
equal or moves by `+0xB8` and clamps at the target. At `0x24945`, ECX is the
number of channels considered complete; ECX == 4 sets `+0xC0 = 1`.

That makes three concrete possibilities distinguishable without guessing:

1. `0x24540` is never called for the transition the logo expects;
2. `0x24700` is not being scheduled/called after the command;
3. `0x24700` runs but current/target math or the four-channel equality count is
   wrong in the lifted code.

## Instrumentation

v48 logs only those boundaries. It does not alter the service, title state,
FPU state, guest registers, or guest memory.

The next Windows log should establish which of the three cases above is real.
Only then should a behavioral correction be made.
