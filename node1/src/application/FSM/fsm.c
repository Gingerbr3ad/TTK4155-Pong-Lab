#include "application/FSM/fsm.h"
#include "application/FSM/bootup.h"
#include "application/FSM/menu.h"
#include "application/FSM/pong_game.h"

static fsm_state_t current_state;

// When called, calls the state enter function for the passed state
static void fsm_transition(fsm_state_t next_state) {
    current_state = next_state;

    switch (current_state) {
        case FSM_BOOTUP: bootup_enter(); break;
        case FSM_MENU: menu_enter(); break;
        case FSM_PLAYING: game_enter(); break;
        case FSM_GAME_OVER: game_over_enter(); break;
    }
}

void fsm_init() {
    fsm_transition(FSM_BOOTUP);
}

fsm_state_t fsm_get_state() {
    return current_state;
}

// This is a function for calling state updates that should run on every loop rather than on system tick
// This mainly checks if a state transition should occur
// Implemented in this form to have a centralized state transition logic, instead of having it scattered in separate state functions
void fsm_update() {
    switch (current_state) {
        case FSM_BOOTUP: if (bootup_update()) {fsm_transition(FSM_MENU);} break;
        case FSM_MENU: if (menu_update() == MENU_ACTION_START_GAME) {fsm_transition(FSM_PLAYING);} break;
        case FSM_PLAYING: if (game_update()) {fsm_transition(FSM_MENU);} break;
        case FSM_GAME_OVER: if (game_over_update()) {fsm_transition(FSM_MENU);} break;
    }
}

// Event handler - TODO: Expand this
void fsm_handle_event(fsm_event_t event) {
    if (current_state == FSM_PLAYING && event == FSM_EVENT_GAME_FINISHED) {
        fsm_transition(FSM_GAME_OVER);
    }
}

// This function calls the update function of the active state on each system tick
void fsm_system_tick_update() {
    switch (current_state) {
        case FSM_BOOTUP:
            if (bootup_tick()) {
                fsm_transition(FSM_MENU);
            }
            break;
        case FSM_MENU: break;
        case FSM_PLAYING: 
            game_system_update();
        case FSM_GAME_OVER: break;
    }
}
