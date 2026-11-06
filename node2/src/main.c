#include <stdio.h>
#include "sam.h"
#include "drivers/uart_driver.h"
#include "drivers/can_driver.h"

void system_init() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
    uart_init(SystemCoreClock);
    can_init((CanInit){.reg = CAN_BR_BRP(41) | CAN_BR_SJW(0) | CAN_BR_PROPAG(4) | CAN_BR_PHASE1(5) | CAN_BR_PHASE2(3)}, 0);
}

static void can_test_receive(void) {
    CanMsg rx;
    if (can_rx(&rx) && rx.id == 0x123 && rx.length == 3 &&
        rx.byte[0] == 0xAA && rx.byte[1] == 0xBB && rx.byte[2] == 0xCC) {
        printf("CAN test received from Node 1: ID=0x123, data=AA BB CC\r\n");
    }
}

static void dispatch_uart_command(uart_command_t command) {
    switch (command) {
        case UART_COMMAND_TEST:
            printf("Test command recieved\n");
            break;
        case UART_COMMAND_NONE:
        default:
            break;
    }
    printf("##############################################################\n");
}

int main(void) {
    system_init();

    while (1) {
        if (uart_recieved_flag) {dispatch_uart_command(handle_uart_interrupt());}
        can_test_receive();
    }
}
