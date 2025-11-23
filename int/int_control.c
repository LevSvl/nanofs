#include "types.h"
#include "console.h"
#include "m328Pdef.h"
#include "bits.h"

void traps_enable()
{
    SREG |= BIT(SREG_I);
}

void traps_disable()
{
    SREG &= ~BIT(SREG_I);
}

void system_int_handler()
{
    ;
}
