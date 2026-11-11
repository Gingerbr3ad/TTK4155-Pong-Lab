#pragma once
#include <stdint.h>

extern volatile int16_t POSITION_MAX_LEFT;
extern volatile int16_t POSITION_MAX_RIGHT;
extern volatile int16_t POSITION_CENTER;

void encoder_init();
int32_t encoder_read();
