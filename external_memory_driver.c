#include "external_memory_driver.h"

void external_memory_init() {
    set_bit(MCUCR, SRE);
    set_bit(SFIOR, XMM2);
}
