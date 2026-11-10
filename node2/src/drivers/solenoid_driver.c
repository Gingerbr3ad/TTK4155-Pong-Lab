#include "drivers/solenoid_driver.h"
#include "utils/sam/sam3x/include/sam.h"
#include <stdint.h>

void solenoid_init(){
    // Enable the peripheral clock for the PIOA
    PMC->PMC_PCER0 |= (1 << ID_PIOA);

    // Configure PA10 as output for solenoid control
    PIOA->PIO_PER = PIO_PA10;  // Enable control of PA10 by PIO
    PIOA->PIO_OER = PIO_PA10;  // Set PA10 as output
    PIOA->PIO_CODR = PIO_PA10; // Set PA10 low (solenoid off)
};
void solenoid_pulse(uint16_t duration_ms){
    PIOA->PIO_SODR = PIO_PA10; // Set PA10 high (solenoid on)

    delay_ms(duration_ms); // Wait for the specified duration
    // Deactivate the solenoid
    PIOA->PIO_CODR = PIO_PA10; // Set PA10 low (solenoid off)
};

void solenoid_set_state(){
    // Toggle the state of the solenoid
    if (PIOA->PIO_ODSR & PIO_PA10) {
        PIOA->PIO_CODR = PIO_PA10; // Set PA10 low (solenoid off)
    } else {
        PIOA->PIO_SODR = PIO_PA10; // Set PA10 high (solenoid on)
    }
};


//Currently not used/finished, but i was thinking of using a timer interrupt to handle solenoid pulse
/*
void timer_interupt(uint16_t duration_ms){
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // Enable and start the timer
    TC0->TC_CHANNEL[0].TC_CMR = 
    TC_CMR_TCCLKS_TIMER_CLOCK4 |
    TC_CMR_WAVE | 
    TC_CMR_WAVSEL_UP_RC; // Set up the timer in waveform mode

    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS; // Enable interrupt on RC compare
    TC0->TC_CHANNEL[0].TC_RC = duration_ms; // Set the compare value
};*/