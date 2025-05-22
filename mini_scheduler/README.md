# STM32F407VGT6U Mini Round-Robin Scheduler Demo

**Board:** STM32F407VGT6U (e.g., STM32F4-Discovery or a custom board)  
**Development Environment:**  
- Ubuntu 20.04 (Jammy)  
- VSCode (no STM32CubeIDE or CubeMX)  
- GCC ARM Toolchain (`arm-none-eabi-gcc`)  
- OpenOCD + Cortex-Debug for debugging  

## Table of Contents

1. [Project Overview](#project-overview)  
2. [Prerequisites](#prerequisites)  


---

## Project Overview

This project demonstrates a **very simple preemptive scheduler** (“mini-RTOS”) on an **ARM Cortex-M4** microcontroller (STM32F407VGT6U). It switches among **four tasks** in a fixed round-robin fashion, each task blinking one LED (PD12–PD15) at a different rate (1 s, 500 ms, 250 ms, 125 ms) and printing a “Task n” message via `printf`.

- **No RTOS kernel** is used—everything is implemented bare-metal.  
- **SysTick timer** (1 kHz) generates interrupts every 1 ms to force a context switch.  
- Each task has its own 5 KiB stack (static allocation), and MSP is used for scheduler/exception mode.  
- All context save/restore is coded manually in `SysTick_Handler` to push/pop R4–R11; the hardware automatically handles R0–R3, R12, LR, PC, and xPSR.  
- **Fault handlers** (HardFault, MemManage, BusFault) are enabled to catch any stack or memory errors.  
- Because tasks do not share any peripheral, there are **no semaphores, mutexes, or queues** implemented—this is purely a demonstration of **context switching** on Cortex-M4.

---

## Prerequisites

- **Ubuntu 20.04 (Jammy) or similar**  
- **VSCode** already installed  
  - VSCode Extensions you should install:  
    1. **C/C++** (by Microsoft) → for IntelliSense and code navigation  
    2. **Makefile Tools** (optional) → to build from within VSCode  
    3. **Cortex-Debug** → for debugging with OpenOCD  
- **ARM GCC toolchain** (`arm-none-eabi-gcc`, `gdb-arm-none-eabi`)  
  ```bash
  sudo apt update
  sudo apt install gcc-arm-none-eabi gdb-arm-none-eabi make
