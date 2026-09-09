#include "system_definitions.h"

#include <avr/io.h>


void external_memory_init();

void xmem_write(uint8_t data, uint16_t addr);
uint8_t xmem_read (uint16_t addr);