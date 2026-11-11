#pragma once

#include "drivers/can_driver.h"
#include "sam.h"

// Bit 0 - speed control, Bit 1 - position control, Bit 2 - position + slider, Bit 3 - Normal mode, Bit 4 - Hyper mode, Bit 5 - NC, Bit 6 - NC, Bit 7 - game on
extern volatile uint8_t game_config;

void game_config_update();
void game_run(CanMsg rx);