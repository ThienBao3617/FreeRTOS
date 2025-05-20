    .syntax unified
    .cpu cortex-m4
    .thumb

    .section .isr_vector, "a", %progbits
    .word   _estack                  /* Initial stack pointer (MSP) */
    .word   Reset_Handler            /* Reset Handler */
    .word   NMI_Handler              /* NMI Handler (default weak) */
    .word   HardFault_Handler        /* HardFault Handler */
    .word   MemManageFault_Handler   /* MemManage Handler */
    .word   BusFault_Handler         /* BusFault Handler */
    .word   UsageFault_Handler       /* UsageFault Handler */
    /* … other exception vectors … */
    .word   SysTick_Handler          /* SysTick Handler (vector[15]) */

    .section .text.Reset_Handler
    .weak   Reset_Handler
    .type   Reset_Handler, %function

Reset_Handler:
    /* 1) Copy .data from Flash to SRAM, 2) Zero .bss, 3) Optionally call SystemInit() */
    BL  SystemInit       /* SystemInit might set up clocks, if provided */
    BL  main             /* Jump to main() */
    BKPT #0              /* Breakpoint if main ever returns */

    ALIGN
    .end
