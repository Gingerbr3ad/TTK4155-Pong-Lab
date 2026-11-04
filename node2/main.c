#include <stdio.h>
#include <stdarg.h>
#include "uart.h"
#include "can.h"
#include "sam.h"

/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */
//#include "../path_to/uart.h"

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    uart_init(F_CPU, 115200); //Initialize UART with CPU frequency and baud rate
    
    //uart_init(/*cpufreq*/, /*baud*/);
    //printf("Hello World\n\r");
    CanInit canInit = {
        .phase2 = 4,
        .phase1 = 5,
        .propag = 1,
        .sjw = 0,
        .brp = 5,
        .smp = 0
    };
    can_init(canInit, 0);
    

    while (1)
    {
        /* code */
    }
    
}