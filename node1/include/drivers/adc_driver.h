#pragma once

#include "utils/board_macros.h"

#define ADC_RW_ADDRESS_DUMMY 0xaa //dummy address for reading and wrting to adc

void adc_init();

void adc_write_strobe();
int adc_read_strobe();

int adc_read(int channel);
