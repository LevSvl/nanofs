#ifndef CONSOLE_H
#define CONSOLE_H

void usart_init();
char usart_getc(void);
void console_init();
void printf(char* fmt, ...);
void console_read(char *dst, int size);

#endif // CONSOLE_H
