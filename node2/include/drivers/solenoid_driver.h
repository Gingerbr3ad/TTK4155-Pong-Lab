#pragma once
#include <stdint.h>

void solenoid_init();

void solenoid_set_state(uint8_t state);
void solenoid_pulse();