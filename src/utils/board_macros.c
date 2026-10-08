#include "utils/board_macros.h"

/*################## COMMAND FLAGS ##################*/
volatile int print_controlls_command_flag = 0;
/*###################################################*/

volatile uint8_t system_tick_flag = 0;

void system_tick_timer_init(uint8_t freq) {
    // Setup of 16 bit timer/counter 3 on pin PD4 (OC3A)
    // Select CTC mode with TOP = OCR3A
    set_bit(TCCR3B, WGM32);

    // Select clk(I/O) / 1024
    set_bit(TCCR3B, CS32);
    set_bit(TCCR3B, CS30);

    // Set timer frequency
    if (freq == 30) {
        OCR3A = 159; // 30 Hz
    } else {
        OCR3A = 199; // 24 Hz
    }

    // Enable Compare Match A interrupt
    set_bit(ETIMSK, OCIE3A);
}

// If the framebuffer was changed set a flag to send the frame buffer to the screen
ISR(TIMER3_COMPA_vect) {
    system_tick_flag = 0;
}