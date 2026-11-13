#include "application/oled/oled_utils.h"

static uint8_t dirty_pages = 0xFF;
static uint8_t cursor_y = 0;
static uint8_t cursor_x = 0;

static const oled_font_t fonts[3] = {
    {&font4[0][0], 4},
    {&font5[0][0], 5},
    {&font8[0][0], 8}
};

static void oled_command_write(uint8_t command[], int command_len) {
    clear_bit(PORTB, OLED_DC);
    spi_w_transmit(command, command_len, oled_ss);
}

static void oled_data_write(uint8_t data[], int data_len) {
    set_bit(PORTB, OLED_DC);
    spi_w_transmit(data, data_len, oled_ss);
}

static void oled_send_page(uint8_t page) {
    // Page addressing mode: select page and reset column to zero.
    uint8_t commands[] = {(uint8_t)(0xB0u | page), 0x00, 0x10};
    oled_command_write(commands, sizeof(commands));
    oled_data_write(PAGEBUFFER, PAGEBUFFER_SIZE);
}

void oled_invalidate_all() {
    dirty_pages = 0xFF;
}

void oled_flush() {
    for (uint8_t page = 0; page < 8; ++page) {
        uint8_t mask = (uint8_t)(1u << page);

        if ((dirty_pages & mask) == 0) {
            continue;  // This page does not need uploading
        }

        uint16_t offset = (uint16_t)page * 128u;

        // Copy only this page into the staging buffer.
        cli();
        memcpy(PAGEBUFFER, FRAMEBUFFER + offset, PAGEBUFFER_SIZE);

        // Clear this page's flag, preserving the other flags.
        dirty_pages &= (uint8_t)~mask;

        // Select the matching OLED page and send its 128 bytes.
        oled_send_page(page);
        sei();
    }
}

void framebuffer_clear() {
    memset(FRAMEBUFFER, 0, FRAMEBUFFER_SIZE);
    oled_invalidate_all();
}


void oled_init() {
    set_bit(DDRB, OLED_DC);

    oled_command_write((uint8_t[]){0x20, 0x02}, 2);       // Page addresing mode
    oled_command_write((uint8_t[]){0x00, 0x10}, 2);       // Set column 0 as the start address
    oled_command_write((uint8_t[]){0x21, 0x00, 0x7F}, 3); // Col 0-127
    oled_command_write((uint8_t[]){0x22, 0x00, 0x07}, 3); // Page 0-7
    oled_command_write((uint8_t[]){0xA1}, 1);             // Reverse horizontal mapping
    oled_command_write((uint8_t[]){0xC8}, 1);             // Reverse vertical mapping
    oled_command_write((uint8_t[]){0xA4}, 1);             // Follow GDDRAM
    oled_command_write((uint8_t[]){0xAF}, 1);             // Display ON

    /*
    oled_checkerboard_test();
    _delay_ms(1000);
    */

    framebuffer_clear();
}

void oled_checkerboard_test(void) {
    uint8_t block[16];

    // Switch to horizontal addressing mode
    oled_command_write((uint8_t[]){0x20, 0x00}, 2);

    // Set column range 0-127 and page range 0-7
    oled_command_write((uint8_t[]){0x21, 0x00, 0x7F}, 3);
    oled_command_write((uint8_t[]){0x22, 0x00, 0x07}, 3);

    // Generate checkerboard pattern
    for (uint8_t i = 0; i < sizeof(block); i++) {
        block[i] = (i & 1) ? 0xAA : 0x55;
    }

    // Send 1024 bytes to cover the entire display
    for (uint8_t i = 0; i < 64; i++) {
        oled_data_write(block, sizeof(block));
    }

    // Restore page addressing mode
    oled_command_write((uint8_t[]){0x20, 0x02}, 2);

    oled_invalidate_all();
}


static void oled_draw_column(uint8_t x, uint8_t y, uint8_t column, uint8_t height) {
    // Ensure that the function will not try to write outside the allocated framebuffer
    if (x >= 128 || y >= 64 || height == 0) {return;}
    if (height > 8) {height = 8;}
    if (height > 64 - y) {height = 64 - y;}

    uint8_t page = y / 8; // Calculate the starting page
    uint8_t shift = y % 8; // Calculate the starting bit on the starting page
    // Construct a pixel mask corresponding to the height of the column we want to draw (so height=5 -> pixel mask = 0000 0000 0001 1111)
    uint16_t pixel_mask = (uint16_t)((1u << height) - 1);
    // Shift the pixel mask so that it corresponds to the starting bit on the starting page
    uint16_t shifted_mask = (uint16_t)(pixel_mask << shift);
    // Apply the mask to the data we want to write
    uint16_t shifted_data = (uint16_t)((column & pixel_mask) << shift);
    // Find the framebuffer index for the starting page for the given y and x
    uint16_t index = (uint16_t)page * 128 + x;

    FRAMEBUFFER[index] = (FRAMEBUFFER[index] & ~shifted_mask); // Clear the pixel to be changed
    FRAMEBUFFER[index] = (FRAMEBUFFER[index] | shifted_data); // Write the data to framebuffer

    // Mark all of the modifed pages
    dirty_pages |= (uint8_t)(1 << page);
    if ((shifted_mask & 0xFF00) != 0 && page + 1 < 8) {
        index += 128;
        FRAMEBUFFER[index] = (uint8_t)(
            (FRAMEBUFFER[index] & (uint8_t)~(shifted_mask >> 8)) |
            (uint8_t)(shifted_data >> 8)
        );
        dirty_pages |= (uint8_t)(1 << (page + 1));
    }
}

