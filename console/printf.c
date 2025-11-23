#include <stdarg.h>

#include "math.h"
#include "types.h"

char digits[] = "0123456789abcdef";

void usart_transmit(uint8_t data);

static int convert(unsigned long x, int base, char buf[16])
{
    int i = 0;

    do{
        buf[i++] = digits[umod4(x, base)];
    } while ( (x = udiv4(x, base)) > 0);

    return i;
}

static void printint(int n, int base, int sign)
{
    char buf[16];
    int i;
    unsigned int x;

    if (sign && (sign = n < 0))
        x = -n;
    else
        x = n;

    i = 0;
    i = convert(x, base, buf);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        usart_transmit(buf[i]);
}

static void printlong(long n, int base, int sign)
{
    char buf[16];
    int i;
    unsigned long x;

    if (sign && (sign = n < 0))
        x = -n;
    else
        x = n;

    i = 0;
    i = convert(x, base, buf);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        usart_transmit(buf[i]);
}

static void printptr(unsigned int n)
{
    int i;
    char buf[16];

    i = convert(n, 16, buf); 

    usart_transmit('0');
    usart_transmit('x');

    while (--i >= 0)
        usart_transmit(buf[i]);
}

void printf(char* fmt, ...)
{
    va_list ap;
    char *p;
    int ival;
    char* sval; 
    unsigned long ptrval;
    unsigned long lval;

    p = fmt;

    va_start(ap, fmt);

    while (*p != 0) {
        if (*p != '%') {
            usart_transmit(*p);
            p++;
            continue;
        }
        p++;
        switch (*p)
        {
        case 'd':
            ival = va_arg(ap, int);
            printint(ival, 10, 1);
            break;
        case 'f':
            break;
        case 'x':
            ival = va_arg(ap, unsigned int);
            printint(ival, 16, 1);
            break;
        case 'l':
            lval = va_arg(ap, long);
            printlong(lval, 10, 1);
            break;
        case 's':
            if((sval = va_arg(ap, char*)) == 0)
                sval = "(null)";
            for(; *sval; sval++)
                usart_transmit(*sval);
            break;
        case 'p':
            ptrval = va_arg(ap, unsigned long);
            printptr(ptrval);
        break;
        }
        p++;
    }
    va_end(ap);
}
