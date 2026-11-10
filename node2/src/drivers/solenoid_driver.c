#include <stdint.h>

#include "sam.h"
#include "time.h"

#include "drivers/solenoid_driver.h"


void solenoid_init(){
    // Enable the peripheral clock for the PIOB
    PMC->PMC_PCER0 |= (1 << ID_PIOB);

    // Configure PB25 as output for solenoid control
    PIOB->PIO_PER = PIO_PB25;  // Enable control of PB25 by PIO
    PIOB->PIO_OER = PIO_PB25;  // Set PB25 as output
    PIOB->PIO_SODR = PIO_PB25; // Set PB25 HIGH (solenoid off)
};

void solenoid_set_state(uint8_t state){
    if (state == 1) {
        PIOB->PIO_CODR = PIO_PB25; // Set PB25 LOW (solenoid on)
    } else {
        PIOB->PIO_SODR = PIO_PB25; // Set PB25 HIGH (solenoid off)
    }
};

void solenoid_pulse(){
    solenoid_set_state(1);
    time_spinFor(msecs(100));
    solenoid_set_state(0);
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