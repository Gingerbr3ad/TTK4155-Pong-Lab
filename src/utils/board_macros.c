#include "utils/board_macros.h"
#include "drivers/spi_driver.h"

const spi_ss_pin_t oled_ss = OLED_SS;
const spi_ss_pin_t slaves[1] = {OLED_SS};