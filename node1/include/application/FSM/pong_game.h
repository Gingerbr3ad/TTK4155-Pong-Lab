#pragma once
#include "utils/board_macros.h"

#include <stdint.h>

// Bit 0 - speed control, Bit 1 - position control, Bit 2 - position + slider, Bit 3 - Normal/Hyper mode, Bit 4 - NC, Bit 5 - NC, Bit 6 - NC, Bit 7 - game on
extern volatile uint8_t game_config;

void pong_game_enter();
void pong_game_receive_score(uint32_t value);
void pong_game_ui_tick();
void pong_game_system_tick();

void pong_game_receive_motor_position(uint8_t percent);
