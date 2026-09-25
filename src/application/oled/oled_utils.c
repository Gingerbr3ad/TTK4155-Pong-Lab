#include "application/oled/oled_utils.h"

void oled_command_write(uint8_t command[], int command_len) {
    clear_bit(PORTB, OLED_DC);
    spi_w(command, command_len, oled_ss);
}

void oled_data_write(uint8_t data[], int data_len) {
    set_bit(PORTB, OLED_DC);
    spi_w(data, data_len, oled_ss);
}

void oled_init() {
    set_bit(DDRB, OLED_DC);

    uint8_t DISP_ON_COMMAND[1] = {0xAF};

    oled_command_write(DISP_ON_COMMAND, sizeof(DISP_ON_COMMAND));
}

void oled_test() {
    uint8_t ENTIRLE_DISP_ON_COMMAND[1] = {0xA5};
    uint8_t ENTIRLE_DISP_ON_REVERT_COMMAND[1] = {0xA4};

    oled_command_write(ENTIRLE_DISP_ON_COMMAND, sizeof(ENTIRLE_DISP_ON_COMMAND));
    _delay_ms(1000);
    oled_command_write(ENTIRLE_DISP_ON_REVERT_COMMAND, sizeof(ENTIRLE_DISP_ON_REVERT_COMMAND));
}