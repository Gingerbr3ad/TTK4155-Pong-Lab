#include "application/oled/oled_animations.h"
static uint8_t pattern = 0xF0;
void shutters() {
    memset(FRAMEBUFFER, pattern, FRAMEBUFFER_SIZE);
    pattern = (pattern >> 1) | (pattern << 7);
    framebuffer_updated_flag = 1;
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

    framebuffer_updated_flag = 1;
}

void bouncing_ball_animation(void)
{
    #define SCREEN_WIDTH   128
    #define SCREEN_HEIGHT   64
    #define BALL_RADIUS      2
    #define ANIMATION_FRAMES (24 * 5)

    static int16_t x = SCREEN_WIDTH / 2;
    static int16_t y = SCREEN_HEIGHT / 2;

    static int8_t vx = 1;
    static int8_t vy = 1;

    static uint8_t frame = 0;

    // State for a small pseudo-random generator
    static uint16_t random_state = 0xACE1u;

    /*
     * Every 5 seconds:
     * - put ball back in centre
     * - generate new random velocity
     */
    if (frame == 0)
    {
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT / 2;

        // Generate next pseudo-random value
        random_state ^= random_state << 7;
        random_state ^= random_state >> 9;
        random_state ^= random_state << 8;

        /*
         * Random horizontal velocity:
         *
         * magnitude = 1 or 2
         * direction = left or right
         */
        vx = (random_state & 0x01) ? 1 : 2;

        if (random_state & 0x02)
        {
            vx = -vx;
        }

        // Generate another value for vertical velocity
        random_state ^= random_state << 7;
        random_state ^= random_state >> 9;
        random_state ^= random_state << 8;

        /*
         * Random vertical velocity:
         *
         * magnitude = 1 or 2
         * direction = up or down
         */
        vy = (random_state & 0x01) ? 1 : 2;

        if (random_state & 0x02)
        {
            vy = -vy;
        }
    }

    /*
     * Clear previous frame.
     */
    memset(FRAMEBUFFER, 0x00, FRAMEBUFFER_SIZE);

    /*
     * Draw a small circular ball.
     *
     * This produces roughly:
     *
     *   XXX
     *  XXXXX
     *  XXXXX
     *  XXXXX
     *   XXX
     */
    for (int8_t dy = -BALL_RADIUS; dy <= BALL_RADIUS; dy++)
    {
        for (int8_t dx = -BALL_RADIUS; dx <= BALL_RADIUS; dx++)
        {
            // Only draw pixels approximately inside a circle
            if ((dx * dx + dy * dy) <=
                (BALL_RADIUS * BALL_RADIUS))
            {
                uint8_t px = (uint8_t)(x + dx);
                uint8_t py = (uint8_t)(y + dy);

                uint16_t index =
                    px +
                    ((uint16_t)(py >> 3) * SCREEN_WIDTH);

                FRAMEBUFFER[index] |=
                    (1 << (py & 0x07));
            }
        }
    }

    /*
     * Move ball.
     */
    x += vx;
    y += vy;

    /*
     * Left/right collision.
     */
    if (x <= BALL_RADIUS)
    {
        x = BALL_RADIUS;
        vx = -vx;
    }
    else if (x >= (SCREEN_WIDTH - 1 - BALL_RADIUS))
    {
        x = SCREEN_WIDTH - 1 - BALL_RADIUS;
        vx = -vx;
    }

    /*
     * Top/bottom collision.
     */
    if (y <= BALL_RADIUS)
    {
        y = BALL_RADIUS;
        vy = -vy;
    }
    else if (y >= (SCREEN_HEIGHT - 1 - BALL_RADIUS))
    {
        y = SCREEN_HEIGHT - 1 - BALL_RADIUS;
        vy = -vy;
    }

    /*
     * Advance the 5-second animation timer.
     */
    frame++;

    if (frame >= ANIMATION_FRAMES)
    {
        frame = 0;
    }

    framebuffer_updated_flag = 1;
}