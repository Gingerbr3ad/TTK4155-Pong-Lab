#include <stdio.h>
#include "sam.h"
#include "drivers/uart_driver.h"
#include "drivers/can_driver.h"

void system_init() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
    uart_init(SystemCoreClock);
    can_init((CanInit){.reg = CAN_BR_BRP(41) | CAN_BR_SJW(0) | CAN_BR_PROPAG(4) | CAN_BR_PHASE1(5) | CAN_BR_PHASE2(3)}, 1);
}

static void dispatch_uart_command(uart_command_t command) {
    switch (command) {
        case UART_COMMAND_TEST:
            printf("Test command recieved\n");
            break;
        case UART_COMMAND_CAN_TEST:
            printf("Can test start\n");
            if(!can_test_send()){printf("Message send succesfully \n");};
            break;
        case UART_COMMAND_NONE:
        default:
            break;
    }
    printf("##############################################################\n");
}

static void dispatch_can_message(CanMsg rx) {
    switch (rx.id) {
        case 0x100:
            printf("Joy x: %d, Joy y: %d \n", (int8_t)rx.byte[0], (int8_t)rx.byte[1]);
            break;
        case 0x123:
            can_test_receive(rx);
            printf("Can test\n");
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
