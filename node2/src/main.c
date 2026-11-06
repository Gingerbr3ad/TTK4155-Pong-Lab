#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "drivers/uart_driver.h"
#include "utils/time.h"

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    uart_init(SystemCoreClock, 9600);

    while (1)
    {
        time_spinFor(msecs(1000));
        printf("Hello World\n");
    }
    
}