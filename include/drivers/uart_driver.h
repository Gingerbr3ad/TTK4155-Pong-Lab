#pragma once

#include "utils/board_macros.h"

#include <stdio.h>
#include <util/setbaud.h>

#define UART0_RECEIVE_INTERRUPT  USART_RXC_vect
#define BufferSize 64 

extern volatile uint8_t line_ready; // Index for the head of the buffer

void uart_init();
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
void handle_uart_interrupt();