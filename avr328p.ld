OUTPUT_ARCH("avr5")
ENTRY("_start")

MEMORY
{
  FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 30K
  SRAM  (xrw)     : ORIGIN = 0x00000100, LENGTH = 2K
}

SECTIONS {
  .text ORIGIN(FLASH) :
  {
    *(.text.vectors)    /* .text.vectors section */
    *(.text.init)       /* .text.init section */
    *(.text.traps)       /* .text.init section */
    *(.text)            /* .text sections */
    *(.text*)           /* other .text* sections */
    . = ALIGN(4);
    text_end = .;
  } >FLASH

  .data :
  {
    data_start = .;    /* create a global symbol at ram start for garbage collector */
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
    *(.srodata)        /* .rodata sections (constants, strings, etc.) */
    *(.srodata*)       /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
    *(.data)           /* .data sections */
    . = ALIGN(4);
    *(.data*)          /* .data* sections */
    data_end = .;        /* define a global symbol at data end; used by startup code in order to initialise the .data section in RAM */
  } >SRAM AT>FLASH

  .bss :
  {
    . = ALIGN(4);
    bss_start  = .;         /* define a global symbol at bss start; used by startup code */
    *(.bss)
    *(.bss*)
    *(.sbss)
    *(.sbss*)
    *(COMMON)
    bss_end = .;         /* define a global symbol at bss end; used by startup code */
  } >SRAM

  .nanofs : 
  {
    . = ALIGN(0x10);
    PROVIDE(nanofs_start = .);
    *(.nanofs)
    PROVIDE(nanofs_end = .);
  } > FLASH
}
