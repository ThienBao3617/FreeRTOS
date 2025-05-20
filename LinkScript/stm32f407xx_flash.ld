/* Linker script for STM32F407VGT6U */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Top of Stack (SRAM_END) */
_estack = 0x20020000;  /* 0x20000000 + 128K = 0x20020000 */

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))  /* Vector table */
    . = ALIGN(4);
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text*)             /* All code */
    *(.rodata*)           /* Read-only data */
    . = ALIGN(4);
  } > FLASH

  .data : AT(.text + SIZEOF(.text))
  {
    . = ALIGN(4);
    _sdata = .;           /* Start of .data in RAM */
    *(.data*)
    . = ALIGN(4);
    _edata = .;           /* End of .data in RAM */
  } > RAM AT > FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;            /* Start of .bss in RAM */
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;            /* End of .bss in RAM */
  } > RAM

  /* Other sections (heap, stack) can be added if needed */
}
