#pragma once

void pwm_ch1_duty_update(int32_t duty_cycle, int32_t in_min, int32_t in_max, int32_t min_duty_percentile, int32_t max_duty_percentile);
void pwm_PB13_init();