#pragma once
#include <stdint.h>

#define POSITION_CONTROL_LOOP_HZ 100
#define MOTOR_EFFORT_LIMIT 1000
#define POSITION_TOLERANCE 4

#define KP 0.6
#define KI 0.7
#define KD 0.12

void position_controller_init();
void position_controller_map_target(int32_t input, int32_t in_min, int32_t in_max, int32_t min_position, int32_t max_position);

void position_controller_stop();
void position_controller_start();

void set_tension(uint8_t tension);