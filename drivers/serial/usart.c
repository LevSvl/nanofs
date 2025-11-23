#include "types.h"
#include "m328Pdef.h"

#define BAUD_RATE 38400
#define UBRR ((SYS_FREQ/(16UL*BAUD_RATE))-1)

void usart_init()
{
    // setting the baud rate
    UBRR0H = (uint8_t)(UBRR>>8);
    UBRR0L = (uint8_t)UBRR;

    // setting frame format
    UCSR0C = (0<<USBS0) | (3<<UCSZ00);

    // enabling the transmitter
    UCSR0B = (1<<TXEN0);
}

static void usart_putc(uint8_t ch)
{
    while ((UCSR0A & (1<<UDRE0)) == 0)
        ;
    UDR0 = ch;   
}

void usart_transmit(uint8_t ch)
{
    if (ch == '\n')
        usart_putc('\r');
    usart_putc(ch);
}
