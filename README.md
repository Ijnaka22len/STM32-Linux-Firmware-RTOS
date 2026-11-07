# Linux-Based STM32 Firmware and RTOS Development

**Repository:** `STM32-Linux-Firmware-RTOS`

 Developing, compiling, flashing, and debugging STM32 firmware entirely from **Debian GNU/Linux 13**. The work spans bare-metal firmware, register-level peripheral drivers, and real-time systems engineering with FreeRTOS.

## Hardware and toolchain

| Component | Platform |
| --- | --- |
| Target board | ST NUCLEO-F446RE (STM32F446RE, Arm Cortex-M4) |
| Development host | Debian GNU/Linux 13 |
| Language | C (with ARM assembly and Make as needed) |
| Cross-compiler | Arm GNU Toolchain (`arm-none-eabi-gcc`) |
| Build system | GNU Make |
| Flash/debug probe | Onboard ST-LINK/V2-1, SWD |
| Flash/debug tools | OpenOCD and GDB (`gdb-multiarch`) |
| RTOS track | FreeRTOS (planned Linux-native integration) |

## Project tracks

### 01 - Linux-Based STM32 Development & Debugging

Cross-compilation, startup code, linker scripts, Makefile-based builds, ELF/BIN/HEX generation, ST-LINK programming, and source-level debugging with OpenOCD and GDB.

**Validated so far:** A bare-metal PA5/LD2 LED-blinking application was compiled on Debian, flashed through OpenOCD, verified, and observed running on the NUCLEO-F446RE.

### 02 - Bare-Metal Firmware & Peripheral Driver Development

Register-level hardware programming and reusable drivers, with planned experiments in GPIO, UART, SPI, I²C, timers, ADC, and interrupts.

### 03 - FreeRTOS & Real-Time Systems Engineering

Priority-based scheduling, periodic tasks, preemption, synchronization, timing jitter, execution-time profiling, and scheduling performance analysis. Earlier experiments on STM32CubeIDE provide a starting point; integration into this Debian-native repository is a future milestone.

## Suggested repository layout

```text
STM32-Linux-Firmware-RTOS/
├── 01-Linux-Development-Toolchain/
│   └── stm32-linux/          # Move the existing working project here
│       ├── Core/
│       │   ├── main.c
│       │   └── startup.c
│       ├── STM32F446RE.ld
│       └── Makefile
├── 02-Bare-Metal-Firmware-Drivers/
├── 03-FreeRTOS-Real-Time-Systems/
├── docs/
├── .gitignore
├── LICENSE
└── README.md
```

Folders for future work can be added as implementations become available.

## Build and flash the validated bare-metal example

After copying the working `stm32-linux` project into `01-Linux-Development-Toolchain/`:

```bash
cd 01-Linux-Development-Toolchain/stm32-linux
make
make flash
```

The `make flash` target uses OpenOCD and the onboard ST-LINK to program, verify, and reset the MCU. Ensure no other OpenOCD instance is holding the ST-LINK connection.

To debug, start the OpenOCD server in one terminal:

```bash
make debug-server
```

Then, in another terminal:

```bash
make debug
```

Within GDB, connect using:

```gdb
target extended-remote localhost:3333
monitor reset halt
break main
continue
```

## Progress(currently working stages)
- [x] Configure the ARM cross-compilation toolchain on Debian GNU/Linux 13
- [x] Build a standalone bare-metal STM32 firmware image using Make
- [x] Flash and verify firmware through OpenOCD and ST-LINK
- [x] Confirm onboard LD2 (PA5) LED blinking
- [x] Publish and document the working Linux-native source project
- [ ] Develop reusable peripheral drivers
- [ ] Integrate FreeRTOS into the Linux-native build
- [ ] Measure task scheduling latency, jitter, preemption, and CPU utilization

## Notes

The Linux kernel runs on the **development host**, not on the STM32F446RE. The microcontroller runs bare-metal firmware or FreeRTOS. Timing measurements should distinguish RTOS tick resolution from high-resolution cycle counting, and account for the overhead of logging and instrumentation.

## License

See [LICENSE](LICENSE) for reuse terms.
