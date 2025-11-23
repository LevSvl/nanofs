.include "m328Pdef.inc"

.macro INT_SAVE_CONTEXT
    PUSH        r0
    PUSH        r1
	PUSH        r2
	PUSH        r3
	PUSH        r4
    PUSH        r5
	PUSH        r6
    PUSH        r7
	PUSH        r8
    PUSH        r9
	PUSH        r10
    PUSH        r11
	PUSH        r12
    PUSH        r13
	PUSH        r14
    PUSH        r15
	PUSH        r16
    PUSH        r17
    PUSH        r18
	PUSH        r19
    PUSH        r20
	PUSH        r21
    PUSH        r22
	PUSH        r23
    PUSH        r24
	PUSH        r25
    PUSH        r26
    PUSH        r27
	PUSH        r28
    PUSH        r29
	PUSH        r30
    PUSH        r31

    IN          r16, SREG
    PUSH        r16
.endm

.macro INT_RESTORE_CONTEXT
    POP         r16
    OUT         SREG, r16

    POP         r31
	POP         r30
	POP         r29
	POP         r28
    POP         r27
	POP         r26
    POP         r25
	POP         r24
    POP         r23
	POP         r22
    POP         r21
	POP         r20
    POP         r19
	POP         r18
    POP         r17
	POP         r16
    POP         r15
    POP         r14
	POP         r13
    POP         r12
	POP         r11
    POP         r10
	POP         r9
    POP         r8
	POP         r7
    POP         r6
    POP         r5
	POP         r4
    POP         r3
	POP         r2
    POP         r1
    POP         r0
.endm

.macro INT_DEFINE_HANDLER vector_name, handler_func
.global \vector_name
.global \handler_func
\vector_name:
    /* Save context */
    INT_SAVE_CONTEXT
    /* Call the exception handler function */
    CALL         \handler_func
    /* Restore the caller saving registers (context) */
    INT_RESTORE_CONTEXT
    /* Return to regular code */
    RETI
.endm


.section .text.traps

INT_DEFINE_HANDLER __vector_default system_int_handler
INT_DEFINE_HANDLER __timer1A_int_handler timer_int_handler
