#include "sam.h"
#include "application/position_controller.h"
#include "drivers/encoder_driver.h"
#include "drivers/motor_driver.h"

static volatile int32_t target;
static volatile uint8_t enabled;
static float integral;

static float clamp_output(float value) {
    if (value > POSITION_OUTPUT_LIMIT) return POSITION_OUTPUT_LIMIT;
    if (value < -POSITION_OUTPUT_LIMIT) return -POSITION_OUTPUT_LIMIT;
    return value;
}

void position_controller_init() {
    motor_init();
    encoder_init();
    enabled = 0;
    integral = 0;
    target = (POSITION_MAX_LEFT + POSITION_MAX_RIGHT) / 2;
    PMC->PMC_PCER0 = 1u << ID_TC0;
    TC0->TC_WPMR = TC_WPMR_WPKEY(0x54494d);
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
    TC0->TC_CHANNEL[0].TC_IDR = 0xffffffffu;
    TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK2
        | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
    TC0->TC_CHANNEL[0].TC_RC = SystemCoreClock / 8u / POSITION_CONTROL_HZ;
    (void)TC0->TC_CHANNEL[0].TC_SR;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    NVIC_SetPriority(TC0_IRQn, 0);
    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
    NVIC_EnableIRQ(TC0_IRQn);
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void position_controller_set_joystick(int8_t x) {
    if (x < -100 || x > 100) return;
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    target = POSITION_MAX_LEFT + ((int32_t)x + 100)
        * (POSITION_MAX_RIGHT - POSITION_MAX_LEFT) / 200;
    enabled = 1;
    __set_PRIMASK(state);
}

void position_controller_stop() {
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    enabled = 0;
    integral = 0;
    motor_stop();
    __set_PRIMASK(state);
}

void TC0_Handler() {
    if (!(TC0->TC_CHANNEL[0].TC_SR & TC_SR_CPCS) || !enabled) return;
    int32_t position = encoder_read();
    float error = (float)position - (float)target;
    if (error >= -POSITION_TOLERANCE && error <= POSITION_TOLERANCE) {
        integral = 0;
        motor_stop();
        return;
    }
    float p = POSITION_KP * error;
    float candidate = clamp_output(integral + POSITION_KI * error / POSITION_CONTROL_HZ);
    float proposed = p + candidate;
    if ((proposed <= POSITION_OUTPUT_LIMIT || error < 0) &&
        (proposed >= -POSITION_OUTPUT_LIMIT || error > 0)) integral = candidate;
    float output = clamp_output(p + integral);
    if ((position <= POSITION_MAX_RIGHT && output > 0) ||
        (position >= POSITION_MAX_LEFT && output < 0)) {
        integral = 0;
        output = 0;
    }
    motor_set((int16_t)output);
}
