#include "types.h"
#include "console.h"
#include "time.h"
#include "string.h"
#include "m328Pdef.h"
#include "traps.h"

int uptime_main(int argc, char *argv[])
{
    printf("uptime ms %l\n", uptime());
    return 0;
}
