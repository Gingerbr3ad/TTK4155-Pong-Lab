#pragma once
#include <stdint.h>

#define POSITION_MAX_LEFT     2400
#define POSITION_MAX_RIGHT   (-2800)
#define POSITION_CONTROL_HZ   100u
/* Drive permille per encoder count; KI is per second. Tune on the board. */
#define POSITION_KP           0.4f
#define POSITION_KI           0.12f
#define POSITION_OUTPUT_LIMIT 650
#define POSITION_TOLERANCE    20

/* Initializes encoder, motor and TC0 channel 0. Holds motor stopped until
 * the first setpoint. Start at the physical encoder-zero reference used
 * to measure the constants above: resetting the MCU resets the count. */
void position_controller_init(void);
/* Joystick -100 = left, +100 = right, 0 = midpoint (-200 counts). */
void position_controller_set_joystick(int8_t x);
void position_controller_stop(void);
