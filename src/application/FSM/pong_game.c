#include "application/FSM/pong_game.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"

// PLACEHOLDER

void game_enter() {
    framebuffer_clear();
    oled_goto_pos(8, 8);
    oled_write_string_P(PSTR("PLAYING"), OLED_FONT_LARGE);
    oled_goto_pos(32, 8);
    oled_write_string_P(PSTR("Game TODO"), OLED_FONT_NORMAL);
    oled_goto_pos(48, 8);
    oled_write_string_P(PSTR("BACK to menu"), OLED_FONT_SMALL);
}

uint8_t game_update() {
    return input_button_pressed(INPUT_BUTTON_L6);
}

void game_over_enter() {
    framebuffer_clear();
    oled_goto_pos(8, 8);
    oled_write_string_P(PSTR("GAME OVER"), OLED_FONT_LARGE);
    oled_goto_pos(32, 8);
    oled_write_string_P(PSTR("SELECT or BACK"), OLED_FONT_SMALL);
}

uint8_t game_over_update() {
    return input_button_pressed(INPUT_BUTTON_JOYSTICK) ||
           input_button_pressed(INPUT_BUTTON_L6);
}
