#include "application/input.h"

static joystick_direction_t previous_direction;
static joystick_direction_t direction_edge;

static uint8_t pressed_buttons;

static joystick_direction_t injected_direction;
static uint8_t injected_buttons;


void input_init() {
    previous_direction = JOYSTICK_NEUTRAL;
    direction_edge = JOYSTICK_NEUTRAL;
    injected_direction = JOYSTICK_NEUTRAL;
    injected_buttons = 0;
    pressed_buttons = 0;
}

void input_update() {
    joystick_direction_t direction = joystick_get_direction();

    // If the direction has changed, register it as the edge, else reports neutral edge (no movement)
    direction_edge = JOYSTICK_NEUTRAL;
    if (direction != previous_direction) {
        direction_edge = direction;
    }
    previous_direction = direction;

    // If a direction was injected, it overwrites the next input update with it
    if(injected_direction != JOYSTICK_NEUTRAL) {
        direction_edge = injected_direction;
        injected_direction = JOYSTICK_NEUTRAL;
    }

    // If any buttons were injected, it overwrites the next input update with them
    pressed_buttons = injected_buttons;
    injected_buttons = 0;
}

// Returns the direction edge
joystick_direction_t input_joystick_direction_edge() {
    return direction_edge;
}

uint8_t input_button_pressed(input_button_t button) {
    if (button != INPUT_BUTTON_SELECT && button != INPUT_BUTTON_BACK) {
        return 0;
    }
    return (pressed_buttons & (uint8_t)(1u << (uint8_t)button)) != 0;
}

void input_inject_joystick(joystick_direction_t direction) {
    injected_direction = direction;
}

void input_inject_button(input_button_t button) {
    if (button == INPUT_BUTTON_SELECT || button == INPUT_BUTTON_BACK) {
        injected_buttons |= (uint8_t)(1u << (uint8_t)button);
    }
}
