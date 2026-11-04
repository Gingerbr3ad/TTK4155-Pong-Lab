#include "utils/board_macros.h"

#include "drivers/uart_driver.h"
#include "drivers/external_memory_driver.h"
#include "drivers/adc_driver.h"
#include "drivers/spi_driver.h"

#include "application/joystick.h"
#include "application/input.h"
#include "application/FSM/fsm.h"
#include "application/oled/oled_utils.h"
#include "application/sram_test.h"

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
static uint8_t ticks;

void system_init() {
    uart_init();
    stdout = stdin = &uartstdout;
    external_memory_init();
    adc_init();
    spi_init();
    joystick_init();
    input_init();
    oled_init();
    fsm_init();
    system_tick_timer_init(SYSTEM_UI_TICK_HZ);
    sei(); // Enable global interrupts
}


// Handles the uart commands in main as to not have to include everything in the uart driver
static void dispatch_uart_command(uart_command_t command) {
    switch(command) {
        case UART_COMMAND_NONE: break;
        case UART_COMMAND_TEST: printf("Test command recieved\n"); break;
        case UART_COMMAND_OLED_CLEAR: 
            framebuffer_clear();
            printf("Clearing the OLED screen \n");
            break;
        case UART_COMMAND_OLED_CHECKERS:
            oled_checkerboard_test();
            printf("Drawing the checker board pattern on the OLED screen \n");
            break;
        case UART_COMMAND_PRINT_DEBUG:
            print_controlls_command_flag = !print_controlls_command_flag;
            printf("Toggled debug printing \n");
            break;
        case UART_COMMAND_SRAM_TEST:
            SRAM_test();
            break;
        case UART_COMMAND_UI_UP: input_inject_joystick(JOYSTICK_UP); break;
        case UART_COMMAND_UI_DOWN: input_inject_joystick(JOYSTICK_DOWN); break;
        case UART_COMMAND_UI_SELECT: input_inject_button(INPUT_BUTTON_SELECT); break;
        case UART_COMMAND_UI_BACK: input_inject_button(INPUT_BUTTON_BACK); break;
        case UART_COMMAND_GAME_FINISHED: fsm_handle_event(FSM_EVENT_GAME_FINISHED); break;
        default: break;
    }

    printf("##############################################################\n");
}

static void debug_print() {
    // If the print flag is high, print debug info once a second
    if (print_controlls_command_flag) {
        if (++ticks < SYSTEM_UI_TICK_HZ) {
            return;
        }
        ticks = 0;

        touch_position_t touch = touch_read_position();
        joystick_position_t joy = joystick_read_position();
        joystick_direction_t direction = joystick_get_direction();
        uint8_t joystick_button = joystick_button_read();
        printf("Touch %d,%d Joy %d,%d Dir %d, Joy Button %d\n", touch.x, touch.y, joy.x, joy.y, direction, joystick_button);
    }
}

int main(void) {
    system_init();

    while (1) {
        // ###### EACH LOOP ######
        if (uart_recieved_flag) {dispatch_uart_command(handle_uart_interrupt());}
        input_update();
        fsm_update();


        // ###### SYSTEM TICK ######
        if (system_tick_take()) {
            fsm_system_tick_update();
            oled_flush();
            debug_print();
        }
    }
}
