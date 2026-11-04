#include "application/sram_test.h"

static void test_address_pair(uint8_t bit, uint16_t a, uint16_t b) {
    volatile uint8_t *p = (volatile uint8_t *)(uintptr_t)a;
    volatile uint8_t *q = (volatile uint8_t *)(uintptr_t)b;

    *p = 0x55;
    *q = 0xAA;

    uint8_t result_a = *p;
    uint8_t result_b = *q;

    printf("A%u: %04X=%02X, %04X=%02X %s\n",
           (unsigned)bit,
           (unsigned)a, (unsigned)result_a,
           (unsigned)b, (unsigned)result_b,
           (result_a == 0x55 && result_b == 0xAA)
               ? "OK" : "FAIL");
}

void SRAM_address_test(void) {
    for (uint8_t bit = 0; bit <= 10; bit++) {
        test_address_pair(
            bit,
            0x1800,
            (uint16_t)(0x1800u ^ (1u << bit))
        );
    }

    test_address_pair(11, 0x1400, 0x1C00);
}