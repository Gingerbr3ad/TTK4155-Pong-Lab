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

    oled_command_write((uint8_t[]){0x20, 0x00}, 2);       // Horizontal mode
    oled_command_write((uint8_t[]){0x21, 0x00, 0x7F}, 3); // Col 0-127
    oled_command_write((uint8_t[]){0x22, 0x00, 0x07}, 3); // Page 0-7

    oled_command_write((uint8_t[]){0xA4}, 1); // Follow GDDRAM

    oled_command_write(DISP_ON_COMMAND, sizeof(DISP_ON_COMMAND));
}

void oled_test() {
    uint8_t ENTIRLE_DISP_ON_COMMAND[1] = {0xA5};
    uint8_t ENTIRLE_DISP_ON_REVERT_COMMAND[1] = {0xA4};

    oled_command_write(ENTIRLE_DISP_ON_COMMAND, sizeof(ENTIRLE_DISP_ON_COMMAND));
    _delay_ms(1000);
    oled_command_write(ENTIRLE_DISP_ON_REVERT_COMMAND, sizeof(ENTIRLE_DISP_ON_REVERT_COMMAND));
}

static void oled_set_full_window(void)
{
    oled_command_write((uint8_t[]){0x20, 0x00}, 2);       // horizontal addressing
    oled_command_write((uint8_t[]){0x21, 0x00, 0x7F}, 3); // columns 0..127
    oled_command_write((uint8_t[]){0x22, 0x00, 0x07}, 3); // pages 0..7
}

void oled_clear(void)
{
    uint8_t block[16] = {0};

    oled_set_full_window();

    // 64 * 16 = 1024 bytes
    for (uint8_t i = 0; i < 64; i++) {
        oled_data_write(block, sizeof(block));
    }
}

void oled_checker(void)
{
    uint8_t block[16];

    for (uint8_t i = 0; i < sizeof(block); i++) {
        block[i] = (i & 1) ? 0xAA : 0x55;
    }

    oled_set_full_window();

    // 64 * 16 = 1024 bytes
    for (uint8_t i = 0; i < 64; i++) {
        oled_data_write(block, sizeof(block));
    }
}