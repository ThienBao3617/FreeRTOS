# STM32F407VGT6U Mini Round-Robin Scheduler Demo

**Board:** STM32F407VGT6U (e.g., STM32F4-Discovery or a custom board)  
**Development Environment:**  
- Ubuntu 20.04 (Jammy)  
- VSCode (using STM32 VSCode Extension)  
- CMake + Ninja (or GNU Make)  
- GCC ARM Toolchain (`arm-none-eabi-gcc`, `arm-none-eabi-gdb`)  
- OpenOCD + Cortex-Debug for on-target debugging 

## Table of Contents

1. [Project Overview](#project-overview)  
2. [Features](#Features)
3. [Prerequisites](#prerequisites)  


---

## Project Overview

This project demonstrates a **bare-metal mini-RTOS** (preemptive, round-robin scheduler) on an **ARM Cortex-M4** (STM32F407VGT6U). It now supports:

- **Five “tasks”** total: four user LED-blinker tasks plus a built-in idle task.  
- **Task delays**: each task can call `task_delay(ms)` to block itself for a specified tick count.  
- An **Idle Task** that executes when all user tasks are blocked/sleeping.  
- **PendSV-based context switching**: full context save/restore (R4–R11) in PendSV handler.  
- **SysTick interrupt at 1 kHz** to drive a global tick counter, unblock sleeping tasks, and pend a context switch.  
- **Manual exception configuration** to catch faults (HardFault, MemManage, BusFault).  
- Static stacks of 5 KiB per task (MSP is reserved for scheduler/exception mode; PSP is used by each task).  

Each of the four user tasks blinks one LED (PD12–PD15) at a different rate and prints “Task n” over `printf`. When a task calls `task_delay(x)`, it becomes blocked until the global tick count reaches its unblock time. The scheduler then finds the next runnable task (skipping blocked tasks). When no user task is runnable, the **idle task** spins in `idle_task()`.  

## Features

1. **Idle Task Added**  
   - A dedicated idle task (`idle_task()`) is now created and scheduled as task index 0.  
   - If all user tasks (1–4) are blocked, the scheduler selects the idle task automatically.

2. **`task_delay()` Implementation**  
   - Each user task may call `task_delay(uint32_t tick_count)` to block itself for `tick_count` milliseconds.  
   - Internally, `task_delay()` computes `block_count = (current global tick) + tick_count`, sets the task’s state to “blocked,” and pend a PendSV.  
   - The SysTick_Handler increments a global tick (`g_tick_count`) every 1 ms and calls `unblock_tasks()`, which wakes any task whose `block_count` has been reached.

3. **PendSV-Based Context Switch**  
   - Instead of forcing a context switch every tick unconditionally, now each task can voluntarily block itself with `task_delay()`.  
   - PendSV_Handler saves R4–R11 onto the current PSP, calls `save_psp_value()`, then `update_next_task()`, restores next task’s R4–R11, and updates PSP.  

4. **Scheduler Stack Separation**  
   - MSP is initialized to point at a dedicated “scheduler stack” at reset.  
   - On `main()`, we call `init_scheduler_task(scheduler_stack_top)`, so MSP always remains in scheduler/exception context.  
   - PSP is switched to the first user task’s stack before the first `taskN_handler()` is invoked.

5. **Fault Handlers Enabled**  
   - Memory Management, Bus, and Usage Faults are enabled in `enable_processor_faults()`.  
   - If a stack underflow/overflow or illegal memory access occurs, the corresponding fault handler prints a message and halts.


---

## Prerequisites


---



Ensure the following software and hardware are available:

### 💻 Software Requirements

- **Linux Host** (e.g., Ubuntu 22.04 or similar)
- **Visual Studio Code (VSCode)**

### 🔌 VSCode Extensions

Install the following extensions from the VSCode Extensions Marketplace:

- **C/C++** (Microsoft)  
  → IntelliSense, code navigation, and debugging integration  
- **CMake Tools**  
  → Build support within VSCode and CMake preset management  
- **Cortex-Debug**  
  → Flashing and debugging via OpenOCD  
- **STM32 for VSCode** (prerequisites by STM32CubeCLT v1.15.0 or later and STM32CubeMX v6.11.0 or later)  
  → Helpful for generating board support and HAL code

### 🛠️ ARM GCC Toolchain and OpenOCD (for SWD/JTAG flashing & debugging)


```bash
sudo apt update
sudo apt install gcc-arm-none-eabi gdb-arm-none-eabi cmake build-essential ninja-build
```

```bash
sudo apt install openocd
```
