#pragma once

#include "utils/board_macros.h"

#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7

void spi_init(const spi_ss_pin_t slaves[], int num_slaves);
uint8_t * spi_wr(uint8_t data[], int data_len, spi_ss_pin_t slave, uint8_t * return_buff);
void spi_w(uint8_t data[], int data_len, spi_ss_pin_t slave);