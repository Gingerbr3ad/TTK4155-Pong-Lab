#include "system_definitions.h"
#include "drivers/spi_driver.h"
#include <util/delay.h>

#define OLED_DC 3

void oled_command_write(char command[], int command_len);
void oled_data_write(char data[], int data_len);
void oled_init();

void oled_test();