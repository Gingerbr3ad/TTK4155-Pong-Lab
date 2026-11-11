#include <stdlib.h>

#include "sam.h"

#include "application/motor.h"
#include "drivers/pwm_driver.h"


static int8_t direction;
static int8_t prev_direction;

void motor_init() {
    PMC->PMC_PCER0 = (1u << ID_PIOC);

    PIOC->PIO_CODR = PIO_PC23;
    PIOC->PIO_PER = PIO_PC23;
    PIOC->PIO_OER = PIO_PC23;

    direction = 0;
}

// Set the motor effort in permilles 
void motor_set(int16_t command) {
    if (command > 1000) {command = 1000;}
    if (command < -1000) {command = -1000;}

    // Get the direction an magnitude from the commanded effort
    if (command < 0) {direction = 0;}
    else {direction = 1;}
    uint32_t magnitude = abs(command);

    // Set the duty cycle
    pwm_ch0_duty_update(magnitude, 0, 1000, 0, 1000);

    // If the direction was changed set the dir pin to the corresponing state
    if (prev_direction != direction) {
        if (direction = 1) {PIOC->PIO_SODR = PIO_PC23;} 
        else {PIOC->PIO_CODR = PIO_PC23;}
        prev_direction = direction;
    }
}
