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

typedef enum {
    OLED_FONT_SMALL,
    OLED_FONT_NORMAL,
    OLED_FONT_LARGE
} oled_font_size_t;

typedef struct {
    const uint8_t *data;
    uint8_t width;
} oled_font_t;

void oled_init();

void oled_flush();
void framebuffer_clear();
void oled_invalidate_all();
void oled_checkerboard_test();

void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t col);
void oled_goto_pos(uint8_t y, uint8_t x);
void oled_goto_page(uint8_t page, uint8_t col);


void oled_write_string(char *str, oled_font_size_t size);
void oled_write_string_P(PGM_P str, oled_font_size_t size);
void oled_draw_sprite(const uint8_t *bitmap, uint8_t width, uint8_t height);
void oled_draw_pixel(uint8_t x, uint8_t y, uint8_t on);