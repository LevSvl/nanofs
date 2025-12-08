#ifndef CONSOLE_H
#define CONSOLE_H

void usart_init();
char usart_getc(void);
void console_init();
void printf(char* fmt, ...);
void console_read(char *dst, int size);
void run_shell(void);

#endif // CONSOLE_H
