#include "application/FSM/menu.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"

/* TODO: This is shit and should be refactored if there is time, handling the menu and sub menus should be more modular and adding more options should be a lot easier, maybe make it heuristic? */

// Menu pages (main menu + sub menus)
typedef enum {
    MENU_MAIN,
    MENU_OPTIONS,
    MENU_ABOUT
} menu_page_t;

static menu_page_t menu_page;
static uint8_t selection;

// Returns the number of option for the menu page
static uint8_t menu_item_count() {
    switch (menu_page) {
        case MENU_MAIN:    return 2;
        case MENU_OPTIONS: return 2;
        case MENU_ABOUT:   return 0;
    }
    return 0;
}

// Draws the current menu page
static void menu_draw() {
    oled_goto_pos(0, 4);

    switch (menu_page) {
        case MENU_MAIN:
            oled_write_string_P(PSTR("PONG MENU"), OLED_FONT_LARGE);
            break;

        case MENU_OPTIONS:
            oled_write_string_P(PSTR("OPTIONS"), OLED_FONT_LARGE);
            break;

        case MENU_ABOUT:
            oled_write_string_P(PSTR("ABOUT"), OLED_FONT_LARGE);
            oled_goto_pos(24, 6);
            oled_write_string_P(PSTR("TTK4155 Pong"), OLED_FONT_NORMAL);
            oled_goto_pos(44, 6);
            oled_write_string_P(PSTR("BACK to options"), OLED_FONT_SMALL);
            return;
    }

    // Draws the menu page options along with the selection cursor '>'
    for (uint8_t i = 0; i < menu_item_count(); ++i) {
        uint8_t y = (uint8_t)(24 + i * 16);

        oled_goto_pos(y, 4);
        if (i == selection) {
            oled_write_string_P(PSTR(">"), OLED_FONT_NORMAL);
        } else {
            oled_write_string_P(PSTR(" "), OLED_FONT_NORMAL);
        }
        oled_goto_pos(y, 16);

        if (menu_page == MENU_MAIN) {
            if (i == 0) {
                oled_write_string_P(PSTR("Start game"), OLED_FONT_NORMAL);
            } else {
                oled_write_string_P(PSTR("Options"), OLED_FONT_NORMAL);
            }
        } else { // MENU_OPTIONS
            if (i == 0) {
                oled_write_string_P(PSTR("About"), OLED_FONT_NORMAL);
            } else {
                oled_write_string_P(PSTR("Back"), OLED_FONT_NORMAL);
            }
        }
    }
}

// Opens a menu page and resets the selection
static void menu_open(menu_page_t next) {
    framebuffer_clear();
    menu_page = next;
    selection = 0;
    menu_draw();
}

// Moves the selection up if possible
static void menu_move_up() {
    if (selection > 0) {
        --selection;
        menu_draw();
    }
}

// Moves the selection down if possible
static void menu_move_down() {
    if ((uint8_t)(selection + 1) < menu_item_count()) {
        ++selection;
        menu_draw();
    }
}

// Handles selecting the highlighted option
static menu_action_t menu_select() {
    switch (menu_page) {
        case MENU_MAIN:
            if (selection == 0) {
                return MENU_ACTION_START_GAME;
            } else {
                menu_open(MENU_OPTIONS);
            }
            break;

        case MENU_OPTIONS:
            if (selection == 0) {
                menu_open(MENU_ABOUT);
            } else {
                menu_open(MENU_MAIN);
            }
            break;

        case MENU_ABOUT:
            break;
    }

    return MENU_ACTION_NONE;
}

// Returns to the parent menu
static void menu_go_back() {
    switch (menu_page) {
        case MENU_ABOUT:
            menu_open(MENU_OPTIONS);
            break;

        case MENU_OPTIONS:
            menu_open(MENU_MAIN);
            break;

        case MENU_MAIN:
            break;
    }
}

// Initializes the menu state
void menu_enter() {
    menu_open(MENU_MAIN);
}

// Processes inputs and returns application actions
menu_action_t menu_update() {
    if (input_button_pressed(INPUT_BUTTON_BACK)) {
        menu_go_back();
        return MENU_ACTION_NONE;
    }
    if (input_button_pressed(INPUT_BUTTON_SELECT)) {
        return menu_select();
    }

    joystick_direction_t direction = input_joystick_direction_edge();
    switch (direction) {
        case JOYSTICK_UP: menu_move_up(); break;
        case JOYSTICK_DOWN: menu_move_down(); break;
        case JOYSTICK_LEFT:
        case JOYSTICK_RIGHT:
        case JOYSTICK_NEUTRAL:
        default: break;
    }

    return MENU_ACTION_NONE;
}
