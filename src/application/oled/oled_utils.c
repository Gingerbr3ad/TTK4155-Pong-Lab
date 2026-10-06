#include "application/oled/oled_utils.h"
#include <avr/pgmspace.h>
#include "application/oled/fonts.h"
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


void oled_write_char(char c) {
    if (c < 32 || c > 126) {
        return; // Ignore unsupported characters
    }
//font5 array is defined in fonts.h and contains the bitmap for each character from ASCII 32 to 126
    const unsigned char *char_bitmap = font5[c - 32];
    uint8_t char_data[5];
      for (int i = 0; i < 5; i++) {
 // Use pgm_read_byte since the font data is stored in flash memory and not in RAM
 //pgm_read_byte is taken from the avr library and it reads a byte from each character's bitmap    
        char_data[i] = pgm_read_byte(&char_bitmap[i]);
    }
    // Send the character data to the OLED display
    oled_data_write(char_data, 5);

    uint8_t space = 0x00;
    oled_data_write(&space, 1); // Add a space between characters
}


// Function to write a string to the OLED display
// It goes through each character in the string and calls oled_write_char function to write it to the display
void oled_write_string(const char *str) {
    while (*str!= '\0') {
        oled_write_char(*str++);
    }
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