# Stage 01 — Engineering Error Log

This is a **reproducible troubleshooting record**, not a list of hypothetical bugs. Each entry identifies an observed symptom from the Debian 13 / NUCLEO-F446RE development sessions, the established or likely cause, the applied fix, and evidence of the outcome. Unconfirmed root causes are explicitly identified.

## ERR-001 — `make: *** No targets. Stop.`

- **Category:** Build system / GNU Make
- **Observed command:** `make` from `~/Desktop/stmrtos/stm32-linux`
- **Error:** `make: *** No targets. Stop.`
- **Cause:** Make found a file named `Makefile` but no usable build target was defined. The contents had not yet been correctly populated/saved.
- **Resolution:** Add explicit `all`, `flash`, `debug-server`, `debug`, and `clean` targets to `Makefile`. Recipe commands must begin with a **TAB**, not spaces.
- **Verification:** A later `make` invocation produced ELF/BIN/HEX/MAP artifacts successfully.
- **Prevention:** `cat -n Makefile`, `make -n`, and `make clean && make` before attempting flash.

## ERR-002 — OpenOCD `Error: open failed`

- **Category:** Probe connection / USB access
- **Observed command:** `openocd -f interface/stlink.cfg -f target/stm32f4x.cfg`
- **Error:** `Error: open failed`
- **Root cause:** **Not conclusively determined for the original occurrence.** Possibilities include temporary USB/device access conflicts and permissions, but do not infer a permission problem without testing.
- **Investigation:** `lsusb` later showed `0483:374B ST-LINK/V2.1`; a subsequent OpenOCD session successfully examined the Cortex-M4.
- **Resolution:** Retry after verifying USB enumeration and checking debugger usage. If needed, compare `sudo openocd` as a *diagnostic only* and fix udev permissions rather than routinely using root.
- **Verification:** OpenOCD later reported `Cortex-M4 ... detected`, `Examination succeeded`, and GDB port 3333 listening.

## ERR-003 — `** OpenOCD init failed **` during `make flash`

- **Category:** Competing OpenOCD processes
- **Observed command:** `make flash`
- **Error:** `** OpenOCD init failed **`, followed by `make: *** [Makefile:58: flash] Error 1`
- **Cause:** An earlier OpenOCD server was still running, keeping the ST-LINK connection in use. `pgrep -a openocd` identified PID `424052`.
- **Resolution:** `kill 424052`, confirm `pgrep -a openocd` returns no result, then rerun `make flash`.
- **Verification:** Flash reported `Programming Finished`, `Verified OK`, and `Resetting Target`; onboard LD2 blinked.
- **Prevention:** Stop `make debug-server` before starting a separate `make flash` session. One ST-LINK probe generally cannot be opened concurrently by two OpenOCD servers.

## ERR-004 — GDB `The program is not being run` / `The program has no registers now`

- **Category:** Debug workflow / remote target attachment
- **Observed sequence:** `make debug` then `break main`, `continue`, and `info registers` without a remote connection.
- **Errors:** `The program is not being run.` and `The program has no registers now.`
- **Cause:** `make debug` launches GDB and loads the ELF symbols but does not attach to the OpenOCD target.
- **Resolution:** Leave `make debug-server` running in Terminal A. In Terminal B launch `make debug`, then enter `target extended-remote localhost:3333`, `monitor reset halt`, `break main`, and `continue`.
- **Verification:** GDB was observed to load symbols and resolve the breakpoint at `Core/main.c`; successful remote attaching/register inspection had **not yet been shown** in the conversation.
- **Prevention:** Document the required two-terminal workflow or automate GDB initialization in a separate `.gdbinit` file.

## ERR-005 — `Error: timeout waiting for ITM_TCR_BUSY_BIT`

- **Category:** OpenOCD trace initialization
- **Observed during:** A successful flash session while OpenOCD was examining the STM32.
- **Message:** `Error: timeout waiting for ITM_TCR_BUSY_BIT`
- **Cause:** Not confirmed; relates to the ITM/trace state or OpenOCD interaction with the debug/trace unit. This message alone does **not** establish a failed flash.
- **Outcome:** OpenOCD continued to program, verify, and reset the MCU successfully; LD2 blinked.
- **Follow-up:** If ITM/SWO trace output is required, reproduce under a clean reset, capture verbose OpenOCD logs, and verify trace configuration. Do not suppress without understanding if trace debugging is being used.

## WARN-001 — OpenOCD SWD frequency substitution

- **Category:** Debug adapter clock configuration
- **Message:** `Unable to match requested speed 2000 kHz, using 1800 kHz` (and similarly 8000→4000 kHz).
- **Explanation:** ST-LINK selected a supported clock rate; it did not block connection or flashing.
- **Optional mitigation:** Specify an adapter-supported clock using `-c 'adapter speed 1800'` after interface configuration.

## New issue template

```markdown
## ERR-XXX — Concise symptom
- **Date / environment:**
- **Subsystem:**
- **Reproduction steps:**
- **Expected behavior:**
- **Actual output (verbatim):**
- **Root cause (confirmed / hypothesis):**
- **Fix / workaround:**
- **Evidence after fix:**
- **Regression prevention / follow-up:**
```

For issues that may impact other stages, link this entry from the root `docs/error-index.md` (planned) or a GitHub Issue. Keep hardware-specific evidence distinct from commands that are only expected to work.
