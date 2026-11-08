#pragma once
#include "utils/board_macros.h"

#include <stdint.h>

void pong_game_enter();
uint8_t pong_game_update();
void pong_game_system_tick();
void pong_game_over_enter();
uint8_t pong_game_over_update();