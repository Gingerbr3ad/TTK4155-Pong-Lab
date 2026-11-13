#include "sam.h"

#include "drivers/encoder_driver.h"

#include "application/position_controller.h"
#include "application/motor.h"

volatile float KP;
volatile float KI; 

static volatile int32_t target;
static volatile uint8_t enabled;
static float integral;


static void position_controller_timer_init() {
    PMC->PMC_PCER0 = 1u << ID_TC3;

    TC1->TC_WPMR = TC_WPMR_WPKEY(0x54494d);
    REG_TC1_CCR0 = TC_CCR_CLKDIS;
    REG_TC1_IDR0 = 0xffffffff;
    REG_TC1_CMR0 = TC_CMR_TCCLKS_TIMER_CLOCK2 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
    REG_TC1_RC0 = SystemCoreClock / 8u / POSITION_CONTROL_LOOP_HZ;

    NVIC_SetPriority(TC3_IRQn, 0);
    REG_TC1_IER0 = TC_IER_CPCS;
    NVIC_EnableIRQ(TC3_IRQn);

    REG_TC1_CCR0 = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void position_controller_init() {
    KP = 0.2;
    KI = 0.3;

    enabled = 0;
    integral = 0;
    target = 0;

    position_controller_timer_init();
}

void position_controller_map_target(int32_t input, int32_t in_min, int32_t in_max, int32_t min_position, int32_t max_position) {
    if (input < in_min) {input = in_min;} 
    if (input > in_max) {input = in_max;}

    int64_t mapped_target= (((int64_t)input - in_min) * ((int64_t)max_position - min_position)/((int64_t)in_max - in_min) + min_position);

    if (mapped_target < min_position) {mapped_target = min_position;} 
    if (mapped_target > max_position) {mapped_target = max_position;}
    
    __disable_irq();
    target = (int32_t)mapped_target;
    __enable_irq();
}

void position_controller_stop() {
    __disable_irq();
    motor_set(0);
    enabled = 0;
    integral = 0;
    __enable_irq();
}

void position_controller_start() {
    __disable_irq();
    enabled = 1;
    integral = 0;
    target = POSITION_CENTER;
    __enable_irq();
}

// Calculate the controller effort on interrup to keep it as time accurate as possible
void TC3_Handler() {
    if (!enabled) {
        REG_TC1_SR0;
        return;
    }

    // Get the error
    int32_t position = encoder_read();
    float error = (float)position - (float)target;

    // Small deadband to avoid tiny oscilation around the setpoint
    if (error >= -POSITION_TOLERANCE && error <= POSITION_TOLERANCE) {
        integral = 0;
        motor_set(0);
        REG_TC1_SR0;
        return;
    }

    // Calculate the integral effort
    float i = (KI*error)/POSITION_CONTROL_LOOP_HZ;
    integral += i;
    int32_t output = KP*error + integral;

    // Apply saturation and anti windup
    if (output > MOTOR_EFFORT_LIMIT) {
        output = MOTOR_EFFORT_LIMIT;
        integral -= i;
    }
    if (output < -MOTOR_EFFORT_LIMIT) {
        output =  -MOTOR_EFFORT_LIMIT;
        integral -= i;
    }

    // Send the regulator effort to the motor
    motor_set((int16_t)output);

    // Clear the interrupt
    REG_TC1_SR0;
}
