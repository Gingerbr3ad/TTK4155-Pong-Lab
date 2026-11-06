#include <stdio.h>
#include "sam.h"
#include "drivers/uart_driver.h"

void system_init() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
    uart_init(SystemCoreClock);
}

// Like Node 1: perform command actions in main, not in the UART driver.
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
    }
}
