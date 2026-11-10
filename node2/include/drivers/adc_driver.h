#pragma once
#include <stdint.h>

void adc_init();
uint16_t adc_read();
uint16_t adc_read_average(uint8_t samples);
uint16_t adc_state();