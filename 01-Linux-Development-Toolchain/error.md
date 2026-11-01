# Stage 01 — Build, Flash, and Debugging Error Log

## Purpose and reporting method

I maintain this log to document issues encountered while developing STM32F446RE firmware on Debian GNU/Linux 13. Entries distinguish **observed symptoms**, **confirmed causes**, **unverified hypotheses**, **corrective actions**, and **validation evidence**. The log is a record of actual sessions, not a catalog of hypothetical failures.

## ERR-001 — GNU Make reported no targets

**Subsystem:** Build automation  
**Command:** `make`  
**Observed output:**

```text
make: *** No targets. Stop.
```

**Diagnosis:** GNU Make found the project Makefile but did not find a usable target. At the time, the file had been created but had not been populated with valid build rules.

**Correction:** I defined the `all`, `flash`, `debug-server`, `debug`, and `clean` targets and ensured that recipe lines began with TAB characters.

**Validation:** A subsequent build generated the `.elf`, `.bin`, `.hex`, and `.map` files successfully. The repository build later reported 844 bytes of text.

## ERR-002 — OpenOCD failed to open ST-LINK

**Subsystem:** USB debug probe access  
**Command:** `openocd -f interface/stlink.cfg -f target/stm32f4x.cfg`  
**Observed output:**

```text
Error: open failed
```

**Diagnosis:** The exact cause of the initial failure was not established. Device access conflicts and USB permissions were considered, but neither was proven responsible for this first occurrence.

**Investigation:** I confirmed that the debugger was enumerated by Linux as `0483:374B ST-LINK/V2.1`. A later OpenOCD invocation connected to the Cortex-M4 and started the GDB server on port 3333.

**Outcome:** SWD target examination subsequently succeeded. The original failure is classified as **resolved in operation, root cause unconfirmed**.

## ERR-003 — OpenOCD initialization failed during flashing

**Subsystem:** Flash programming / competing debug sessions  
**Command:** `make flash`  
**Observed output:**

```text
** OpenOCD init failed **
shutdown command invoked
make: *** [Makefile:58: flash] Error 1
```

**Diagnosis:** A separate OpenOCD server was still running and occupying the ST-LINK interface. I identified it with:

```bash
pgrep -a openocd
```

```text
424052 openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```

**Correction:** I stopped the obsolete process and confirmed that no OpenOCD process remained:

```bash
kill 424052
pgrep -a openocd
```

**Validation:** `make flash` subsequently reported `Programming Finished`, `Verified OK`, and `Resetting Target`. The onboard LD2 blinked continuously in the initial hardware test.

## ERR-004 — GDB had no running target or registers

**Subsystem:** Remote debugging  
**Observed sequence:** `make debug`, `break main`, `continue`, `info registers`  
**Observed output:**

```text
The program is not being run.
The program has no registers now.
```

**Diagnosis:** GDB had loaded the local ELF and symbol table but had not connected to the OpenOCD GDB server. The breakpoint could be resolved from debug symbols without a live target connection.

**Correct debugging sequence:**

```bash
# Terminal A
make debug-server

# Terminal B
make debug
```

```gdb
target extended-remote localhost:3333
monitor reset halt
break main
continue
```

**Validation:** ELF loading and breakpoint resolution were observed. A successful remote attach and register read have **not yet been independently documented**; this entry remains **procedure corrected, hardware-debug verification pending**.

## ERR-005 — ITM trace unit timeout during OpenOCD examination

**Subsystem:** ARM debug/trace initialization  
**Observed output:**

```text
Error: timeout waiting for ITM_TCR_BUSY_BIT
```

**Diagnosis:** The exact cause has not been confirmed. The diagnostic refers to an ITM trace-control busy condition during debugger interaction and is not, by itself, evidence of failed flash programming.

**Outcome:** In the November 1, 2026 repository test, OpenOCD continued to identify the target, program flash, verify the image, and reset the processor successfully.

**Status:** Nonfatal for the tested flashing workflow; trace-related cause unresolved.

## WARN-001 — ST-LINK selected a different SWD clock rate

**Subsystem:** Debug adapter speed negotiation  
**Observed output:**

```text
Unable to match requested speed 2000 kHz, using 1800 kHz
Unable to match requested speed 8000 kHz, using 4000 kHz
```

**Interpretation:** The adapter selected supported SWD clock frequencies. This warning did not prevent SWD connection, firmware programming, or verification.

**Status:** Informational warning; no corrective action was required for the validated workflow.

---

## Error record template

~~~markdown
## ERR-XXX — Short technical description

**Date:** YYYY-MM-DD  
**Subsystem:**  
**Command or reproduction:**  
**Observed output:**

```text
Exact error output
```

**Diagnosis:** Confirmed cause or explicitly identified hypothesis.

**Correction:** Commands or source changes applied.

**Validation:** Actual evidence after the correction.

**Status:** Resolved / unresolved / verification pending.
~~~
