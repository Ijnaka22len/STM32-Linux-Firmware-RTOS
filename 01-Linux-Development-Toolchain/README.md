# Stage 01 — Linux-Based STM32 Development & Debugging

> **Status:** Verified on hardware for the bare-metal PA5/LD2 example. FreeRTOS integration is a separate future stage.

This stage documents an end-to-end, reproducible STM32 firmware development workflow hosted on **Debian GNU/Linux 13**. The target is an **ST NUCLEO-F446RE** with its onboard **ST-LINK/V2-1**. Linux hosts the cross-compiler and debug tools; **Linux does not run on the STM32F446RE**.

## 1. Learning objectives and verification

The workflow consists of authoring C firmware, cross-compiling with Arm GNU GCC, linking to the STM32 memory map, generating an ELF/BIN/HEX image, programming via OpenOCD over USB/SWD, and debugging via GDB.

**Observed on the actual setup:**

| Check | Evidence |
|---|---|
| GNU Arm cross-compiler installed | `arm-none-eabi-gcc` 14.2.1 |
| Binary utilities installed | `arm-none-eabi-objcopy` 2.44 |
| OpenOCD installed | 0.12.0+dev snapshot |
| Probe enumerated on USB | `0483:374B` ST-LINK/V2.1 |
| MCU recognized | Cortex-M4, 512 KiB flash reported by OpenOCD |
| Flash and verify | `Programming Finished`, `Verified OK` |
| Firmware running | Onboard green LD2 (PA5) blinked continuously |

Versions reflect this experiment, not strict minimum requirements.

## 2. Hardware and connections

- Board: ST NUCLEO-F446RE, STM32F446RE Arm Cortex-M4.
- Host: Debian GNU/Linux 13 (the setup was accessed over an SSH terminal).
- Cable: data-capable USB cable to **the onboard ST-LINK USB connector**.
- Debug link: ST-LINK/V2-1 → SWD → STM32 target, already routed on the NUCLEO board.
- Test output: onboard LD2 on **PA5**; no external breadboard wiring required.

The native Linux host can run on an x86 or ARM machine as long as the appropriate packages are available; the compiler produces Cortex-M4 instructions, not Linux executables.

## 3. Install and verify packages

```bash
sudo apt update
sudo apt install -y \
    build-essential make gcc-arm-none-eabi binutils-arm-none-eabi \
    openocd gdb-multiarch git tree usbutils

arm-none-eabi-gcc --version
arm-none-eabi-objcopy --version
openocd --version
```

For this bare-metal `-nostdlib` sample, no HAL, FreeRTOS, or STM32CubeIDE installation is required. For programs using the C standard library, interrupts, or FreeRTOS, additional startup/linker/runtime integration is necessary.

## 4. Connect and verify ST-LINK

Connect the board via USB, then run:

```bash
lsusb
lsusb -d 0483:
```

The observed device was `0483:374B STMicroelectronics ST-LINK/V2.1`.

