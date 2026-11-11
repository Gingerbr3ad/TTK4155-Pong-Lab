#include "application/FSM/pong_game.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"
#include "drivers/can_driver.h"

joystick_position_t joy_pos;
joystick_position_t prev_joy_pos;
uint8_t shoot_button;

can_message_t joy_message = {
    .id = JOYSTICK_MESSAGE_ID,
    .length = 3,
    .data = {0x00, 0x00, 0x00}
};

void pong_game_enter() {
    framebuffer_clear();
    oled_goto_pos(8, 8);
    oled_write_string_P(PSTR("PLAYING"), OLED_FONT_LARGE);
    oled_goto_pos(32, 8);
    oled_write_string_P(PSTR("Game TODO"), OLED_FONT_NORMAL);
    oled_goto_pos(48, 8);
    oled_write_string_P(PSTR("BACK to menu"), OLED_FONT_SMALL);
}

uint8_t pong_game_update() {
    return input_button_pressed(INPUT_BUTTON_L6);
}

void pong_game_system_tick() {
    joy_pos =  input_joystick_position_read();
    shoot_button = input_button_pressed(INPUT_BUTTON_JOYSTICK);
    if(joy_pos.x != prev_joy_pos.x || joy_pos.y != prev_joy_pos.y || shoot_button) {
        joy_message.data[0] = joy_pos.x;
        joy_message.data[1] = joy_pos.y;
        joy_message.data[2] = shoot_button;

        can_send(&joy_message);
        prev_joy_pos = joy_pos;
    }
}

void pong_game_over_enter() {
    framebuffer_clear();
    oled_goto_pos(8, 8);
    oled_write_string_P(PSTR("GAME OVER"), OLED_FONT_LARGE);
    oled_goto_pos(32, 8);
    oled_write_string_P(PSTR("SELECT or BACK"), OLED_FONT_SMALL);
}

uint8_t pong_game_over_update() {
    return input_button_pressed(INPUT_BUTTON_JOYSTICK) || input_button_pressed(INPUT_BUTTON_L6);
}
