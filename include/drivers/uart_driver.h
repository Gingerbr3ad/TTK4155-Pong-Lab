#pragma once

#include "utils/board_macros.h"

#include <stdio.h>
#include <util/setbaud.h>

void uart_init();
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);