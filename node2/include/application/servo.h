#pragma once

#define ARM_SERVO_MIN_DUTY 45
#define ARM_SERVO_MAX_DUTY 100 // Slightly lowered from datasheet specs to reduce endpoint chatter
#define ARM_SERVO_MIN_INPUT -100
#define ARM_SERVO_MAX_INPUT 100

void arm_servo_init();
void arm_servo_update(int32_t position);
