.include "m328Pdef.inc"

.section .text

.global flash_load_word
.global flash_store


flash_load_word:
    # R25:R24:          @dst
    # R23:R22:          @src
    MOV         XL, R24
    MOV         XH, R25

    MOV         ZL, R22
    MOV         ZH, R23

    LPM         r18, Z+
    LPM         r19, Z

    ST          X+, r18
    ST          X, r19

    RET


flash_store:
    # R25:R24:          @dst
    # R23:R22:          @src
    # R21:R20, R19:R18: @size 
