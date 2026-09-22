#include "system_definitions.h"

#include <util/delay.h>
#include <stdio.h>

#include "drivers/uart_driver.h"
#include "drivers/external_memory_driver.h"
#include "drivers/adc_driver.h"

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void system_init() {
    uart_init();
    stdout = stdin = &uartstdout; // Replace the defualt stdout/in stream with the custom uart one
    external_memory_init();
    adc_init();
}

int main(void) {
    system_init();
    int adc_data = 0;
    while(1) { 
        adc_data = adc_read(1);
        //char ch = getchar(); // Waits until it gets a character on the stdin stream
        printf("I've got this from the ADC: %i\n", adc_data);
        _delay_ms(1000);
    }
    
    return 0;
}