#pragma once

#define MOTOR_PWM_HZ 20000u
#define MOTOR_PERIOD (uint32_t)SystemCoreClock/MOTOR_PWM_HZ

void pwm_ch1_duty_update(int32_t duty_cycle, int32_t in_min, int32_t in_max, int32_t min_duty_permille, int32_t max_duty_permille);
void pwm_PB13_init();

void pwm_ch0_duty_update(int32_t duty_cycle, int32_t in_min, int32_t in_max, int32_t min_duty_permille, int32_t max_duty_permille);
void pwm_PB12_init();