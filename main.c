 
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#include "uart_driver.h"


#define set_bit(reg, bit) (reg |= (1 << bit))
#define clear_bit(reg, bit) (reg &= ~(1 << bit ))
#define test_bit(reg, bit) (reg & (1 << bit))

#define WAVE_PIN PB1
#define F_CPU 1000000UL
#define UBRR 31

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main(void) {    
    /*
    set_bit(DDRB, WAVE_PIN);

    while(1) {
        set_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
        clear_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
    }
    */

  uart_init(UBRR);
  stdout = &uartstdout;
  printf("Hello, world!\n");
  return 0;
}