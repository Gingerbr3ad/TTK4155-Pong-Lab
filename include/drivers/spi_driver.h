#pragma once

#include "utils/board_macros.h"

typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    int bit;
} spi_ss_pin_t;

extern const spi_ss_pin_t oled_ss;
extern const spi_ss_pin_t io_ss;
extern const spi_ss_pin_t slaves[2];

void spi_init();
uint8_t * spi_wr(uint8_t data[], int data_len, spi_ss_pin_t slave, uint8_t * return_buff);
void spi_w(uint8_t data[], int data_len, spi_ss_pin_t slave);
uint8_t * spi_transmit(uint8_t data[], int data_len, uint8_t * return_buff);

void select_slave(spi_ss_pin_t slave);
void deselect_slave(spi_ss_pin_t slave);