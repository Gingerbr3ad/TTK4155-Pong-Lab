#ifndef JOYSTICK_H
#define JOYSTICK_H


#include <stdint.h>

typedef struct
{
    int16_t x;
    int16_t y;
} joystick_position_t;

typedef enum
{
    JOYSTICK_NEUTRAL,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_UP,
    JOYSTICK_DOWN
} joystick_direction_t;

void joystick_init(void);

joystick_position_t joystick_read_position(void);

joystick_direction_t joystick_get_direction(void);

#endif
