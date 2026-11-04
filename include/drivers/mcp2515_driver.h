#pragma once

#include "utils/board_macros.h"
#include "drivers/mcp2515.h"

uint8_t mcp_read(uint8_t address);
void mcp_write(uint8_t address, uint8_t value);
void mcp_request_to_send();
uint8_t mcp_read_status();
void mcp_bit_modify(uint8_t address, uint8_t mask, uint8_t value);
uint8_t mcp_init();
