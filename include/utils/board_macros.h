#pragma once

#include <avr/io.h>

/*######### EXTERNAL DEVICE ADDRESS MAPPING #########*/
#define ADC_XMEM_OFFSET 0x1000
#define SRAM_XMEM_OFFSET 0x1400
/*###################################################*/

/*##################### MACROS ######################*/
#define set_bit(reg, bit) (reg |= (1 << bit))
#define clear_bit(reg, bit) (reg &= ~(1 << bit ))
#define test_bit(reg, bit) (reg & (1 << bit))
/*###################################################*/

/*###################### PINS #######################*/
#define OLED_SS {&DDRB, &PORTB, PB3}

#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7

#define OLED_DC 2

#define JOYSTICK_X_CHANNEL 3
#define JOYSTICK_Y_CHANNEL 2
#define TOUCH_X_CHANNEL 1
#define TOUCH_Y_CHANNEL 0
/*###################################################*/