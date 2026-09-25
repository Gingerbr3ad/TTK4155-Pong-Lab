#pragma once

#include "utils/board_macros.h"

#define JOYSTICK_X_CHANNEL 3
#define JOYSTICK_Y_CHANNEL 2
#define TOUCH_X_CHANNEL 1
#define TOUCH_Y_CHANNEL 0

#define JOYSTICK_DEADZONE 20

typedef struct {
    int x;
    int y;
} touch_position_t;

typedef struct {
    int x;
    int y;
} joystick_position_t;

typedef enum {
    JOYSTICK_NEUTRAL,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_UP,
    JOYSTICK_DOWN
} joystick_direction_t;

typedef struct {
    int center_x;
    int center_y;

    int min_x;
    int max_x;

    int min_y;
    int max_y;

} joystick_calibration_t;

void joystick_init();

touch_position_t touch_read_position();

joystick_position_t joystick_read_position();
joystick_direction_t joystick_get_direction();
