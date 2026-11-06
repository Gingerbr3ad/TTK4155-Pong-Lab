#pragma once
#include "utils/board_macros.h"

#include <stdint.h>

void game_enter();
uint8_t game_update();
void game_system_update();
void game_over_enter();
uint8_t game_over_update();