#include "drivers/external_memory_driver.h"

void external_memory_init() {
    set_bit(MCUCR, SRE);
    set_bit(SFIOR, XMM2);
}

void xmem_write(uint8_t data, uint16_t addr, int16_t device_xmem_offset) {
    volatile char * ext_mem = (char *) device_xmem_offset;
    ext_mem[addr] = data;
}

uint8_t xmem_read (uint16_t addr, int16_t device_xmem_offset) {
    volatile char * ext_mem = (char *) device_xmem_offset;
    uint8_t ret_val = ext_mem[addr];
    return ret_val;
}
