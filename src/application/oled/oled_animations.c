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
    enum {
        SCREEN_WIDTH     = 128,
        SCREEN_HEIGHT    = 64,
        BALL_RADIUS      = 3,
        ANIMATION_FRAMES = 24 * 5,

        // Q8.8 fixed point: 256 = 1 pixel
        FP_SHIFT = 8,
        FP_ONE   = 1 << FP_SHIFT,

        // General air resistance
        DRAG = 253,

        // Energy retained when bouncing off walls
        WALL_BOUNCE = 235,

        // Energy retained when bouncing off floor
        FLOOR_BOUNCE = 180,

        // Downward acceleration per frame
        // 20 / 256 = 0.078 pixels/frame^2
        GRAVITY = 20,

        // Maximum downward speed
        TERMINAL_VELOCITY = 5 * FP_ONE
    };

    static int32_t x = (SCREEN_WIDTH / 2) * FP_ONE;
    static int32_t y = (SCREEN_HEIGHT / 2) * FP_ONE;

    static int16_t vx = 0;
    static int16_t vy = 0;

    static uint8_t frame = 0;
    static uint16_t random_state = 0xACE1u;

    /*
     * Generate a new ball every 5 seconds.
     */
    if (frame == 0)
    {
        x = (SCREEN_WIDTH / 2) * FP_ONE;
        y = (SCREEN_HEIGHT / 2) * FP_ONE;

        random_state ^= random_state << 7;
        random_state ^= random_state >> 9;
        random_state ^= random_state << 8;

        uint8_t speed_x = 2 + (random_state & 0x03);

        vx = speed_x * FP_ONE;

        if (random_state & 0x08)
            vx = -vx;

        random_state ^= random_state << 7;
        random_state ^= random_state >> 9;
        random_state ^= random_state << 8;

        uint8_t speed_y = 1 + (random_state & 0x03);

        vy = speed_y * FP_ONE;

        if (random_state & 0x08)
            vy = -vy;
    }

    /*
     * Clear previous frame.
     */
    memset(FRAMEBUFFER, 0x00, FRAMEBUFFER_SIZE);

    /*
     * Draw ball.
     */
    int16_t ball_x = x >> FP_SHIFT;
    int16_t ball_y = y >> FP_SHIFT;

    for (int8_t dy = -BALL_RADIUS;
         dy <= BALL_RADIUS;
         dy++)
    {
        for (int8_t dx = -BALL_RADIUS;
             dx <= BALL_RADIUS;
             dx++)
        {
            if ((dx * dx + dy * dy) <=
                (BALL_RADIUS * BALL_RADIUS))
            {
                int16_t px = ball_x + dx;
                int16_t py = ball_y + dy;

                if (px >= 0 &&
                    px < SCREEN_WIDTH &&
                    py >= 0 &&
                    py < SCREEN_HEIGHT)
                {
                    uint16_t index =
                        (uint16_t)px +
                        ((uint16_t)(py >> 3) *
                         SCREEN_WIDTH);

                    FRAMEBUFFER[index] |=
                        (1 << (py & 0x07));
                }
            }
        }
    }

    /*
     * Gravity.
     *
     * Positive Y is downward on the display.
     */
    vy += GRAVITY;

    if (vy > TERMINAL_VELOCITY)
        vy = TERMINAL_VELOCITY;

    /*
     * Move ball.
     */
    x += vx;
    y += vy;

    /*
     * Air resistance.
     */
    vx = ((int32_t)vx * DRAG) >> 8;
    vy = ((int32_t)vy * DRAG) >> 8;

    /*
     * Screen boundaries.
     */
    int32_t left =
        BALL_RADIUS * FP_ONE;

    int32_t right =
        (SCREEN_WIDTH - 1 - BALL_RADIUS) * FP_ONE;

    int32_t top =
        BALL_RADIUS * FP_ONE;

    int32_t bottom =
        (SCREEN_HEIGHT - 1 - BALL_RADIUS) * FP_ONE;

    /*
     * Left wall.
     */
    if (x < left)
    {
        x = left;

        vx = -vx;
        vx = ((int32_t)vx * WALL_BOUNCE) >> 8;
    }

    /*
     * Right wall.
     */
    else if (x > right)
    {
        x = right;

        vx = -vx;
        vx = ((int32_t)vx * WALL_BOUNCE) >> 8;
    }

    /*
     * Ceiling.
     */
    if (y < top)
    {
        y = top;

        vy = -vy;
        vy = ((int32_t)vy * WALL_BOUNCE) >> 8;
    }

    /*
     * Floor.
     */
    else if (y > bottom)
    {
        y = bottom;

        vy = -vy;
        vy = ((int32_t)vy * FLOOR_BOUNCE) >> 8;

        /*
         * If the bounce is very small, stop vertical
         * movement completely so the ball settles.
         */
        if (vy > -80 && vy < 80)
        {
            vy = 0;
        }
    }

    /*
     * Stop tiny horizontal movement.
     */
    if (vx > -16 && vx < 16)
        vx = 0;

    /*
     * Restart after five seconds.
     */
    frame++;

    if (frame >= ANIMATION_FRAMES) {
        frame = 0;
    }
    
    framebuffer_updated_flag = 1;
}
    
    