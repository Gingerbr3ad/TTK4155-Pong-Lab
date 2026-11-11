#include "application/FSM/pong_game.h"
#include "application/FSM/fsm.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"
#include "drivers/can_driver.h"
#include <stdlib.h>

static uint32_t score;
static uint8_t game_over;
static uint8_t game_over_drawn;
static uint8_t game_status_message_pending;

volatile uint8_t game_config;

joystick_position_t joy_pos;
joystick_position_t prev_joy_pos;
input_slider_t slider_pos;
input_slider_t prev_slider_pos;
uint8_t shoot_button;
uint8_t prev_shoot_button;

can_message_t joy_message = {
    .id = JOYSTICK_MESSAGE_ID,
    .length = 3,
    .data = {0x00, 0x00, 0x00}
};

can_message_t game_status_message = {
    .id = GAME_STATUS_MESSAGE_ID, 
    .length = 1,
    .data = {0x00},
};


static void pong_game_draw_score() {
    char text[11]; // Ten decimal digits and the terminating zero.
    ultoa(score, text, 10); // This converts the unsigned integer into string
    oled_goto_pos(32, 8);
    oled_write_string_P(PSTR("Score: "), OLED_FONT_NORMAL);
    oled_write_string(text, OLED_FONT_NORMAL);
}

static void pong_game_over_draw() {
    framebuffer_clear();
    oled_goto_pos(8, 8);
    oled_write_string_P(PSTR("GAME OVER"), OLED_FONT_LARGE);
    pong_game_draw_score();
    oled_goto_pos(48, 8);
    oled_write_string_P(PSTR("SELECT or BACK"), OLED_FONT_SMALL);
}

void pong_game_enter() {
    game_config |= 1 << 7;
    score = 0;
    game_over = 0;
    game_over_drawn = 0;
    game_status_message.data[0] = game_config;
    game_status_message_pending = 1;

    framebuffer_clear();
    oled_goto_pos(8, 8);
    oled_write_string_P(PSTR("PLAYING"), OLED_FONT_LARGE);
    pong_game_draw_score();
}

void pong_game_receive_score(uint32_t value) {
    if (value) {score = value;}
    else {
        game_over = 1;
        game_status_message.data[0] = 0;
        game_status_message_pending = 1;
    }
}

void pong_game_ui_tick() {
    if (!game_over) {pong_game_draw_score();}
}

void pong_game_system_tick() {
    if (game_status_message_pending) {if(!can_send(&game_status_message)) {game_status_message_pending = 0;}}
    if (game_over) {
        if (!game_over_drawn) {pong_game_over_draw(); game_over_drawn = 1; return;}
        if (input_button_pressed(INPUT_BUTTON_L6) || input_button_pressed(INPUT_BUTTON_R6)) {fsm_handle_event(FSM_EVENT_MENU_ENTER); return;}
        if (input_button_pressed(INPUT_BUTTON_JOYSTICK)) {fsm_handle_event(FSM_EVENT_START_GAME); return;}
    }

    // Manually end the game
    if (input_button_pressed(INPUT_BUTTON_R5)) {pong_game_receive_score(0); return;}

    // Speed control
    if ((game_config >> 0) & 1) {
        joy_pos = input_joystick_position_read();
        shoot_button = (input_button_pressed(INPUT_BUTTON_JOYSTICK) || input_button_pressed(INPUT_BUTTON_R6));
        
        if(joy_pos.x != prev_joy_pos.x || joy_pos.y != prev_joy_pos.y || shoot_button != prev_shoot_button) {
            joy_message.data[0] = joy_pos.x;
            joy_message.data[1] = joy_pos.y;
            joy_message.data[2] = shoot_button;
        }

        if (!can_send(&joy_message)) {prev_joy_pos = joy_pos; prev_shoot_button = shoot_button;}
    }

    // Position control
    if ((game_config >> 1) & 1) {
        joy_pos = input_joystick_position_read();
        shoot_button = (input_button_pressed(INPUT_BUTTON_JOYSTICK) || input_button_pressed(INPUT_BUTTON_R6));
        
        if(joy_pos.x != prev_joy_pos.x || joy_pos.y != prev_joy_pos.y || shoot_button != prev_shoot_button) {
            joy_message.data[0] = joy_pos.x;
            joy_message.data[1] = joy_pos.y;
            joy_message.data[2] = shoot_button;
        }

        if (!can_send(&joy_message)) {prev_joy_pos = joy_pos; prev_shoot_button = shoot_button;}
    }

    // Slider + position control
    if ((game_config >> 2) & 1) {
        joy_pos = input_joystick_position_read();
        slider_pos = input_slider_read();
        shoot_button = (input_button_pressed(INPUT_BUTTON_JOYSTICK) || input_button_pressed(INPUT_BUTTON_R6));
        
        if(slider_pos.position!= prev_slider_pos.position || joy_pos.y != prev_joy_pos.y || shoot_button != prev_shoot_button) {
            joy_message.data[0] = slider_pos.position;
            joy_message.data[1] = joy_pos.y;
            joy_message.data[2] = shoot_button;
        }

        if (!can_send(&joy_message)) {prev_slider_pos = slider_pos; prev_joy_pos = joy_pos; prev_shoot_button = shoot_button;}
    }
}