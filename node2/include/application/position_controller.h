#pragma once
#include <stdint.h>

#define POSITION_MAX_LEFT     2400
#define POSITION_MAX_RIGHT   (-2800)
#define POSITION_CONTROL_HZ   100u
#define POSITION_KP           0.4f
#define POSITION_KI           0.12f
#define POSITION_OUTPUT_LIMIT 650
#define POSITION_TOLERANCE    20

void position_controller_init();
void position_controller_set_joystick(int8_t x);
void position_controller_stop();
