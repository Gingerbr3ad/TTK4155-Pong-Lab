#pragma once

#include <stdint.h>
#include "application/joystick.h"

typedef enum {
    INPUT_BUTTON_SELECT,
    INPUT_BUTTON_BACK
} input_button_t;

void input_init();

void input_update();
joystick_direction_t input_joystick_direction_edge();
uint8_t input_button_pressed(input_button_t button);

void input_inject_joystick(joystick_direction_t direction);
void input_inject_button(input_button_t button);