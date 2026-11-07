#pragma once

#include "utils/board_macros.h"

#define JOYSTICK_DEADZONE 20
#define JOYSTICK_FILTER_SIZE 4
#define JOYSTICK_RAW_MIN 0
#define JOYSTICK_RAW_MAX 255

typedef struct {
    int8_t x;
    int8_t y;
} touch_position_t;

typedef struct {
    int8_t x;
    int8_t y;
} joystick_position_t;

typedef enum {
    JOYSTICK_NEUTRAL,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_UP,
    JOYSTICK_DOWN
} joystick_direction_t;

typedef struct {
    uint8_t center_x;
    uint8_t center_y;

    uint8_t min_x;
    uint8_t max_x;

    uint8_t min_y;
    uint8_t max_y;
} joystick_calibration_t;

void joystick_init();

touch_position_t touch_read_position();

joystick_position_t joystick_read_position();
joystick_direction_t joystick_get_direction(joystick_position_t position);
uint8_t joystick_button_read();
