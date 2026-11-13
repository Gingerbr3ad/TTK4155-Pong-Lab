#pragma once

// FSM States
typedef enum {
    FSM_BOOTUP,
    FSM_MENU,
    FSM_PLAYING,
    FSM_PAINT,
} fsm_state_t;

// FSM Events
typedef enum {
    FSM_EVENT_MENU_ENTER,
    FSM_EVENT_START_GAME,
    FSM_EVENT_GAME_FINISHED,
    FSM_EVENT_START_PAINT,
} fsm_event_t;

void fsm_init();
void fsm_update();
void fsm_handle_event(fsm_event_t event);
void fsm_system_ui_tick();
void fsm_system_tick();
fsm_state_t fsm_get_state();
