#include "utils/board_macros.h"

#include "drivers/uart_driver.h"
#include "drivers/external_memory_driver.h"
#include "drivers/adc_driver.h"
#include "drivers/spi_driver.h"
#include "drivers/can_driver.h"

#include "application/input.h"
#include "application/FSM/fsm.h"
#include "application/FSM/pong_game.h"
#include "application/oled/oled_utils.h"
#include "application/sram_test.h"

#include <avr/pgmspace.h>

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
static uint8_t ticks;

static void system_init() {
    external_memory_init();
    uart_init();
    stdout = stdin = &uartstdout;
    adc_init();
    spi_init();
    can_init();
    input_init();
    oled_init();
    fsm_init();
    system_tick_timer_init(SYSTEM_UI_TICK_HZ);
    sei(); // Enable global interrupts
}


// Handles the uart commands in main as to not have to include everything in the uart driver
static void dispatch_uart_command(uart_command_t command) {
    switch(command) {
        case UART_COMMAND_TEST: printf_P(PSTR("Test command recieved\n")); break;
        case UART_COMMAND_OLED_CLEAR: 
            framebuffer_clear();
            printf_P(PSTR("Clearing the OLED screen \n"));
            break;
        case UART_COMMAND_OLED_CHECKERS:
            oled_checkerboard_test();
            printf_P(PSTR("Drawing the checker board pattern on the OLED screen \n"));
            break;
        case UART_COMMAND_PRINT_DEBUG:
            print_controlls_command_flag = !print_controlls_command_flag;
            printf_P(PSTR("Toggled debug printing \n"));
            break;
        case UART_COMMAND_SRAM_TEST:
            //SRAM_test();
            //printf("Disabled to save program memory.");
            break;
        case UART_COMMAND_CAN_TEST:
            printf_P(PSTR("Can test start\n"));
            if(!can_test_send()){printf_P(PSTR("Message send succesfully \n"));};
            break;
        case UART_COMMAND_UI_UP: input_inject_joystick(JOYSTICK_UP); break;
        case UART_COMMAND_UI_DOWN: input_inject_joystick(JOYSTICK_DOWN); break;
        case UART_COMMAND_UI_SELECT: input_inject_button(INPUT_BUTTON_JOYSTICK); break;
        case UART_COMMAND_UI_BACK: input_inject_button(INPUT_BUTTON_L6); break;
        case UART_COMMAND_GAME_FINISHED: fsm_handle_event(FSM_EVENT_GAME_FINISHED); break;
        case UART_COMMAND_NONE: break;
        default: break;
    }
    printf("########################################################\n");
}

static void dispatch_can_message(can_message_t *msg) {
    switch (msg->id) {
        case TEST_MESSAGE_ID:
            if (!can_test_receive(*msg)) {printf("Can test successful\n");}
            break;
        case SCORE_MESSAGE_ID:
            if (fsm_get_state() == FSM_PLAYING) {
                uint32_t score = (uint32_t)msg->data[0] | ((uint32_t)msg->data[1] << 8) | ((uint32_t)msg->data[2] << 16) | ((uint32_t)msg->data[3] << 24);
                pong_game_receive_score(score);
            }
            break;
        case HYPER_MOTOR_MESSAGE_ID:
            if (fsm_get_state() == FSM_PLAYING) {
                pong_game_receive_motor_position(msg->data[0]);
            }
            break;
        default:
            break;
    }
}

static void debug_print() {
    // If the print flag is high, print debug info once a second
    if (print_controlls_command_flag) {
        if (++ticks < SYSTEM_UI_TICK_HZ) {
            return;
        }
        ticks = 0;

        touch_position_t touch = input_touch_position_read();
        joystick_position_t joy = input_joystick_position_read();
        joystick_direction_t direction = input_joystick_direction_read();
        input_slider_t slider = input_slider_read();
        uint32_t buttons = input_buttons_read();
        printf_P(PSTR("Touch %d,%d Joy %d,%d Dir %d, Silder %d, Buttons 0x%08lX\n"), touch.x, touch.y, joy.x, joy.y, direction, slider.position, (unsigned long)buttons);
        fsm_state_t fsm_state = fsm_get_state();
        printf_P(PSTR("FSM State: %d \n"), fsm_state);
        printf_P(PSTR("------------------------------------------------------\n"));
    }
}

int main() {
    system_init();

    while (1) {
        // ###### EACH LOOP ######
        if (uart_recieved_flag) {dispatch_uart_command(handle_uart_interrupt());}
        if (can_received_flag) {dispatch_can_message(handle_can_interrupt());}
        fsm_update();


        // ###### SYSTEM TICK ######
        if (system_tick_take()) {
            input_update();
            fsm_system_tick();
        }


        // ###### SYSTEM UI TICK ######
        if (system_ui_tick_take()) {
            fsm_system_ui_tick();
            oled_flush();
            debug_print();

        }
    }
}
