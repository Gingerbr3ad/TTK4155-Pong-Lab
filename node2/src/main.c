#include <stdio.h>
#include "sam.h"

#include "drivers/uart_driver.h"
#include "drivers/can_driver.h"

#include "application/servo.h"

void system_init() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
    uart_init(SystemCoreClock);
    can_init((CanInit){.brp = 41, .sjw = 0, .propag = 4, .phase1 = 5, .phase2 = 3}, 1);
    arm_servo_init();
}

static void dispatch_uart_command(uart_command_t command) {
    switch (command) {
        case UART_COMMAND_TEST:
            printf("Test command recieved\n");
            break;
        case UART_COMMAND_CAN_TEST:
            printf("Can test start\n");
            can_test_send();
            printf("Message send\n");
            break;
        case UART_COMMAND_NONE:
        default:
            break;
    }
    printf("########################################################\n");
}

static void dispatch_can_message(CanMsg rx) {
    switch (rx.id) {
        case JOYSTICK_MESSAGE_ID:
            printf("Joy x: %d, Joy y: %d \n", (int8_t)rx.byte[0], (int8_t)rx.byte[1]);
            arm_servo_update((int8_t)rx.byte[0]);
            break;
        case TEST_MESSAGE_ID:
            can_test_receive(rx);
            printf("Can test successfull\n");
            break;
        default:
            break;
    }
}

int main(void) {
    system_init();

    while (1) {
        if (uart_recieved_flag) {dispatch_uart_command(handle_uart_interrupt());}
        if (can_recieved_flag) {dispatch_can_message(handle_can_interrupt());}
    }
}
