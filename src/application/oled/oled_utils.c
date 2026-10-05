#include "application/oled/oled_utils.h"

volatile uint8_t display_update_flag = 0;
volatile uint8_t framebuffer_updated_flag = 0;

void oled_command_write(uint8_t command[], int command_len) {
    clear_bit(PORTB, OLED_DC);
    spi_w(command, command_len, oled_ss);
}

void oled_data_write(uint8_t data[], int data_len) {
    set_bit(PORTB, OLED_DC);
    spi_w(data, data_len, oled_ss);
}

void oled_flush() {
    oled_data_write(FRAMEBUFFER, FRAMEBUFFER_SIZE);
    display_update_flag = 0;
}

void display_update_timer_init() {
    // Setup of 16 bit timer/counter 3 on pin PD4 (OC3A)
    // Select CTC mode with TOP = OCR3A
    set_bit(TCCR3B, WGM32);

    // Select clk(I/O) / 1024
    set_bit(TCCR3B, CS32);
    set_bit(TCCR3B, CS30);

    // Set timer frequency
    OCR3A = 199; // 24 Hz
    //OCR3A = 159; // 30 Hz


    // Enable Compare Match A interrupt
    set_bit(ETIMSK, OCIE3A);
}

// If the framebuffer was changed set a flag to send the frame buffer to the screen
ISR(TIMER3_COMPA_vect) {
    if (framebuffer_updated_flag) {
        display_update_flag = 1;
        framebuffer_updated_flag = 0;
    }
}

void oled_init() {
    set_bit(DDRB, OLED_DC);

    oled_command_write((uint8_t[]){0x20, 0x00}, 2);       // Horizontal mode
    oled_command_write((uint8_t[]){0x21, 0x00, 0x7F}, 3); // Col 0-127
    oled_command_write((uint8_t[]){0x22, 0x00, 0x07}, 3); // Page 0-7
    oled_command_write((uint8_t[]){0xA4}, 1); // Follow GDDRAM
    oled_command_write((uint8_t[]){0xAF}, 1); // Display ON

    oled_checkerboard_test();
    _delay_ms(1000);

    display_update_timer_init();
    oled_clear();
}

void oled_clear() {
    memset(FRAMEBUFFER, 0, FRAMEBUFFER_SIZE);
    framebuffer_updated_flag = 1;
}

void oled_checkerboard_test() {
    uint8_t block[16];

    for (uint8_t i = 0; i < sizeof(block); i++) {
        block[i] = (i & 1) ? 0xAA : 0x55;
    }
    
    for (uint8_t i = 0; i < 64; i++) {
        oled_data_write(block, sizeof(block));
    }
}