#pragma once

// FSM States
typedef enum {
    FSM_BOOTUP,
    FSM_MENU,
    FSM_PLAYING,
    FSM_GAME_OVER
} fsm_state_t;

// FSM Events
typedef enum {
    FSM_EVENT_NONE,
    FSM_EVENT_GAME_FINISHED
} fsm_event_t;

void fsm_init();
void fsm_update();
void fsm_handle_event(fsm_event_t event);
void fsm_system_tick_update();
fsm_state_t fsm_get_state();
