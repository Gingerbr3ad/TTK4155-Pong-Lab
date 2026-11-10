#include "sam.h"
#include "drivers/motor_driver.h"

#define MOTOR_PWM_HZ 20000u
static uint32_t period;
static int8_t direction;

static void disconnect_pwm(void) {
    /* Low PB12 means coast on the newer DRV8871 shield. */
    PIOB->PIO_CODR = PIO_PB12;
    PIOB->PIO_OER = PIO_PB12;
    PIOB->PIO_PER = PIO_PB12;
    __DSB();
}

void motor_stop(void) {
    disconnect_pwm();
    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = 0;
    direction = 0;
}

void motor_init(void) {
    PMC->PMC_PCER0 = (1u << ID_PIOB) | (1u << ID_PIOC);
    PMC->PMC_PCER1 = 1u << (ID_PWM - 32);
    disconnect_pwm();
    PIOC->PIO_CODR = PIO_PC23;
    PIOC->PIO_PER = PIO_PC23;
    PIOC->PIO_OER = PIO_PC23;
    PIOB->PIO_ABSR |= PIO_PB12B_PWMH0;

    PWM->PWM_WPCR = PWM_WPCR_WPKEY(0x50574d) | PWM_WPCR_WPRG0
        | PWM_WPCR_WPRG1 | PWM_WPCR_WPRG2 | PWM_WPCR_WPRG3
        | PWM_WPCR_WPRG4 | PWM_WPCR_WPRG5;
    PWM->PWM_DIS = PWM_DIS_CHID0;
    PWM->PWM_SCM &= ~PWM_SCM_SYNC0;
    period = SystemCoreClock / MOTOR_PWM_HZ;
    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_MCK | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[0].PWM_CPRD = period;
    PWM->PWM_CH_NUM[0].PWM_CDTY = 0;
    PWM->PWM_ENA = PWM_ENA_CHID0;
    direction = 0;
    /* PB12 stays low as GPIO. Servo channel 1 is left untouched. */
}

void motor_set(int16_t command) {
    if (command > 1000) command = 1000;
    if (command < -1000) command = -1000;
    if (!command) { motor_stop(); return; }

    int8_t next_direction = command > 0 ? 1 : -1;
    uint16_t magnitude = command > 0 ? command : -command;
    if (next_direction != direction) {
        /* Stop before changing DIR. The next control tick reconnects PWM,
         * after the new duty has latched, without a blocking delay. */
        disconnect_pwm();
        if ((next_direction > 0) == MOTOR_RIGHT_DIR_HIGH)
            PIOC->PIO_SODR = PIO_PC23;
        else
            PIOC->PIO_CODR = PIO_PC23;
        PWM->PWM_CH_NUM[0].PWM_CDTYUPD = period * magnitude / 1000u;
        direction = next_direction;
        return;
    }
    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = period * magnitude / 1000u;
    PIOB->PIO_PDR = PIO_PB12;
}
