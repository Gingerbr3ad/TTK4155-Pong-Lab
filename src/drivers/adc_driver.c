#include "drivers/adc_driver.h"

void adc_init() {
    //Setup of 16 bit timer/counter 1 on pin PD5 (OC1A)
    // Select Clear OC1A on Compare Match, set OC1A at TOP (Non-inverting)
    set_bit(TCCR1A, COM1A1);

    // Select Fast PWM with TOP = ICR1
    set_bit(TCCR1A,WGM31);
    set_bit(TCCR1B,WGM33);
    set_bit(TCCR1B,WGM32);
    
    // Select clk(I/O) / 1 (No prescaling)
    set_bit(TCCR1B,CS10);

    // Select TOP = ICR1 = 0
    // Nothing since it's set to 0 by default

    // Set PD5 pin as output
    set_bit(DDRD, PD5);
}