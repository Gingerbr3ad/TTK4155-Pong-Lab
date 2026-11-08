#pragma once

#define ARM_SERVO_MIN_DUTY 45
#define ARM_SERVO_MAX_DUTY 105
#define ARM_SERVO_MIN_INPUT -100
#define ARM_SERVO_MAX_INPUT 100

void arm_servo_init();
void arm_servo_update(int32_t position);
