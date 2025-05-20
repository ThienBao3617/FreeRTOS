# Makefile for STM32F407 Mini Scheduler Demo

# Toolchain
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# CPU and FPU configuration
CFLAGS  = -Wall -Wextra -g -O0 -std=gnu11 \
          -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
          -ffunction-sections -fdata-sections

LDFLAGS = -T Core/LinkerScript/stm32f407xx_flash.ld \
          -Wl,--gc-sections \
          -Wl,-Map=build/firmware.map

INCLUDE_DIRS = -ICore/Inc

# Source files
C_SOURCES   = Core/Src/main.c \
              Core/Src/led.c \
              Core/Src/system_stm32f4xx.c
ASM_SOURCES = Core/Src/startup_stm32f407xx.s

# Convert source file names to object file names under build/
OBJS = $(patsubst %.c,build/%.o,$(C_SOURCES)) \
       $(patsubst %.s,build/%.o,$(ASM_SOURCES))

# Default target
all: build/directories build/firmware.elf firmware.bin

# Create build directories if needed
build/directories:
	mkdir -p build/Core/Src

# Compile C source files
build/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Compile assembly source files
build/%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# Link into firmware.elf
build/firmware.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@

# Generate binary from ELF
firmware.bin: build/firmware.elf
	$(OBJCOPY) -O binary build/firmware.elf build/firmware.bin

# Clean build artifacts
clean:
	rm -rf build
