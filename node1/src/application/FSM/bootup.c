#include "application/FSM/bootup.h"
#include "application/FSM/fsm.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"
#include "application/oled/sprites.h"
#include "utils/board_macros.h"

// Splah screen at bootup duratuion = 3s
#define BOOTUP_TICKS (3 * SYSTEM_UI_TICK_HZ)

static uint8_t elapsed_ticks;
static uint8_t bootup_flag;

void bootup_enter() {
    elapsed_ticks = 0;
    bootup_flag = 1;
    framebuffer_clear();

    oled_goto_pos(4, 48);
    oled_draw_sprite(ntnu_logo, NTNU_LOGO_WIDTH, NTNU_LOGO_HEIGHT);
    oled_goto_pos(44, 43);
    oled_write_string_P(PSTR("HYPER"), OLED_FONT_LARGE);
    oled_goto_pos(56, 47);

}

void bootup_ui_tick() {
    if (elapsed_ticks > BOOTUP_TICKS - 36 && bootup_flag) {
        oled_write_string_P(PSTR("PONG"), OLED_FONT_LARGE);
        bootup_flag = 0;
    }
    if (elapsed_ticks < BOOTUP_TICKS) {
        ++elapsed_ticks;
    } else {fsm_handle_event(FSM_EVENT_MENU_ENTER);}
}

// Pressing the select button skips the splash screen
void bootup_system_tick() {
    if (input_button_pressed(INPUT_BUTTON_JOYSTICK)) {
        fsm_handle_event(FSM_EVENT_MENU_ENTER);
    }
}
