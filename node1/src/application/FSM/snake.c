#include "application/FSM/snake.h"
#include "application/FSM/fsm.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"
#include "application/oled/sprites.h"
#include <stdlib.h>

#define SNAKE_COLUMNS (128 / SNAKE_BLOCK_WIDTH)
#define SNAKE_ROWS (64 / SNAKE_BLOCK_HEIGHT)
#define SNAKE_MAX_LENGTH (SNAKE_COLUMNS * SNAKE_ROWS)
#define SNAKE_MOVE_TICKS 16 // About 6 moves per second at the 96 Hz system tick.

typedef struct {
    uint8_t x;
    uint8_t y;
} snake_cell_t;

// 882 cells * 2 bytes = 1764 bytes, using 0x1900 through 0x1FE3
#define SNAKE_BODY ((snake_cell_t *)(uintptr_t)0x1900)

static uint16_t length;
static snake_cell_t food;
static joystick_direction_t direction;
static joystick_direction_t next_direction;
static uint8_t move_ticks;
static uint8_t game_over;


// For a given cell check if any of the snake blocks are in it
static uint8_t snake_occupies(snake_cell_t cell, uint16_t count) {
    for (uint16_t i = 0; i < count; ++i) {
        if (SNAKE_BODY[i].x == cell.x && SNAKE_BODY[i].y == cell.y) {return 1;}
    }
    return 0;
}


static void snake_place_food() {
    if (length >= SNAKE_MAX_LENGTH) {return;} // Grid is full

    // Generate a random starting point
    food.x = rand() % SNAKE_COLUMNS;
    food.y = rand() % SNAKE_ROWS;

    // Keep searching unitl a free spot to place the food is found
    while (snake_occupies(food, length)) {
        food.x = rand() % SNAKE_COLUMNS;
        food.y = rand() % SNAKE_ROWS;
    }
}

static void snake_draw_block(snake_cell_t cell) {
    oled_goto_pos(cell.y*SNAKE_BLOCK_HEIGHT, 1 + cell.x*SNAKE_BLOCK_WIDTH);
    oled_draw_sprite(snake_block, SNAKE_BLOCK_WIDTH, SNAKE_BLOCK_HEIGHT);
}

static void snake_draw() {
    framebuffer_clear();
    // Draw the snake
    for (uint16_t i = 0; i < length; ++i) {snake_draw_block(SNAKE_BODY[i]);}
    // Draw the food
    oled_goto_pos(food.y*SNAKE_BLOCK_HEIGHT, 1 + food.x*SNAKE_BLOCK_WIDTH);
    oled_draw_sprite(snake_block, SNAKE_BLOCK_WIDTH, SNAKE_BLOCK_HEIGHT);
}

static void snake_finish() {
    game_over = 1;
    framebuffer_clear();

    oled_goto_pos(8, 20);
    if (length == SNAKE_MAX_LENGTH) {oled_write_string_P(PSTR("YOU WIN"), OLED_FONT_LARGE);}
    else {oled_write_string_P(PSTR("GAME OVER"), OLED_FONT_LARGE);}

    oled_goto_pos(32, 4);
    oled_write_string_P(PSTR("Joy / Center: Restart"), OLED_FONT_SMALL);
    oled_goto_pos(48, 4);
    oled_write_string_P(PSTR("L6: Menu"), OLED_FONT_SMALL);
}

void snake_enter() {
    length = 3;
    direction = JOYSTICK_RIGHT;
    next_direction = direction;
    move_ticks = 0;
    game_over = 0;
    // Create the snake roughly in the middle of the grid
    for (uint16_t i = 0; i < length; ++i) {
        SNAKE_BODY[i] = (snake_cell_t){SNAKE_COLUMNS / 2 - i, SNAKE_ROWS / 2};
    }
    snake_place_food();
    snake_draw();
}

void snake_system_tick() {
    if (input_button_pressed(INPUT_BUTTON_L6)) {fsm_handle_event(FSM_EVENT_MENU_ENTER); return;}
    if (game_over) {if (input_button_pressed(INPUT_BUTTON_JOYSTICK) || input_button_pressed(INPUT_BUTTON_NAV_CENTER)) {snake_enter();}return;}

joystick_direction_t input = input_joystick_direction_edge();

    // Read NAV buttons if no joystick direction was detected
    if (input == JOYSTICK_NEUTRAL) {
        if (input_button_pressed(INPUT_BUTTON_NAV_UP)) {
            input = JOYSTICK_UP;
        } else if (input_button_pressed(INPUT_BUTTON_NAV_DOWN)) {
            input = JOYSTICK_DOWN;
        } else if (input_button_pressed(INPUT_BUTTON_NAV_LEFT)) {
            input = JOYSTICK_LEFT;
        } else if (input_button_pressed(INPUT_BUTTON_NAV_RIGHT)) {
            input = JOYSTICK_RIGHT;
        }
    }

    // Update direction, preventing 180-degree turns
    if (input != JOYSTICK_NEUTRAL) {
        if (!((direction == JOYSTICK_LEFT && input == JOYSTICK_RIGHT) || (direction == JOYSTICK_RIGHT && input == JOYSTICK_LEFT) || (direction == JOYSTICK_UP && input == JOYSTICK_DOWN) || (direction == JOYSTICK_DOWN && input == JOYSTICK_UP))) {
            next_direction = input;
        }
    }

    if (++move_ticks < SNAKE_MOVE_TICKS) {return;}
    move_ticks = 0;
    direction = next_direction;
    snake_cell_t head = SNAKE_BODY[0];
    switch (direction) {
        case JOYSTICK_LEFT: --head.x; break;
        case JOYSTICK_RIGHT: ++head.x; break;
        case JOYSTICK_UP: --head.y; break;
        case JOYSTICK_DOWN: ++head.y; break;
        default: break;
    }

    // Loose if the edge is hit by the head
    if (head.x >= SNAKE_COLUMNS || head.y >= SNAKE_ROWS ) {snake_finish(); return;}
    // Set grow to one if head is in the same cell as food
    uint8_t grow = (head.x == food.x && head.y == food.y);
    // Loose if any snake body cell is hit by the head
    if (snake_occupies(head, length - 1)) {snake_finish(); return;}
    if (grow) {++length;}
    // SLANGEN BEVEGER SEG, TUNG, TREIG ... OG SULTEN
    for (uint16_t i = length - 1; i > 0; --i) {SNAKE_BODY[i] = SNAKE_BODY[i - 1];}
    SNAKE_BODY[0] = head;

    if (length == SNAKE_MAX_LENGTH) {snake_finish(); return;}
    if (grow) {snake_place_food();}
    snake_draw();
}
