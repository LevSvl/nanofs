
.include "m328Pdef.inc"

.section .text.vectors
.global _start

# ***** VECTORS TABLE **************************************************

.macro RESET_VECTOR
    JMP         _start 
.endm

.macro DEFAULT_INT_HANDLER
    JMP         __vector_default
.endm

.macro TIMER0A_INT_HANDLER
    JMP         __timer1A_int_handler
.endm

#***********************************************************************
#***********************************************************************


vector_base:
    RESET_VECTOR                /* 1: System reset                      */
    DEFAULT_INT_HANDLER         /* 2: External Interrupt Request 0      */
    DEFAULT_INT_HANDLER         /* 3: External Interrupt Request 1      */
    DEFAULT_INT_HANDLER         /* 4: Pin Change Interrupt Request 0    */
    DEFAULT_INT_HANDLER         /* 5: Pin Change Interrupt Request 0    */
    DEFAULT_INT_HANDLER         /* 6: Pin Change Interrupt Request 1    */
    DEFAULT_INT_HANDLER         /* 7: Watchdog Time-out Interrupt       */
    DEFAULT_INT_HANDLER         /* 8: Timer/Counter2 Compare Match A    */
    DEFAULT_INT_HANDLER         /* 9: Timer/Counter2 Compare Match A    */
    DEFAULT_INT_HANDLER         /* 10: Timer/Counter2 Overflow          */
    DEFAULT_INT_HANDLER         /* 11: Timer/Counter1 Capture Event     */
    DEFAULT_INT_HANDLER         /* 12: Timer/Counter1 Compare Match A   */
    DEFAULT_INT_HANDLER         /* 13: Timer/Counter1 Compare Match B   */
    DEFAULT_INT_HANDLER         /* 14: Timer/Counter1 Overflow          */
    TIMER0A_INT_HANDLER         /* 15: TimerCounter0 Compare Match A    */
    DEFAULT_INT_HANDLER         /* 16: TimerCounter0 Compare Match B    */
    DEFAULT_INT_HANDLER         /* 17: Timer/Couner0 Overflow           */
    DEFAULT_INT_HANDLER         /* 18: SPI Serial Transfer Complete     */
    DEFAULT_INT_HANDLER         /* 19: USART Rx Complete                */
    DEFAULT_INT_HANDLER         /* 20: USART, Data Register Empty       */
    DEFAULT_INT_HANDLER         /* 21: USART Tx Complete                */
    DEFAULT_INT_HANDLER         /* 22: ADC Conversion Complete          */
    DEFAULT_INT_HANDLER         /* 23: EEPROM Ready                     */
    DEFAULT_INT_HANDLER         /* 24: Analog Comparator                */
    DEFAULT_INT_HANDLER         /* 25: Two-wire Serial Interface        */
    DEFAULT_INT_HANDLER         /* 26: Store Program Memory Read        */


.section .text.init
.global main

_start:
    # init data
    LDI         XL, lo8(data_start)
    LDI         XH, hi8(data_start)

    LDI         YL, lo8(data_end)
    LDI         YH, hi8(data_end)

    LDI         ZL, lo8(text_end)
    LDI         ZH, hi8(text_end)
1:
    CP          XH, YH
    BRSH        1f
2:
    LPM         r16, Z+
    ST          X+, r16
    JMP         1b
1:
    CP          XL, YL
    BRSH        2f
    JMP         2b
2:
    # init bss
    LDI         XL, lo8(bss_start)
    LDI         XH, hi8(bss_start)

    LDI         YL, lo8(bss_end)
    LDI         YH, hi8(bss_end)
1:
    CP          XH, YH
    BRSH        1f
2:
    LDI         r16, 0
    ST          X+, r16
    JMP         1b
1:
    CP          XL, YL
    BRSH        2f
    JMP         2b
2:
    # setup stack pointer
    LDI         r16, lo8(RAMEND)
    OUT         SPL, r16
    LDI         r16, hi8(RAMEND)
    OUT         SPH, r16

    # all done, go main
    JMP         main
