#include <stdint.h>

#include "sam.h"
#include "time.h"

#include "drivers/solenoid_driver.h"

#define SOLENOID_PULSE_TICKS_GOAL 7

static uint8_t solenoid_pulse_flag;
static uint8_t solenoid_pulse_ticks;

void solenoid_init() {
    // Enable the peripheral clock for the PIOB
    PMC->PMC_PCER0 |= (1 << ID_PIOB);

    // Configure PB25 as output for solenoid control
    PIOB->PIO_PER = PIO_PB25;  // Enable control of PB25 by PIO
    PIOB->PIO_OER = PIO_PB25;  // Set PB25 as output
    PIOB->PIO_SODR = PIO_PB25; // Set PB25 HIGH (solenoid off)
};

void solenoid_set_state(uint8_t state) {
    if (state == 1) {
        PIOB->PIO_CODR = PIO_PB25; // Set PB25 LOW (solenoid on)
    } else {
        PIOB->PIO_SODR = PIO_PB25; // Set PB25 HIGH (solenoid off)
    }
};

void solenoid_pulse_start() {
    solenoid_set_state(1);
    solenoid_pulse_ticks = 0;
    solenoid_pulse_flag = 1;
};

void solenoid_pulse_tick() {
    if ((solenoid_pulse_ticks++ >= SOLENOID_PULSE_TICKS_GOAL) && solenoid_pulse_flag) {
        solenoid_set_state(0);
        solenoid_pulse_flag = 0;
    }
}
