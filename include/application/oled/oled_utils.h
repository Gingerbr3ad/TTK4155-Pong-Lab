#pragma once

#include "utils/board_macros.h"
#include "drivers/spi_driver.h"
#include <util/delay.h>

void oled_command_write(uint8_t command[], int command_len);
void oled_data_write(uint8_t data[], int data_len);
void oled_init();

void oled_test();