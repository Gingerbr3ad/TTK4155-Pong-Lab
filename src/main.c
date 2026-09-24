#include "system_definitions.h"

#include <util/delay.h>
#include <stdio.h>

#include "drivers/uart_driver.h"
#include "drivers/external_memory_driver.h"
#include "drivers/adc_driver.h"
//#include "drivers/spi_driver.h"

#include "application/joystick.h"
#include "application/oled/oled_utils.h"

static FILE uartstdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

static touch_position_t touch_pos;
static joystick_position_t joy_pos;
static joystick_direction_t joy_dir;

void system_init() {
    uart_init();
    stdout = stdin = &uartstdout; // Replace the defualt stdout/in stream with the custom uart one
    external_memory_init();
    adc_init();
    //spi_init();
    joystick_init();
    oled_init();
}

int main(void) {
    system_init();

    while(1) { 
        touch_pos = touch_read_position();
        joy_pos = joystick_read_position();
        joy_dir = joystick_get_direction();
        
        //char ch = getchar(); // Waits until it gets a character on the stdin stream
        printf("Touch position: x:%i%% ; y:%i%% \n", touch_pos.x, touch_pos.y);
        printf("Joystick position: x:%i%% ; y:%i%% \n", joy_pos.x, joy_pos.y);
        printf("Joystick direction value: %i \n", joy_dir);
        _delay_ms(1000);
    }
    
    return 0;
}