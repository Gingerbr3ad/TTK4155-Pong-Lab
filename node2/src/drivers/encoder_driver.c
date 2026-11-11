#include "sam.h"
#include "drivers/encoder_driver.h"

#define PINS PIO_PC25B_TIOA6 | PIO_PC26B_TIOB6

volatile int16_t POSITION_MAX_LEFT = 50;
volatile int16_t POSITION_MAX_RIGHT = 50;
volatile int16_t POSITION_CENTER = 0;

void encoder_init() {
    PMC->PMC_PCER0 = (1u << ID_PIOC) | (1u << ID_PIOA);
    PMC->PMC_PCER1 = 1u << (ID_TC6 - 32);

    // Disable the PA29 pin to avoid interferance from it
    PIOA->PIO_PER = PIO_PA29;
    PIOA->PIO_ODR = PIO_PA29;
    PIOA->PIO_PUDR = PIO_PA29;
    PIOA->PIO_IDR = PIO_PA29;

    // Enable the QDEC pins
    PIOC->PIO_IDR = PINS;
    PIOC->PIO_ODR = PINS;
    PIOC->PIO_PUER = PINS;
    PIOC->PIO_ABSR |= PINS;
    PIOC->PIO_PDR = PINS;

    // Configure the time
    TC2->TC_WPMR = TC_WPMR_WPKEY(0x54494d);
    REG_TC2_CCR0 = TC_CCR_CLKDIS;
    REG_TC2_IDR0 = 0xffffffffu;
    // Also disable all of the QDEC specific interrupts
    REG_TC2_QIDR = 0xffffffffu;
    // Enable quadrature decoding Enable postion measurment 4x quad Filter out ~100ns pulses
    REG_TC2_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA | TC_BMR_MAXFILT(8);
    // Select external timer (required from datasheet)
    REG_TC2_CMR0 = TC_CMR_TCCLKS_XC0;
    // Start and reset the counter
    REG_TC2_CCR0 = TC_CCR_CLKEN | TC_CCR_SWTRG;

    // Initialize the max travelt to very small values
    POSITION_MAX_LEFT = 50;
    POSITION_MAX_RIGHT = 50;
    POSITION_CENTER = 0;
}

int32_t encoder_read() {
    uint32_t count = REG_TC2_CV0;

    // Funky math to convert the quad CV value into a signed integer
    if (count <= INT32_MAX) return (int32_t)count;
    return -1 - (int32_t)(UINT32_MAX - count);
}
