#pragma once
#include <stdint.h>

#define POSITION_MAX_LEFT 2400
#define POSITION_MAX_RIGHT (-2800)
#define POSITION_CONTROL_LOOP_HZ 100
#define MOTOR_EFFORT_LIMIT 1000
#define POSITION_TOLERANCE 20

extern volatile float KP;
extern volatile float KI;

void position_controller_init();
void position_controller_map_target(int32_t input, int32_t in_min, int32_t in_max, int32_t min_position, int32_t max_position);

void position_controller_stop();
void position_controller_start();
