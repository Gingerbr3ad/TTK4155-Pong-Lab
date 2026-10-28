#include "application/oled/oled_utils.h"
#include "application/joystick.h"
#include "application/menu.h"

void menu_framebuffer_draw(const char *items[], uint8_t num_items, uint8_t selected_item) {
    oled_clear();
    for (uint8_t i = 0; i < num_items; i++) {
        if (i == selected_item) {
            // Draw the selected item with a different style
            oled_write_string(">", i, 0); // Arrow to indicate selection
            oled_write_string(items[i], i, 6); // Draw the item text
        } else {
            oled_write_string(" ", i, 0); // No arrow for unselected items
            oled_write_string(items[i], i, 6); // Draw the item text
        }
    }   
}

uint8_t menu_navigate(uint8_t num_items, const char *items[]) {
    uint8_t selected_item = 0;
    menu_framebuffer_draw(items, num_items, selected_item);
 
    while (1) {
        joystick_direction_t direction = joystick_get_direction();
        if (direction == JOYSTICK_UP) {
            if (selected_item > 0) {
                selected_item--;
                menu_framebuffer_draw(items, num_items, selected_item);
            }
            else if (selected_item == 0) {
                selected_item = num_items - 1; // Wrap around to the last item
                menu_framebuffer_draw(items, num_items, selected_item);
            }
        
        } else if (direction == JOYSTICK_DOWN) {
            if (selected_item < num_items - 1) {
                selected_item++;
                menu_framebuffer_draw(items, num_items, selected_item);
        }
            }
        
        }
        else if (joystick_button_pressed()) {
            return selected_item; // Return the index of the selected item
        }
        _delay_ms(100); // Debounce delay
    }
}
