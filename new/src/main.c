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

static motor_calibration_t motor_limits;
static uint8_t motor_calibrated;

void system_init() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
    uart_init(SystemCoreClock);
    can_init((CanInit){.brp = 41, .sjw = 0, .propag = 4, .phase1 = 5, .phase2 = 3}, 1);
    system_tick_60Hz_init();
    adc_init();
    solenoid_init();
    arm_servo_init();
    encoder_init();
    motor_init();
    position_controller_init();
    printf("Motor calibration: homing left...\n");
    // Set use_saved to 0 here to force a fresh left/right measurement.
    motor_calibrated = motor_calibrate(&motor_limits, 1);
    if (motor_calibrated) {
        printf("Motor limits: left=%ld, right=%ld\n",
               (long)motor_limits.left, (long)motor_limits.right);
        if (!motor_calibration_save(&motor_limits)) {
            printf("Calibration usable, but flash save failed\n");
        }
        position_controller_map_target(0, -100, 100, motor_limits.right, motor_limits.left);
        position_controller_start();
    } else {
        printf("Motor calibration failed; controller remains stopped\n");
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
        case UART_COMMAND_SHOOT:
            printf("Triggering the solenoid\n");
            solenoid_pulse();
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
            printf("Joy x: %d, Joy y: %d \n, Button: %d", (int8_t)rx.byte[0], (int8_t)rx.byte[1], rx.byte[2]);
            if (motor_calibrated) {
                position_controller_map_target((int8_t)rx.byte[0], -100, 100, motor_limits.right, motor_limits.left);
            }
            arm_servo_update((int8_t)rx.byte[1]);
            if (rx.byte[2]) {
                solenoid_pulse();
            }
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

        if (system_tick_60Hz_take()) {
            
        }

        if (!adc_state()) {
            printf("-----------\n");
            printf("Beam broken\n");
            printf("-----------\n");
            time_spinFor(seconds(1));
        }
    }
}
