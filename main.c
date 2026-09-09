#include "system_definitions.h"

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#include "uart_driver.h"
#include "external_memory_driver.h"

#define WAVE_PIN PB1
#define ERROR_LED PB0

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void) {    
    set_bit(DDRB, ERROR_LED);
    /*
    set_bit(DDRB, WAVE_PIN);

    while(1) {
        set_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
        clear_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
    }
    */

    uart_init();
    stdout = stdin = &uartstdout; // Replace the defualt stdout/in stream with the custom uart one

    external_memory_init();
    
    while(1) { 
        int address = 4096;
        while(address < 4351) {
            unsigned char *p = (unsigned char *) address;
            *p = 0xaa;
            ++address;
            _delay_ms(80);
        }
        

        //char c = getchar(); // Waits until it gets a character on the stdin stream
        //printf("Hello, PC! You've sent me this: %c\n", c); 
    }
    
    return 0;
}