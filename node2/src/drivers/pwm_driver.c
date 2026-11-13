#include "sam.h"

#include "drivers/pwm_driver.h"

#define SERVO_CPRD 13125

void pwm_PB13_init() {
    // Enable peripheral clocks
    PMC->PMC_PCER0 = (1u << ID_PIOB);
    PMC->PMC_PCER1 = (1u << (ID_PWM - 32));

    // Hold PWM output LOW while initializing
    PIOB->PIO_CODR = PIO_PB12;
    PIOB->PIO_PUDR = PIO_PB12;
    PIOB->PIO_PER  = PIO_PB12;
    PIOB->PIO_OER  = PIO_PB12;

    // Disable channel before configuring it
    REG_PWM_DIS = PWM_DIS_CHID1;

    // Turn on peripheral B for pin PB13
    PIOB->PIO_ABSR |= PIO_ABSR_P13;
    // Disable GPIO control for pin PB13
    PIOB->PIO_PDR = PIO_PDR_P13;

    // Unlock the user interface
    PWM->PWM_WPCR = (PWM_WPCR_WPKEY(0x50574D)|PWM_WPCR_WPRG0|PWM_WPCR_WPRG1|PWM_WPCR_WPRG2|PWM_WPCR_WPRG3|PWM_WPCR_WPRG4|PWM_WPCR_WPRG5|PWM_WPCR_WPCMD(0));
    // Set channel 1 clock to mck/128 and active high
    REG_PWM_CMR1 = PWM_CMR_CPRE_MCK_DIV_128 | PWM_CMR_CPOL;
    // Set the frequency to 50Hz (50Hz = (128*13125)/84MHz)
    REG_PWM_CPRD1 = SERVO_CPRD;
    // Set the startign duty cycle to 50%
    REG_PWM_CDTY1 = (uint32_t)984;
    // Enable the PWM channel
    REG_PWM_ENA = PWM_ENA_CHID1;
}

void pwm_ch1_duty_update(int32_t duty_cycle, int32_t in_min, int32_t in_max, int32_t min_duty_permille, int32_t max_duty_permille) {
    int32_t min_duty = ((int64_t)SERVO_CPRD * min_duty_permille)/1000;
    int32_t max_duty = ((int64_t)SERVO_CPRD * max_duty_permille)/1000;

    if (min_duty < 0) {min_duty = 0;} 
    if (max_duty > SERVO_CPRD) {max_duty = SERVO_CPRD;}

    int64_t duty_cycle_update = (((int64_t)duty_cycle - in_min) * ((int64_t)max_duty - min_duty)/((int64_t)in_max - in_min) + min_duty);

    if (duty_cycle_update < min_duty) {duty_cycle_update = min_duty;} 
    if (duty_cycle_update > max_duty) {duty_cycle_update = max_duty;}
    
    REG_PWM_CDTYUPD1 = (uint32_t)duty_cycle_update;
}

void pwm_PB12_init() {
    PMC->PMC_PCER0 = (1u << ID_PIOB);
    PMC->PMC_PCER1 = 1u << (ID_PWM - 32);

    REG_PWM_DIS = PWM_DIS_CHID0;

    PIOB->PIO_ABSR |= PIO_PB12B_PWMH0;
    PIOB->PIO_PDR = PIO_PB12;
    PWM->PWM_WPCR = PWM_WPCR_WPKEY(0x50574d) | PWM_WPCR_WPRG0 | PWM_WPCR_WPRG1 | PWM_WPCR_WPRG2 | PWM_WPCR_WPRG3 | PWM_WPCR_WPRG4 | PWM_WPCR_WPRG5;

    REG_PWM_CMR0 = PWM_CMR_CPRE_MCK | PWM_CMR_CPOL;
    REG_PWM_CPRD0 = MOTOR_PERIOD;
    REG_PWM_CDTY0  = 0;

    REG_PWM_ENA = PWM_ENA_CHID0;
}

void pwm_ch0_duty_update(int32_t duty_cycle, int32_t in_min, int32_t in_max, int32_t min_duty_permille, int32_t max_duty_permille) {
   int32_t min_duty = ((int64_t)MOTOR_PERIOD * min_duty_permille)/1000;
    int32_t max_duty = ((int64_t)MOTOR_PERIOD * max_duty_permille)/1000;

    if (min_duty < 0) {min_duty = 0;} 
    if (max_duty > MOTOR_PERIOD) {max_duty = MOTOR_PERIOD;}

    int64_t duty_cycle_update = (((int64_t)duty_cycle - in_min) * ((int64_t)max_duty - min_duty)/((int64_t)in_max - in_min) + min_duty);

    if (duty_cycle_update < min_duty) {duty_cycle_update = min_duty;} 
    if (duty_cycle_update > max_duty) {duty_cycle_update = max_duty;}
    
    REG_PWM_CDTYUPD0 = (uint32_t)duty_cycle_update;
}