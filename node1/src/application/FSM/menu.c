#include "application/FSM/menu.h"
#include "application/FSM/fsm.h"
#include "application/FSM/pong_game.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"

/* TODO: This is bad and should be refactored if there is time, handling the menu and sub menus should be more modular and adding more options should be a lot easier, maybe make it heuristic? */


// Menu pages (main menu + sub menus)
typedef enum {
    MENU_MAIN,
    MENU_OPTIONS,
    MENU_CONTROL_SCHEME,
    MENU_GAMEMODE,
    MENU_MINIGAMES,
} menu_page_t;

static menu_page_t menu_page;
static uint8_t selection;

// Returns the number of option for the menu page
static uint8_t menu_item_count() {
    switch (menu_page) {
        case MENU_MAIN: return 3;
        case MENU_OPTIONS: return 2;
        case MENU_CONTROL_SCHEME: return 3;
        case MENU_GAMEMODE: return 2;
        case MENU_MINIGAMES: return 1;
    }
    return 0;
}

// Draws the current menu page
static void menu_draw() {
    oled_goto_pos(0, 4);

    switch (menu_page) {
        case MENU_MAIN: oled_write_string_P(PSTR("PONG MENU"), OLED_FONT_LARGE); break;
        case MENU_OPTIONS: oled_write_string_P(PSTR("OPTIONS"), OLED_FONT_LARGE); break;
        case MENU_CONTROL_SCHEME: oled_write_string_P(PSTR("CONTROL SCHEME"), OLED_FONT_LARGE); break;
        case MENU_GAMEMODE: oled_write_string_P(PSTR("GAME MODES"), OLED_FONT_LARGE); break;
        case MENU_MINIGAMES: oled_write_string_P(PSTR("MINIGAMES"), OLED_FONT_LARGE); break;
    }

    // Draws the menu page options along with the selection cursor '>'
    for (uint8_t i = 0; i < menu_item_count(); ++i) {

        oled_goto_page(i+2, 4);
        if (i == selection) {
            oled_write_string_P(PSTR(">"), OLED_FONT_NORMAL);
        } else {
            oled_write_string_P(PSTR(" "), OLED_FONT_NORMAL);
        }
        oled_goto_page(i+2, 16);

        if (menu_page == MENU_MAIN) {
            switch (i) {
                case 0: oled_write_string_P(PSTR("Play Pong(?)"), OLED_FONT_NORMAL); break;
                case 1: oled_write_string_P(PSTR("Options"), OLED_FONT_NORMAL); break;
                case 2: oled_write_string_P(PSTR("MINIGAMES :D"), OLED_FONT_NORMAL); break;
            }
        } else if (menu_page == MENU_OPTIONS) {
            switch (i) {
                case 0: oled_write_string_P(PSTR("Control Scheme"), OLED_FONT_NORMAL); break;
                case 1: oled_write_string_P(PSTR("Gamemodes"), OLED_FONT_NORMAL); break;
            }
        } else if (menu_page == MENU_CONTROL_SCHEME) {
            switch (i) {
                case 0: oled_write_string_P(PSTR("Speed Control"), OLED_FONT_NORMAL); break;
                case 1: oled_write_string_P(PSTR("Position Control"), OLED_FONT_NORMAL); break;
                case 2: oled_write_string_P(PSTR("Position + Slider"), OLED_FONT_NORMAL); break;
            }
        } else if (menu_page == MENU_GAMEMODE) {
            switch (i) {
                case 0: oled_write_string_P(PSTR("WIP"), OLED_FONT_NORMAL); break;
            }
        } else if (menu_page == MENU_MINIGAMES) {
            switch (i) {
                case 0: oled_write_string_P(PSTR("WIP D:"), OLED_FONT_NORMAL); break;
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
static void menu_select() {
    switch (menu_page) {
        case MENU_MAIN:
            switch (selection) {
                case 0: fsm_handle_event(FSM_EVENT_START_GAME); break;
                case 1: menu_open(MENU_OPTIONS); break;
                case 2: menu_open(MENU_MINIGAMES); break;
        } break;
        case MENU_OPTIONS:
            switch (selection) {
                case 0: menu_open(MENU_CONTROL_SCHEME); break;
                case 1: menu_open(MENU_GAMEMODE); break;
        } break;
        case MENU_GAMEMODE:
            switch (selection) {
                case 0: game_config |= 1 << 3; menu_open(MENU_MAIN); break;
                case 1: game_config |= 1 << 4; menu_open(MENU_MAIN); break;
        } break;
        case MENU_CONTROL_SCHEME:
            switch (selection) {
                case 0: game_config |= 1 << 0; menu_open(MENU_MAIN); break;
                case 1: game_config |= 1 << 1; menu_open(MENU_MAIN); break;
                case 2: game_config |= 1 << 2; menu_open(MENU_MAIN); break;
        } break;
        case MENU_MINIGAMES:
            switch (selection) {
                case 0: break;
        } break;
    }
}

// Returns to the parent menu
static void menu_go_back() {
    switch (menu_page) {
        case MENU_MINIGAMES: menu_open(MENU_MAIN); break;
        case MENU_CONTROL_SCHEME: menu_open(MENU_OPTIONS); break;
        case MENU_GAMEMODE: menu_open(MENU_OPTIONS); break;
        case MENU_OPTIONS: menu_open(MENU_MAIN); break;
        case MENU_MAIN: break;
    }
}

// Initializes the menu state
void menu_enter() {
    menu_open(MENU_MAIN);
}

// Menu navigation
void menu_system_tick() {
    joystick_direction_t direction = input_joystick_direction_edge();

    if (direction == JOYSTICK_UP || input_button_pressed(INPUT_BUTTON_NAV_UP)) {menu_move_up();}
    else if (direction == JOYSTICK_DOWN || input_button_pressed(INPUT_BUTTON_NAV_DOWN)) {menu_move_down();}
    else if (direction == JOYSTICK_LEFT || input_button_pressed(INPUT_BUTTON_NAV_LEFT) || input_button_pressed(INPUT_BUTTON_L6) || input_button_pressed(INPUT_BUTTON_L7)) {menu_go_back();}
    else if (direction == JOYSTICK_RIGHT || input_button_pressed(INPUT_BUTTON_NAV_RIGHT) || input_button_pressed(INPUT_BUTTON_NAV_CENTER) || input_button_pressed(INPUT_BUTTON_R6) || input_button_pressed(INPUT_BUTTON_JOYSTICK)) {menu_select();}
}
