#include "application/oled/oled_utils.h"

static uint8_t dirty_pages = 0xFF;

static void oled_command_write(uint8_t command[], int command_len) {
    clear_bit(PORTB, OLED_DC);
    spi_w(command, command_len, oled_ss);
}

static void oled_data_write(uint8_t data[], int data_len) {
    set_bit(PORTB, OLED_DC);
    spi_w(data, data_len, oled_ss);
}

static void oled_send_page(uint8_t page) {
    uint8_t page_command = 0xB0 + page;
    oled_command_write((uint8_t[]){page_command}, 1);

    oled_data_write(PAGEBUFFER, PAGEBUFFER_SIZE);
}

void oled_flush() {
    for (uint8_t page = 0; page < 8; ++page) {
        uint8_t mask = (uint8_t)(1u << page);

        if ((dirty_pages & mask) == 0) {
            continue;  // This page does not need uploading
        }

        uint16_t offset = (uint16_t)page * 128u;

        // Copy only this page into the staging buffer.
        memcpy(PAGEBUFFER, FRAMEBUFFER + offset, 128);

        // Clear this page's flag, preserving the other flags.
        dirty_pages &= (uint8_t)~mask;

        // Select the matching OLED page and send its 128 bytes.
        oled_send_page(page);
    }
}


void oled_init() {
    set_bit(DDRB, OLED_DC);

    oled_command_write((uint8_t[]){0x20, 0x02}, 2);       // Page addresing mode
    oled_command_write((uint8_t[]){0x00, 0x10}, 2);       // Set column 0 as the start address
    oled_command_write((uint8_t[]){0x21, 0x00, 0x7F}, 3); // Col 0-127
    oled_command_write((uint8_t[]){0x22, 0x00, 0x07}, 3); // Page 0-7
    oled_command_write((uint8_t[]){0xC8}, 1);             // Flip the display vertically
    oled_command_write((uint8_t[]){0xA4}, 1);             // Follow GDDRAM
    oled_command_write((uint8_t[]){0xAF}, 1);             // Display ON

    oled_checkerboard_test();
    _delay_ms(1000);

    framebuffer_clear();
}

void framebuffer_clear() {
    memset(FRAMEBUFFER, 0, FRAMEBUFFER_SIZE);
    dirty_pages = 0xFF;
}

// Checkerboard pattern independent of the SRAM
void oled_checkerboard_test() {
    uint8_t block[16];

    for (uint8_t i = 0; i < sizeof(block); i++) {
        block[i] = (i & 1) ? 0xAA : 0x55;
    }
    
    for (uint8_t i = 0; i < 64; i++) {
        oled_data_write(block, sizeof(block));
    }
}


// Those need to be updated to write to the framebuffer and not oled
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