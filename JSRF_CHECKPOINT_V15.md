# JSRF native checkpoint v15

v14 fixed the previous `0x00160080` stack corruption: the Windows run no longer crashed and instead survived until the 20-second xboxrecomp watchdog diagnostic exit.

The sample lands during DirectInput8 initialization. Recent indirect targets show the input COM vtable followed by `0x0015FF10`; retail code proves `0x0015FF10` calls DirectInput8 creation at `0x00166760` (version 0x800, DirectInput8 CLSID/IID). The stack also carries repeated sites from retail heap allocator `0x001497DC`.

Because a watchdog timeout is not itself proof of a hang, v15 makes no synchronization/input/gameplay substitutions. It extends the watchdog to 60 seconds and enables high-budget kernel, raw critical-section, argument, and targeted function entry/exit tracing for the DirectInput/heap path.

Artifacts are persisted in `/JSRF` Library as Easy Test v15, Source HOTFIX14, and the v14->v15 patch. Fresh verification: 23/23 Python tests, 20/20 CTest, py_compile, and ZIP integrity all passed.
