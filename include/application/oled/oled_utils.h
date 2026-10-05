#pragma once

#include "utils/board_macros.h"
#include "drivers/spi_driver.h"
#include <util/delay.h>

#define FRAMEBUFFER_ADDR  0x1400
#define FRAMEBUFFER_SIZE 1024
#define FRAMEBUFFER ((uint8_t *)(uintptr_t)FRAMEBUFFER_ADDR)

extern volatile uint8_t display_update_flag;
extern volatile uint8_t framebuffer_updated_flag;

void oled_command_write(uint8_t command[], int command_len);
void oled_data_write(uint8_t data[], int data_len);
void display_update_timer_init();
void oled_init();

void oled_flush();

void oled_clear();
void oled_checkerboard_test();