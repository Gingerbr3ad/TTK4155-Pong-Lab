#pragma once

#include <avr/io.h>

#define ADC_XMEM_OFFSET 0x1000
#define SRAM_XMEM_OFFSET 0x1400

#define set_bit(reg, bit) (reg |= (1 << bit))
#define clear_bit(reg, bit) (reg &= ~(1 << bit ))
#define test_bit(reg, bit) (reg & (1 << bit))

typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    int bit;
} spi_ss_pin_t;

#define OLED_SS {&DDRB, &PORTB, PB3}

extern const spi_ss_pin_t oled_ss;
extern const spi_ss_pin_t slaves[1];