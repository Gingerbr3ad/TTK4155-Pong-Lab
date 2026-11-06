#pragma once

#include <stdint.h>
#include "application/joystick.h"

typedef enum {
    INPUT_BUTTON_JOYSTICK,
    INPUT_BUTTON_R1,
    INPUT_BUTTON_R2,
    INPUT_BUTTON_R3,
    INPUT_BUTTON_R4,
    INPUT_BUTTON_R5,
    INPUT_BUTTON_R6,
    INPUT_BUTTON_L1,
    INPUT_BUTTON_L2,
    INPUT_BUTTON_L3,
    INPUT_BUTTON_L4,
    INPUT_BUTTON_L5,
    INPUT_BUTTON_L6,
    INPUT_BUTTON_L7,
    INPUT_BUTTON_NAV_CENTER,
    INPUT_BUTTON_NAV_RIGHT,
    INPUT_BUTTON_NAV_LEFT,
    INPUT_BUTTON_NAV_DOWN,
    INPUT_BUTTON_NAV_UP,
    INPUT_BUTTON_COUNT
} input_button_t;

typedef struct {
    uint8_t right; // Bits 0..5: R1..R6
    uint8_t left;  // Bits 0..6: L1..L7
    uint8_t nav;   // Bits 0..4: center, right, left, down, up
} input_buttons_t;

typedef struct {
    uint8_t position; // Slider X byte (0..255)
    uint8_t size;     // Slider size byte
} input_slider_t;


void input_init();
void input_update();

joystick_position_t input_joystick_position_read();
joystick_direction_t input_joystick_direction_read();
joystick_direction_t input_joystick_direction_edge();

touch_position_t input_touch_position_read();

uint8_t input_button_down(input_button_t button);
uint8_t input_button_pressed(input_button_t button);
uint8_t input_button_released(input_button_t button);
uint32_t input_buttons_read();

input_slider_t input_slider_read();

void input_inject_joystick(joystick_direction_t direction);
void input_inject_button(input_button_t button);
