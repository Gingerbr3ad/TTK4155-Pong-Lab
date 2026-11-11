#pragma once

// Actions that the menu can request from the FSM
typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_START_GAME
} menu_action_t;

void menu_enter();
menu_action_t menu_system_tick();
