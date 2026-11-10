#pragma once
#include <stdint.h>

#define MOTOR_RIGHT_DIR_HIGH 1


void motor_init();
void motor_set(int16_t command);
void motor_stop();
