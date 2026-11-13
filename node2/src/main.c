#include <stdio.h>

#include "sam.h"
#include "time.h"
#include "system_tick_60Hz.h"

#include "drivers/uart_driver.h"
#include "drivers/can_driver.h"
#include "drivers/adc_driver.h"
#include "drivers/solenoid_driver.h"
#include "drivers/encoder_driver.h"

#include "application/servo.h"
#include "application/motor.h"
#include "application/motor_calibration.h"
#include "application/position_controller.h"
#include "application/score.h"
#include "application/game_runner.h"



void system_init() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
    motor_init();
    uart_init(SystemCoreClock);
    can_init((CanInit){.brp = 41, .sjw = 0, .propag = 4, .phase1 = 5, .phase2 = 3}, 1);
    system_tick_60Hz_init();
    adc_init();
    solenoid_init();
    arm_servo_init();
    encoder_init();
    position_controller_init();
    if (!motor_calibrate()) {
        printf("Motor Calibration failed, please re-run it with 'C_MOTOR_CAL'.\n");
        return;
    }
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
        case UART_COMMAND_MOTOR_CALIBRATE:
            printf("Running motor calibration\n");
            if (!motor_calibrate()) {
                printf("Motor Calibration failed, please re-run it with 'C_MOTOR_CAL'.\n");
                break;
            }
            printf("Motor calibrated successfuly\n");
            break;
        case UART_COMMAND_SHOOT:
            printf("Triggering the solenoid\n");
            solenoid_pulse_start();
            break;
        case UART_COMMAND_NONE:
        default:
            break;
    }
    printf("########################################################\n");
}

static void dispatch_can_message(CanMsg rx) {
    switch (rx.id) {
        case TEST_MESSAGE_ID:
            can_test_receive(rx);
            printf("Can test successfull\n");
            break;
        case JOYSTICK_MESSAGE_ID:
            //printf("Joy x: %d, Joy y: %d \n, Button: %d\n", (int8_t)rx.byte[0], (int8_t)rx.byte[1], rx.byte[2]);
            game_run(rx);
            break;
        case GAME_STATUS_MESSAGE_ID:
            game_config = (uint8_t)rx.byte[0];
            game_config_update();
            break;
        case HYPER_TENSION_MESSAGE_ID:
            game_hyper_set_tension(rx.byte[0]);
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

        score_send_pending();

        if (system_tick_60Hz_take()) {
            score_system_tick();
            game_hyper_system_tick();
        }
    }
}
