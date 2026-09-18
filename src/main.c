#include "system_definitions.h"

#include <util/delay.h>
#include <stdio.h>

#include "drivers/uart_driver.h"
#include "drivers/external_memory_driver.h"
#include "drivers/adc_driver.h"

#define WAVE_PIN PB1
#define ERROR_LED PB0

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void system_init() {
    uart_init();
    stdout = stdin = &uartstdout; // Replace the defualt stdout/in stream with the custom uart one
    external_memory_init();
    adc_init();
}

int main(void) {
    system_init();
    /*s
    set_bit(DDRB, WAVE_PIN);

    while(1) {
        set_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
        clear_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
    }
    */

    while(1) { 
        volatile char * a = (char *) 0x13ff; //0001 0011 1111 1111
        *a = 0xaa;
        _delay_ms(500);
        volatile char * b = (char *) 0x17ff; //0001 0011 1111 1111
        *b = 0xaa;
        _delay_ms(500);
        *a = 0xab;
        _delay_ms(500);
        volatile char * c = (char *) 0x1800; //0001 1000 0000 0000
        *c = 0xaa;
        _delay_ms(500);
        *a = 0xac;
        _delay_ms(500);
        volatile char * d = (char *) 0xc00; //0001 1100 0000 0000
        *d = 0xaa;
        _delay_ms(500);
        *a = 0xad;
        _delay_ms(500);

        char ch = getchar(); // Waits until it gets a character on the stdin stream
        printf("Hello, PC! You've sent me this: %c\n", ch); 
    }
    
    return 0;
}