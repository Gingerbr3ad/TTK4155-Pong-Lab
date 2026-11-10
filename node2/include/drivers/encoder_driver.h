#pragma once
#include <stdint.h>

void encoder_init(void);
/* Signed count relative to the position at encoder_init(). */
int32_t encoder_read(void);
