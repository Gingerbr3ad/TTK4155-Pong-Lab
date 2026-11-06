#pragma once

#include "utils/board_macros.h"

#include <stdio.h>
#include <util/setbaud.h>

#define BufferSize 64 
#define UART_RECEIVE_BUFFER_ADDR 0x1880
#define UART_PARSE_BUFFER_ADDR 0x18C0

extern volatile uint8_t uart_recieved_flag;

typedef enum {
    UART_COMMAND_NONE,
    UART_COMMAND_TEST,
    UART_COMMAND_OLED_CLEAR,
    UART_COMMAND_OLED_CHECKERS,
    UART_COMMAND_PRINT_DEBUG,
    UART_COMMAND_SRAM_TEST,
    UART_COMMAND_CAN_TEST,
    UART_COMMAND_UI_UP,
    UART_COMMAND_UI_DOWN,
    UART_COMMAND_UI_SELECT,
    UART_COMMAND_UI_BACK,
    UART_COMMAND_GAME_FINISHED
} uart_command_t;

void uart_init();
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
uart_command_t handle_uart_interrupt();