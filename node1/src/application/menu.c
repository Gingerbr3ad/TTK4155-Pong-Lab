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
    uint8_t joystick_moved = 0; // Flag to track if the joystick has moved
    menu_framebuffer_draw(items, num_items, selected_item);
 
    while (1) {
        joystick_direction_t direction = joystick_get_direction();
        if (direction == JOYSTICK_UP && joystick_moved == 0) {
            if (selected_item > 0) {
                selected_item--;
                menu_framebuffer_draw(items, num_items, selected_item);
            }//check if the selected item is the first item in the menu, if it is then the marker goes to the last item
            else if (selected_item == 0) {
                selected_item = num_items - 1; // Wrap around to the last item
                menu_framebuffer_draw(items, num_items, selected_item);
            }
        joystick_moved = 1; // Set the flag to indicate that the joystick has moved
        } 
        else if (direction == JOYSTICK_DOWN && joystick_moved == 0) {
            if (selected_item < num_items - 1) {
                selected_item++;
                menu_framebuffer_draw(items, num_items, selected_item);
            }
            else if (selected_item == num_items - 1) {
                selected_item = 0; // Wrap around to the first item
                menu_framebuffer_draw(items, num_items, selected_item);
            }
        joystick_moved = 1; // Set the flag to indicate that the joystick has moved
        } 
        else if (direction == JOYSTICK_NEUTRAL) {
            joystick_moved = 0; // Reset the flag when the joystick returns to neutral
        }
        if (joystick_button_pressed()) {
            return selected_item; // Return the index of the selected item
        }
        _delay_ms(50); // Debounce delay
    }
}

void home_menu(void){
    uint8_t num_items = 2;
    const char *items[] = {"START GAME", "SETTINGS"};
    uint8_t selected_item = menu_navigate(num_items, items);
    if (selected_item == 0) {
        game_menu();
    } else if (selected_item == 1) {
        settings_menu();
    }

}

void game_menu(uint8_t score){
     char score_text[3];

    if (score < 10) {
        score_text[0] = '0' + score;
        score_text[1] = '\0';
    }
    else { //Manual way of representing 10 since that is two characteres
        score_text[0] = '1';
        score_text[1] = '0';
        score_text[2] = '\0';
    }
    oled_clear();

    oled_write_string("Score", 0, 20);
    oled_write_string(score_text, 2, 20);
    oled_write_string("TO RETURN", 6, 0);
    if(score>=10){
        oled_clear();
        oled_write_string("YOU LOST", 4, 50);
        oled_write_string("TO RETURN", 6, 0);
    }
    while(1){
        if(joystick_button_pressed()){
           return; // Return to the previous menu or exit the game menu
        }
    }
}

void settings_menu(void){
    uint8_t num_items = 2;
    const char *items[] = {"DIFFICULTY", "TO RETURN"};
    oled_clear();
    uint8_t selected_item = menu_navigate(num_items, items);
    if(selected_item==0){

    }
    else if(selected_item==1){
        return; // Return to the previous menu
    }
    
}

