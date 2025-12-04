#include "types.h"
#include "console.h"
#include "m328Pdef.h"
#include "math.h"
#include "time.h"
#include "bits.h"

#define TIMER_DEFAULT_DIV (1024)

enum clocksource_div {
    NO_CLOCK,
    SYSTEM_CLOCK,
    SYSTEM_CLOCK_DIV_8,
    SYSTEM_CLOCK_DIV_64,
    SYSTEM_CLOCK_DIV_256,
    SYSTEM_CLOCK_DIV_1024
};

enum timer_mode {
    NORMAL              = 0,
    CLEAR_ON_CMP_MATCH  = 2
};

enum timer_resolution {
    TIMER_BOTTOM  = 0,
    TIMER_MAX     = 255
};

enum timer_int_types {
    TIMER_OVERFLOW_INT  = (1 << TOIE0),
    CMP_A_MATCH_INT     = (1 << OCIE0A),
    CMP_B_MATCH_INT     = (1 << OCIE0B),
};

struct time_info time_info;

void timer_int_mask(uint8_t int_mask)
{
    TIMSK0 |= int_mask;
}

void timer_int_unmask(uint8_t int_mask)
{
    TIMSK0 &= ~int_mask;
}

uint8_t cmp_read()
{
    return OCR0A;
}

void cmp_write(uint8_t cmp_val)
{
    OCR0A = cmp_val;
}

void timer_setup_clk_div(uint16_t div)
{
    enum clocksource_div div_mode;
    
    switch (div)
    {
    case 1:
        div_mode = SYSTEM_CLOCK;
        break;
    case 8:
        div_mode = SYSTEM_CLOCK_DIV_8;
        break;
    case 64:
        div_mode = SYSTEM_CLOCK_DIV_64;
        break;
    case 256:
        div_mode = SYSTEM_CLOCK_DIV_256;
        break;
    case 1024:
        div_mode = SYSTEM_CLOCK_DIV_1024;
        break;
    default:
        printf("Error, invalid divisor(%d)\n", div);
        return;
    }

    time_info.clk_div = div;
    time_info.freq = udiv4(SYS_FREQ , div);
    TCCR0B |= div_mode;
}

void timer_init()
{
    /* Setup timer mode */
    TCCR0B &= ~BIT(WGM02);
    TCCR0A &= ~(BIT(WGM01) | BIT(WGM00));
    TCCR0A |= CLEAR_ON_CMP_MATCH;

    /* Set value to compare */
    cmp_write(TIMER_MAX);

    /* Mask timer interrupts */
    timer_int_mask(CMP_A_MATCH_INT);

    time_info.ticks = 0;
    /* Setup default clocksource divisor */
    timer_setup_clk_div(TIMER_DEFAULT_DIV);

    printf( "### [%l] Init timer done.\n"
            "### System frequency = %l\n"
            "### Timer frequency  = %l\n", 
            uptime(), SYS_FREQ, time_info.freq);
}

void timer_int_handler()
{
    time_info.ticks += TIMER_MAX;
}
