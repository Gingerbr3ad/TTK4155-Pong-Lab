#pragma once
#include <stdint.h>

/* Positive command must move physically RIGHT (toward negative counts).
 * Change to 0 if your motor wiring reverses this direction. */
#define MOTOR_RIGHT_DIR_HIGH 1

void motor_init(void);
/* Signed drive, -1000..1000 permille; NOT a measured speed.
 * Call once each 10 ms control tick. Starting/reversing stages PWM for
 * one tick before connecting it to PB12. Keep calls serialized. */
void motor_set(int16_t command);
void motor_stop(void);
