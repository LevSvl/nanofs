#ifndef CONSOLE_H
#define CONSOLE_H

void usart_init();
char usart_getc(void);
void console_init();
void printf(char* fmt, ...);
void console_read(char *dst, int size);
void run_shell(void);

#define printk(fmt, ...) \
do { \
    printf("[%l] ", uptime()); \
    printf(fmt, ## __VA_ARGS__); \
} \
while (0);

#endif // CONSOLE_H
