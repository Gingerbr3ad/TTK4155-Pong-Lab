#include "sam.h"

#include "application/servo.h"
#include "drivers/pwm_driver.h"

void arm_servo_init() {
    pwm_PB13_init();
}

void arm_servo_update(int32_t position) {
    pwm_ch1_duty_update(position, ARM_SERVO_MIN_INPUT, ARM_SERVO_MAX_INPUT, ARM_SERVO_MIN_DUTY, ARM_SERVO_MAX_DUTY);
}