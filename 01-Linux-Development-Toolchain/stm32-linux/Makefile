.DEFAULT_GOAL := all
TARGET := stm32_blinky
BUILD := build
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

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

flash: all
	openocd -f interface/stlink.cfg -c 'transport select swd' \
	        -f target/stm32f4x.cfg \
	        -c 'program $(ELF) verify reset exit'

debug-server:
	openocd -f interface/stlink.cfg -c 'transport select swd' \
	        -f target/stm32f4x.cfg

debug: $(ELF)
	gdb-multiarch $(ELF)

clean:
	rm -rf $(BUILD)
