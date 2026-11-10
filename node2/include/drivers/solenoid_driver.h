#pragma once
#include <stdint.h>

void solenoid_init();
void solenoid_pulse(uint16_t duration_ms);
void solenoid_set_state();