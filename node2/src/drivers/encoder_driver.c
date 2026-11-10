#include "sam.h"
#include "drivers/encoder_driver.h"
#include <limits.h>

void encoder_init(void) {
    const uint32_t pins = PIO_PC25B_TIOA6 | PIO_PC26B_TIOB6;
    PMC->PMC_PCER0 = (1u << ID_PIOC) | (1u << ID_PIOA);
    /* TC2 channel 0 is peripheral TC6, not ID_TC2. */
    PMC->PMC_PCER1 = 1u << (ID_TC6 - 32);

    /* Arduino D4 also connects to PA29: keep that pad high impedance. */
    PIOA->PIO_PER = PIO_PA29;
    PIOA->PIO_ODR = PIO_PA29;
    PIOA->PIO_PUDR = PIO_PA29;
    PIOA->PIO_IDR = PIO_PA29;
    PIOC->PIO_IDR = pins;
    PIOC->PIO_ODR = pins;
    PIOC->PIO_PUER = pins;
    PIOC->PIO_ABSR |= pins;
    PIOC->PIO_PDR = pins;

    TC2->TC_WPMR = TC_WPMR_WPKEY(0x54494d);
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
    TC2->TC_CHANNEL[0].TC_IDR = 0xffffffffu;
    TC2->TC_QIDR = 0xffffffffu;
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA
                | TC_BMR_MAXFILT(8);
    /* Count both phases (four edges/cycle); no index-triggered reset. */
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0 | TC_CMR_ABETRG;
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int32_t encoder_read(void) {
    uint32_t count = TC2->TC_CHANNEL[0].TC_CV;
    if (count <= INT32_MAX) return (int32_t)count;
    return -1 - (int32_t)(UINT32_MAX - count);
}
