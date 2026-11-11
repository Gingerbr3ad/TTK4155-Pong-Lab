#pragma once
#include "utils/board_macros.h"

#include <stdint.h>

void pong_game_enter();
void pong_game_receive_score(uint32_t value);
void pong_game_ui_tick();
void pong_game_system_tick();
