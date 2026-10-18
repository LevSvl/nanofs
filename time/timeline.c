#include "types.h"
#include "console.h"
#include "m328Pdef.h"
#include "math.h"
#include "time.h"
#include "bits.h"

extern struct time_info time_info;

#define CK2SEC(freq) (freq)
#define CK2MS(freq) (udiv4(freq, 1000))

#define JIFFIES(ck) (udiv4(ck, 255))

void delay_ms(uint32_t ms)
{
    uint32_t start = time_info.jiffies;

    while ((time_info.jiffies - start) < JIFFIES(CK2MS(time_info.freq) * ms))
        ;
}

void delay_sec(uint32_t sec)
{
    uint32_t start = time_info.jiffies;

    while ((time_info.jiffies - start) < (JIFFIES(time_info.freq) * sec))
        ;
}

uint32_t uptime()
{
    return udiv4(time_info.ticks, udiv4(time_info.freq, 1000));
}
