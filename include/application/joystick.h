#include "system_definitions.h"

#define JOYSTICK_X_CHANNEL 0
#define JOYSTICK_Y_CHANNEL 1

#define JOYSTICK_DEADZONE 20

typedef struct {
    int16_t x;
    int16_t y;
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

joystick_position_t joystick_read_position();
joystick_direction_t joystick_get_direction();
