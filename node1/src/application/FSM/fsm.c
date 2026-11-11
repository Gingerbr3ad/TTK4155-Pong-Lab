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
        case FSM_PLAYING: pong_game_enter(); break;
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
        case FSM_BOOTUP: break;
        case FSM_MENU: menu_update(); break;
        case FSM_PLAYING: break;
    }
}

// Event handler - This is the function that's exposed to other files for transitioning states
void fsm_handle_event(fsm_event_t event) {
    switch (event) {
        case FSM_EVENT_GAME_FINISHED: fsm_transition(FSM_MENU); break;
        case FSM_EVENT_BOOTUP_EXIT: fsm_transition(FSM_MENU); break;
        case FSM_EVENT_START_GAME: fsm_transition(FSM_PLAYING); break;
    }
}

// This function calls the update function of the active state on each system UI tick
void fsm_system_ui_tick() {
    switch (current_state) {
        case FSM_BOOTUP: bootup_ui_tick(); break;
        case FSM_MENU: break;
        case FSM_PLAYING: pong_game_ui_tick(); break;
    }
}

// This function calls the update function of the active state on each system tick
void fsm_system_tick() {
    switch (current_state) {
        case FSM_BOOTUP: break;
        case FSM_MENU: break;
        case FSM_PLAYING: pong_game_system_tick(); break;
    }
}
