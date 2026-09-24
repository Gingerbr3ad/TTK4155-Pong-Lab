#include "application/oled/oled_utils.h"
static const spi_ss_pin OLED_SS = {&DDRB, &PORTB, PB3};

void oled_command_write(char command[], int command_len) {
    clear_bit(PORTB, OLED_DC);
    spi_w(command, command_len, OLED_SS);
}

void oled_data_write(char data[], int data_len) {
    set_bit(PORTB, OLED_DC);
    spi_w(data, data_len, OLED_SS);
}

void oled_init() {
    set_bit(DDRB, OLED_DC);

    spi_ss_pin slaves[1] = {OLED_SS};
    spi_init(slaves, 1); //For testign only, should be moved to main later on

    char DISP_ON_COMMAND[1] = {0xAF};
    char ENTIRLE_DISP_ON_COMMAND[1] = {0xA5};

    oled_command_write(DISP_ON_COMMAND, sizeof(DISP_ON_COMMAND));
    oled_command_write(ENTIRLE_DISP_ON_COMMAND, sizeof(ENTIRLE_DISP_ON_COMMAND));
}