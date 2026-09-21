/* 
This file is for defninitg variables used by the various AVR Libc macros.
Without it calues like F_CPU would need to be defined in each file using it separately,
this way we just need to include this file. 
!!!MAKE SURE THAT THIS IS THE 1ST FILE INCLUDED!!!
*/
#include <avr/io.h>

#define ADC_XMEM_OFFSET 0x1000
#define SRAM_XMEM_OFFSET 0x1400

#define set_bit(reg, bit) (reg |= (1 << bit))
#define clear_bit(reg, bit) (reg &= ~(1 << bit ))
#define test_bit(reg, bit) (reg & (1 << bit))
