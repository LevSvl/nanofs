#include "types.h"
#include "console.h"
#include "m328Pdef.h"
#include "math.h"
#include "time.h"
#include "bits.h"

extern struct time_info time_info;

#define CK2SEC(freq) (freq)
#define CK2MS(freq) (udiv4(freq, 1000))


void delay_ms(uint32_t ms)
{
    uint32_t start = time_info.ticks;

    while (time_info.ticks - start < (CK2MS(time_info.freq) * ms))
        ;
}

void delay_sec(uint32_t sec)
{
    uint32_t start = time_info.ticks;

    while (time_info.ticks - start < (CK2SEC(time_info.freq) * sec))
        ;
}

uint32_t uptime()
{
    return udiv4(time_info.ticks, CK2MS(time_info.freq));
}