void oled_goto_line(uint8_t line) {
    cursor_y = line;
}

void oled_goto_column(uint8_t col) {
    cursor_x = col;
}

void oled_goto_pos(uint8_t line, uint8_t col) {
    cursor_y = line;
    cursor_x = col;
}

void oled_goto_page(uint8_t page, uint8_t col) {
    cursor_x = col;
    cursor_y = page*8;
}

static void cursor_advance(uint8_t advance_cols) {
    uint16_t next_x = cursor_x + advance_cols;

    if (next_x >= 128) {
        cursor_x = 0;
        cursor_y = (cursor_y + 8);

        if (cursor_y >= 64) {
            cursor_y = 0;
        }
    } else {
        cursor_x = (uint8_t)next_x;
    }
}

static void oled_write_char(char c, oled_font_size_t size) {
    if (c < 32 || c > 126) {
        return; // Ignore unsupported characters
    }

    // Select the proper fornt size
    const oled_font_t *font = &fonts[size];
    uint8_t font_width = font->width;

    // Calculate the array offset for the start index of the character 'c' based on the font size to find the index of the 2d array as if it was 1d
    uint16_t char_offset = (c - 32) * font_width;

    // Iterate through the column amount = font size and draw each of the character columns
    for (uint8_t col = 0; col < font_width; ++col) {
        // Get the 'col' column of the character and draw it at the current possition
        uint8_t column_bitmap = pgm_read_byte(font->data + char_offset + col);
        oled_draw_column((uint8_t)(cursor_x + col), cursor_y, column_bitmap, 8);
    }

    // Clear at character spacing and advance the cursor
    oled_draw_column((cursor_x + font_width), cursor_y, 0, 8);
    cursor_advance(font_width + 1);
}


// Function to write a string to the OLED display
// It goes through each character in the string and calls oled_write_char function to write it to the display
void oled_write_string(char *str, oled_font_size_t size) {
    while (*str!= '\0') {
        oled_write_char(*str++, size);
    }
}
// Same as above but reads the input string from progmem
void oled_write_string_P(PGM_P str, oled_font_size_t size) {
    char c;
    while ((c = (char)pgm_read_byte(str++)) != '\0') {
        oled_write_char(c, size);
    }
}


// This function can draw any sprite saved as a bitmap at any pixel position
// It works by first spliting the sprite into bands 8 bits tall and using the oled_draw_column function to draw each of the column in a band
void oled_draw_sprite(const uint8_t *bitmap, uint8_t width, uint8_t height) {
    // Iterate through each 8 bit band of the sprite, the seven is added to ensure that the amount of bands is rounded up
    for (uint8_t band = 0; band < (height + 7) / 8; ++band) {
        uint16_t dest_y = cursor_y + band * 8;
        // Break if the next sprite band would start outside the display
        if (dest_y >= 64) {
            break;
        }

        // Calculate the height of the column to draw
        // If the remaining amount of rows is <8, use it, else the height is the maximum of 8 bits
        uint16_t rows_left = height - band * 8;
        uint16_t strip_height = rows_left;
        if (rows_left >= 8u) {strip_height = 8;}

        // Find the offset of the current band in the sprite bitmap index
        uint16_t band_offset = band * width;

        // Iterate through the column amount = sprite width and draw each of the sprite band columns
        for (uint16_t col = 0; col < width; ++col) {
            uint16_t dest_x = cursor_x + col;
            // Break if the next sprite column would start outside the display
            if (dest_x >= 128) {
                break;
            }

            // Find the index of the column to draw in the sprite bitmap
            const uint8_t *source_index = bitmap + band_offset + col;
            uint8_t column = pgm_read_byte(source_index);
            
            oled_draw_column(dest_x, dest_y, column, strip_height);
        }
    }
}

void oled_draw_pixel(uint8_t x, uint8_t y, uint8_t on) {
    if (on) {oled_draw_column(x, y, 1, 1);} // Draw
    else {oled_draw_column(x, y, 0, 1);} // Clear
}
