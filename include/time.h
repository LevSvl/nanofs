#ifndef TIMER_H
#define TIMER_H

#include "types.h"

extern struct time_info time_info;

struct time_info {

    /* 
     * ticks must not be optimized because it means that
     * it can change very fast while be used for comparison
     * with other quantities in timeline functions
     */
    volatile uint32_t ticks;
    uint32_t freq;
    uint16_t clk_div;
};

static inline uint32_t now()
{
    return time_info.ticks;
}

void delay_ms(uint32_t ms);

void timer_init();

uint32_t uptime();

#endif // TIMER_H
