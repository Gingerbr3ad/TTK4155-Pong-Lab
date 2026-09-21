#include <util/delay.h>

#include "drivers/adc_driver.h"
#include "drivers/external_memory_driver.h"

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
    set_bit(ICR1L, 0);

    // Set PD5 pin as output
    set_bit(DDRD, PD5);
}

void adc_write_strobe() {xmem_write(ADC_RW_ADDRESS_DUMMY, ADC_RW_ADDRESS_DUMMY, ADC_XMEM_OFFSET); }
int adc_read_strobe() {return xmem_read(ADC_RW_ADDRESS_DUMMY, ADC_XMEM_OFFSET); }

int adc_read(int channel) {
    // Initiate a read sequence
    adc_write_strobe();

    // Wait for specified conversion time (maybe upgrade to interrupt later?)
    _delay_us(3.6);

    // Read and return data from the requested channel
    int adc_data = 0;
    for (int i = 0; i < (channel+1); i++) {
        adc_data = adc_read_strobe();
    }
    return adc_data;
}
