#include "application/oled/oled_animations.h"
static uint8_t pattern = 0xF0;
void shutters() {
    memset(FRAMEBUFFER, pattern, FRAMEBUFFER_SIZE);
    pattern = (pattern >> 1) | (pattern << 7);
}


// AI GENERATED FOR A QUICK TEST
void expanding_rectangles(void)
{
    static uint8_t step = 0;

    // Start a new animation cycle
    if (step == 0)
    {
        memset(FRAMEBUFFER, 0x00, FRAMEBUFFER_SIZE);
    }

    /*
     * step goes from 0 to 31.
     *
     * At step 0:
     *   x = 63..64
     *   y = 31..32
     *
     * At step 31:
     *   x = 0..127
     *   y = 0..63
     */

    uint8_t dy = step;
    uint8_t dx = ((uint16_t)step * 63) / 31;

    uint8_t x0 = 63 - dx;
    uint8_t x1 = 64 + dx;

    uint8_t y0 = 31 - dy;
    uint8_t y1 = 32 + dy;

    // Draw top and bottom edges
    for (uint8_t x = x0; x <= x1; x++)
    {
        uint16_t top_index =
            x + ((uint16_t)(y0 >> 3) * 128);

        uint16_t bottom_index =
            x + ((uint16_t)(y1 >> 3) * 128);

        FRAMEBUFFER[top_index] |=
            (1 << (y0 & 0x07));

        FRAMEBUFFER[bottom_index] |=
            (1 << (y1 & 0x07));
    }

    // Draw left and right edges
    for (uint8_t y = y0; y <= y1; y++)
    {
        uint16_t left_index =
            x0 + ((uint16_t)(y >> 3) * 128);

        uint16_t right_index =
            x1 + ((uint16_t)(y >> 3) * 128);

        uint8_t bit =
            (1 << (y & 0x07));

        FRAMEBUFFER[left_index] |= bit;
        FRAMEBUFFER[right_index] |= bit;
    }

    // Advance to the next rectangle
    step++;

    // Restart after reaching the screen edges
    if (step > 31)
    {
        step = 0;
    }
}