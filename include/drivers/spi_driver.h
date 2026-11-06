#pragma once

#include "utils/board_macros.h"

typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    int bit;
} spi_ss_pin_t;

extern const spi_ss_pin_t slaves[3];
#define oled_ss (slaves[0])
#define io_ss (slaves[1])
#define mcp2515_ss (slaves[2])

void spi_init();

uint8_t * spi_r_transmit(int data_len, spi_ss_pin_t slave, uint8_t * return_buff);
void spi_w_transmit(uint8_t data[], int data_len, spi_ss_pin_t slave);
uint8_t * spi_wr_transmit(uint8_t data[], int data_len, spi_ss_pin_t slave, uint8_t * return_buff);

uint8_t * spi_transmit(uint8_t data[], int data_len, uint8_t * return_buff);
uint8_t spi_read();
void spi_write(uint8_t cData);
void select_slave(spi_ss_pin_t slave);
void deselect_slave(spi_ss_pin_t slave);