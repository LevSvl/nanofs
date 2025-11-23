#include "types.h"
#include "console.h"
#include "time.h"

void console_init()
{
    usart_init();

    printf("Console initialized\n");
}
