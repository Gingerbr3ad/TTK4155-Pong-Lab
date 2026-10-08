#pragma once

#include "utils/board_macros.h"
#include "drivers/spi_driver.h"
#include "application/oled/fonts.h"
#include <util/delay.h>

#define FRAMEBUFFER_ADDR  0x1400 // Framebuffer is set to addresses 0x1400 - 0x17FF
#define FRAMEBUFFER_SIZE 1024
#define FRAMEBUFFER ((uint8_t *)(uintptr_t)FRAMEBUFFER_ADDR)

#define PAGEBUFFER_ADDR  0x1800 // Pagebuffer is set to addresses 0x1800 - 187F
#define PAGEBUFFER_SIZE 128
#define PAGEBUFFER ((uint8_t *)(uintptr_t)PAGEBUFFER_ADDR)

void oled_write_char(char c);
void oled_write_string(const char *str);

void oled_init();

void oled_flush();
void framebuffer_clear();
void oled_checkerboard_test();