Probe using OpenOCD:

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```

Expected success indicators: `Cortex-M4 ... detected`, `Examination succeeded`, and `Listening on port 3333 for gdb connections`. **Exit with Ctrl+C** after the check; otherwise the resident server holds the probe and another OpenOCD process cannot open it. You can check for stale sessions with `pgrep -a openocd`.

If permission errors occur, troubleshoot udev rules and USB device access before resorting to root. See [error.md](error.md).

## 5. Source files and memory map

The minimal test project is located in [`stm32-linux/`](stm32-linux/):

```text
stm32-linux/
├── Core/
│   ├── main.c                # RCC/GPIOA register-level LD2 toggle
│   └── startup.c             # Reset vector and C data/BSS initialization
├── STM32F446RE.ld           # 512 KiB flash, 128 KiB RAM layout
└── Makefile                 # Build, flash, and debug targets
```

- `main.c` configures GPIOA pin 5 as a push-pull output and toggles the `BSRR` register in a busy-wait loop. **The loop is not a calibrated timebase**.
- `startup.c` sets up the initial stack and reset path, initializes `.data`/`.bss`, and calls `main()`. The interrupt vector table is intentionally minimal and must be replaced with the official device vector table for interrupt-driven/RTOS work.
- `STM32F446RE.ld` places the interrupt vectors and executable code in flash at `0x08000000`, and writable data in RAM at `0x20000000`.
- `Makefile` invokes `arm-none-eabi-gcc`, `objcopy`, `size`, and OpenOCD. A build directory is generated locally and excluded from version control.

The repository example is deliberately small and not a general-purpose CMSIS/HAL BSP. The source files in this Stage 01 package are a documented reconstruction of the validated blinking-LED example, not a byte-for-byte copy of the user's original Debian working directory; test this published copy with `make` and `make flash` on the board before marking it independently validated.

## 6. Compile the firmware

```bash
cd 01-Linux-Development-Toolchain/stm32-linux
make clean
make
ls -lh build/
```

The following generated files are expected:

| File | Purpose |
|---|---|
| `stm32_blinky.elf` | Executable with symbols for GDB and flash programming |
| `stm32_blinky.bin` | Raw flash image |
| `stm32_blinky.hex` | Intel HEX flash image |
| `stm32_blinky.map` | Linker memory map |

The observed baseline compiled to **864 bytes of text** with no `.data` or `.bss` in that particular build. Code size will vary if the compiler flags or source change.

### Typical commands

```bash
make                 # Build
make clean           # Remove generated files
make flash           # Build and program flash
make debug-server    # Launch OpenOCD; leave running
make debug           # Launch GDB with ELF symbols
```

## 7. Flash and verify on hardware

Ensure no other process is using ST-LINK:

```bash
pgrep -a openocd
# If an obsolete server exists, stop it in its terminal or use `kill <PID>`.
make flash
```

The `flash` target invokes the equivalent of:

```bash
openocd -f interface/stlink.cfg \
    -c 'transport select swd' \
    -f target/stm32f4x.cfg \
    -c 'program build/stm32_blinky.elf verify reset exit'
```

On this setup, OpenOCD logged `Programming Finished`, `Verified OK`, and `Resetting Target`. The **green LD2 on PA5 blinked** after reset.

An `ITM_TCR_BUSY_BIT` timeout appeared once during probe setup, but the later programming and verification succeeded; do not mistake it for a clean debug trace. See [error.md](error.md).

## 8. Debug with GDB (two terminals)

**Terminal A — OpenOCD:**

```bash
cd 01-Linux-Development-Toolchain/stm32-linux
make debug-server
```

**Terminal B — GDB:**

```bash
cd 01-Linux-Development-Toolchain/stm32-linux
make debug
```

At the `(gdb)` prompt:

```gdb
target extended-remote localhost:3333
monitor reset halt
break main
continue
info registers
x/8i $pc
x/1wx 0x40020000
x/1wx 0x40020014
```

`0x40020000` is `GPIOA_MODER` and `0x40020014` is `GPIOA_ODR`. GPIOA pin 5's output state is represented by bit 5 in the ODR register. Breakpoints stop MCU execution and are **not appropriate for unbiased real-time jitter measurement**.

`make debug` in the supplied Makefile loads the ELF into GDB; **it does not automatically attach to OpenOCD**. The first two GDB commands above are necessary.

For a remote SSH host, `localhost:3333` refers to the Debian machine where GDB is running. No external network exposure of the GDB port is needed.

## 9. Validation record

| Experiment | Actual result | Notes |
|---|---|---|
| Linux cross-compile | Passed | ELF, BIN, HEX, MAP generated |
| ST-LINK detection | Passed | USB VID:PID `0483:374B` |
| OpenOCD target examination | Passed | Cortex-M4 detected |
| Flash and verify | Passed | `Verified OK` |
| LD2 LED blinking | Passed | Confirmed visually |
| GDB launch | Passed | Symbols read, breakpoint at `main` set |
| GDB remote attach and stepping | Not yet confirmed | Earlier attempt omitted `target extended-remote` |
| FreeRTOS on this Linux-built firmware | Not yet implemented | Covered in Stage 03 |

## 10. Troubleshooting and error tracking

Read the accompanying [error.md](error.md) for observed error messages, root causes, fixes, and validation status. Add new entries whenever a build, flash, or debug issue is encountered; preserve the exact error text, environment, fix, and verification.

## 11. Next improvements

- Replace minimal startup with the official STM32F446 device startup/vector table.
- Add CMSIS device headers rather than manual register address definitions.
- Configure automated CI compilation (no hardware required on CI runners).
- Separate shared platform startup and drivers from individual examples.
- Add a reproducibility checklist for ARM and x86 Debian hosts.

**References:** [OpenOCD project](https://openocd.org/), [GNU Arm Embedded Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain), [FreeRTOS](https://www.freertos.org/), [ST NUCLEO-F446RE](https://www.st.com/en/evaluation-tools/nucleo-f446re.html).
