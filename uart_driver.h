#include "system_definitions.h"

#include <stdio.h>
#include <util/setbaud.h>
#include <avr/io.h>

void uart_init();
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);