# STM32F446RE native Linux cross-compilation and programming workflow.
# GNU Make drives ARM GCC, objcopy, and OpenOCD over the onboard ST-LINK.

.DEFAULT_GOAL := all

TARGET := stm32_blinky
BUILD := build

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
GDB := gdb-multiarch
OPENOCD := openocd

CPUFLAGS := -mcpu=cortex-m4 -mthumb
CFLAGS := $(CPUFLAGS) -std=gnu11 -O0 -g3 -Wall -Wextra \
          -ffreestanding -ffunction-sections -fdata-sections
LDFLAGS := $(CPUFLAGS) -T STM32F446RE.ld -nostdlib \
           -Wl,--gc-sections -Wl,-Map=$(BUILD)/$(TARGET).map

SOURCES := Core/main.c Core/startup.c
ELF := $(BUILD)/$(TARGET).elf
BIN := $(BUILD)/$(TARGET).bin
HEX := $(BUILD)/$(TARGET).hex

.PHONY: all flash debug-server debug clean

# Produce ELF, raw binary, and Intel HEX images.
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
