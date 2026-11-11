#pragma once

extern volatile uint8_t system_tick_60Hz_flag;

void system_tick_60Hz_init();
uint8_t system_tick_60Hz_take();