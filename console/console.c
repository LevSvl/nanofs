#include "types.h"
#include "console.h"
#include "time.h"

void console_init()
{
    usart_init();

    printf("Console initialized\n");
}

void console_read(char *dst, int size)
{
    int i = 0;
    while (size--)
        dst[i++] = usart_getc();
}
