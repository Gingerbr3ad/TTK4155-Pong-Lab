#include "sam.h"

#include "system_tick_60Hz.h"

volatile uint8_t system_tick_60Hz_flag = 0;

void system_tick_60Hz_init() {
    // Timer enable
    PMC->PMC_PCER0 = 1u << ID_TC0;
    TC0->TC_WPMR = TC_WPMR_WPKEY(0x54494d);
    // Stop the timer for config
    REG_TC0_CCR0 = TC_CCR_CLKDIS;
    // Disable all default interrupts
    REG_TC0_IDR0 = 0xffffffff;
    // C_IO/8 Wave form Reset on compare
    REG_TC0_CMR0 = TC_CMR_TCCLKS_TIMER_CLOCK2 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
    // Set the timer frequency to 60 Hz
    REG_TC0_RC0 = SystemCoreClock / 8u / 60;
    // Enable interrupt on compare
    REG_TC0_IER0 = TC_IER_CPCS;
    NVIC_EnableIRQ(TC0_IRQn);
    //Start and reset the timer
    REG_TC0_CCR0 = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void TC0_Handler() {
    system_tick_60Hz_flag = 1;
    // Clear the interrupt
    REG_TC0_SR0;
}

uint8_t system_tick_60Hz_take() {
    uint8_t pending;
    __disable_irq();
    pending = system_tick_60Hz_flag;
    system_tick_60Hz_flag = 0;
    __enable_irq();

    return pending;
}