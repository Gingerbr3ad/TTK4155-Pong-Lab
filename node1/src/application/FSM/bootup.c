#include "application/FSM/bootup.h"
#include "application/input.h"
#include "application/oled/oled_utils.h"
#include "application/oled/sprites.h"
#include "utils/board_macros.h"

// Splah screen at bootup duratuion = 3s
#define BOOTUP_TICKS (3 * SYSTEM_UI_TICK_HZ)

static uint8_t elapsed_ticks;

void bootup_enter() {
    elapsed_ticks = 0;
    framebuffer_clear();

    oled_goto_pos(0, 48);
    oled_draw_sprite(ntnu_logo, NTNU_LOGO_WIDTH, NTNU_LOGO_HEIGHT);
    oled_goto_pos(40, 47);
    oled_write_string_P(PSTR("PONG"), OLED_FONT_LARGE);
}

uint8_t bootup_ui_tick() {
    if (elapsed_ticks < BOOTUP_TICKS) {
        ++elapsed_ticks;
    }
    return elapsed_ticks >= BOOTUP_TICKS;
}

// Pressing the select button skips the splash screen
uint8_t bootup_update(void) {
    return input_button_pressed(INPUT_BUTTON_JOYSTICK);
}
