#include <stdio.h>

void uart_init(unsigned int ubrr);
int uart_putchar(char c, FILE *stream);