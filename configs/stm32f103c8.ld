/*
 * Linker script for STM32F103C8T6
 * Flash: 64KB, SRAM: 20KB
 *
 * NOTE: This is a minimal placeholder. For production use,
 * generate a proper linker script via STM32CubeMX or use
 * the one provided by your toolchain/HAL package.
 */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Entry point */
ENTRY(Reset_Handler)

/* Stack size */
_Min_Stack_Size = 0x400;

SECTIONS
{
    /* Vector table and code → Flash */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Initialized data → SRAM (loaded from Flash) */
    .data : AT(_etext)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM

    /* Uninitialized data → SRAM */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* Stack */
    ._stack :
    {
        . = ALIGN(8);
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
        _estack = .;
    } > SRAM
}
