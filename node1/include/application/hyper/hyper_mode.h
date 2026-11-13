#pragma once

#include <stdint.h>
#include "drivers/joystick_driver.h"


void hyper_mode_start();
void hyper_mode_tick(touch_position_t touch);
void hyper_mode_receive_position(uint8_t percent);
uint8_t hyper_mode_tension();
uint16_t hyper_mode_bonus();
void hyper_mode_draw(uint16_t score);
