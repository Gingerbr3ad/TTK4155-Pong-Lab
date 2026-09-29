#pragma once

#include "utils/board_macros.h"

#include <stdio.h>
#include <util/setbaud.h>

#define BufferSize 64 

extern volatile uint8_t uart_recieved_flag;

void uart_init();
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
void handle_uart_interrupt();