#include "application/FSM/paint.h"
#include "application/FSM/fsm.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"

#define PAINT_SIZE_MASK 0x3F
#define PAINT_PENCIL (1u << 6)
#define PAINT_ERASER (1u << 7)
#define CANVAS_LEFT 16
#define CANVAS_BOTTOM 51

// Bits 0-5 sizes Bit 6 pencil Bit 7 eraser
static uint8_t paint_config;

// Draw the selectable controls
static void paint_draw_controls() {
    // Brush sizes on the left
    for (uint8_t i = 0; i < 6; ++i) {
        char label[] = {' ', (char)('1' + i), '\0'};

        if (paint_config & (1u << i)) {
            label[0] = '>';
        }

        oled_goto_pos(i * 10, 0);
        oled_write_string(label, OLED_FONT_SMALL);
    }

    // Draw/Earase on the bottom
    oled_goto_pos(55, CANVAS_LEFT);
    if (paint_config & PAINT_PENCIL) {
        oled_write_string_P(PSTR(">Pencil"), OLED_FONT_SMALL);
    } else {
        oled_write_string_P(PSTR(" Pencil"), OLED_FONT_SMALL);
    }

    oled_goto_pos(55, 73);
    if (paint_config & PAINT_ERASER) {
        oled_write_string_P(PSTR(">Eraser"), OLED_FONT_SMALL);
    } else {
        oled_write_string_P(PSTR(" Eraser"), OLED_FONT_SMALL);
    }
}

static void paint_draw_brush(touch_position_t touch) {
    // Find the diameter
    uint8_t size = 1;
    if (paint_config & (1u << 0)) {size = 1;}
    if (paint_config & (1u << 1)) {size = 2;}
    if (paint_config & (1u << 2)) {size = 3;}
    if (paint_config & (1u << 3)) {size = 4;}
    if (paint_config & (1u << 4)) {size = 5;}
    if (paint_config & (1u << 5)) {size = 6;}

    // Map touch coordinates onto the canvas.
    int16_t x = CANVAS_LEFT + ((int16_t)touch.x + 100)*(127 - CANVAS_LEFT)/200;
    int16_t y = (100 - (int16_t)touch.y)*CANVAS_BOTTOM/200;

    // Draw/Erase all the pixels in the square
    for (uint8_t row = 0; row < size; ++row) {
        for (uint8_t col = 0; col < size; ++col) {
            int16_t px = x + col - size / 2;
            int16_t py = y + row - size / 2;

            // Keep drawing and erasing inside the canvas.
            if (px < CANVAS_LEFT || px > 127 ||
                py < 0 || py > CANVAS_BOTTOM) {
                continue;
            }

            oled_draw_pixel(px, py, paint_config & PAINT_PENCIL);
        }
    }
}

void paint_enter() {
    paint_config = PAINT_PENCIL | (1u << 0);
    framebuffer_clear();
    paint_draw_controls();
}

void paint_system_tick() {
    if (input_button_pressed(INPUT_BUTTON_L6) || input_button_pressed(INPUT_BUTTON_L7)) {fsm_handle_event(FSM_EVENT_MENU_ENTER); return;}

    joystick_direction_t direction = input_joystick_direction_edge();
    uint8_t previous_config = paint_config;
    uint8_t size = paint_config & PAINT_SIZE_MASK;

    if (direction == JOYSTICK_UP ||
        input_button_pressed(INPUT_BUTTON_NAV_UP)) {
        if (size > 1) {
            size >>= 1;
        }
    } else if (direction == JOYSTICK_DOWN ||
               input_button_pressed(INPUT_BUTTON_NAV_DOWN)) {
        if (size < (1u << 5)) {
            size <<= 1;
        }
    }

    // Update the size while keeping the selected tool.
    paint_config = (paint_config & (PAINT_PENCIL | PAINT_ERASER)) | size;

    if (direction == JOYSTICK_LEFT ||
        input_button_pressed(INPUT_BUTTON_NAV_LEFT)) {
        paint_config = size | PAINT_PENCIL;
    } else if (direction == JOYSTICK_RIGHT ||
               input_button_pressed(INPUT_BUTTON_NAV_RIGHT)) {
        paint_config = size | PAINT_ERASER;
    }

    if (paint_config != previous_config) {
        paint_draw_controls();
    }

    paint_draw_brush(input_touch_position_read());
}