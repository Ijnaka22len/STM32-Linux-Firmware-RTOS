
# STM32F446RE — Linux-Based Bare-Metal Firmware Development
#
# Host: Debian GNU/Linux 13
# Target: STM32F446RE (ARM Cortex-M4)
# Programmer/Debugger: ST-LINK/V2-1, SWD
#
# Build artifacts: ELF, BIN, HEX, and linker MAP.
# Flash operations use a verified ST-LINK SWD frequency of 1800 kHz.

.DEFAULT_GOAL := all

TARGET := stm32_blinky
BUILD  := build

# ARM GNU toolchain
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size
GDB     := gdb-multiarch
OPENOCD := openocd

# Cortex-M4 compilation
CPUFLAGS := -mcpu=cortex-m4 -mthumb

CFLAGS := $(CPUFLAGS) \
          -std=gnu11 \
          -O0 -g3 \
          -Wall -Wextra \
          -ffreestanding \
          -ffunction-sections \
          -fdata-sections

LDFLAGS := $(CPUFLAGS) \
           -T STM32F446RE.ld \
           -nostdlib \
           -Wl,--gc-sections \
           -Wl,-Map=$(BUILD)/$(TARGET).map

# Firmware source files
SOURCES := Core/main.c Core/startup.c

ELF := $(BUILD)/$(TARGET).elf
BIN := $(BUILD)/$(TARGET).bin
HEX := $(BUILD)/$(TARGET).hex

# ST-LINK configuration
SWD_SPEED ?= 1800

OPENOCD_CONFIG := -f interface/stlink.cfg \
                  -c "transport select swd" \
                  -c "adapter speed $(SWD_SPEED)" \
                  -f target/stm32f4x.cfg

.PHONY: all flash debug-server debug clean size

# Compile and generate firmware images.
all: $(ELF) $(BIN) $(HEX)

$(BUILD):
	mkdir -p $(BUILD)

$(ELF): $(SOURCES) STM32F446RE.ld | $(BUILD)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

# Program the ELF image, verify flash contents, and reset the MCU.
# A separately running OpenOCD server must be stopped before this target.
flash: all
	$(OPENOCD) -f interface/stlink.cfg -c 'transport select swd' \
	        -f target/stm32f4x.cfg \
	        -c 'program $(ELF) verify reset exit'

# OpenOCD serves GDB on localhost:3333 until the process is stopped.
debug-server:
	$(OPENOCD) -f interface/stlink.cfg -c 'transport select swd' \
	        -f target/stm32f4x.cfg

# Open ELF symbols in GDB; attach with target extended-remote localhost:3333.
debug: $(ELF)
	$(GDB) $(ELF)

clean:
	rm -rf $(BUILD)
