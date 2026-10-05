#include "application/oled/oled_animations.h"

void shutters() {
    static uint8_t pattern = 0xF0;
    memset(FRAMEBUFFER, pattern, FRAMEBUFFER_SIZE);
    pattern = (pattern >> 1) | (pattern << 7);
}