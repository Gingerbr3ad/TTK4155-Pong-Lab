#include "application/game_runner.h"
#include "application/position_controller.h"
#include "application/motor.h"
#include "application/servo.h"
#include "application/score.h"

#include "drivers/encoder_driver.h"
#include "drivers/solenoid_driver.h"

volatile uint8_t game_config = 0;

static uint8_t speed_control_bit;
static uint8_t position_control_bit;
static uint8_t slider_control_bit;
static uint8_t normal_mode_control_bit;
static uint8_t hyper_mode_control_bit;
static uint8_t game_on_control_bit;

void game_config_update() {
    speed_control_bit = (game_config >> 0) & 1;
    position_control_bit = (game_config >> 1) & 1;
    slider_control_bit = (game_config >> 2) & 1;
    normal_mode_control_bit = (game_config >> 3) & 1;
    hyper_mode_control_bit = (game_config >> 4) & 1;
    game_on_control_bit = (game_config >> 7) & 1;
}

void game_run(CanMsg rx) {
    if(game_on_control_bit) {
        position_controller_start();
        score_start();

        if (speed_control_bit) {
            motor_set(-(int8_t)rx.byte[0]);
            arm_servo_update((int8_t)rx.byte[1]);
        } else if (position_control_bit) {
            position_controller_map_target(-(int8_t)rx.byte[0], -100, 100, POSITION_MAX_RIGHT, POSITION_MAX_LEFT);
            arm_servo_update((int8_t)rx.byte[1]);
        } else if (slider_control_bit) {
            position_controller_map_target(-(int16_t)rx.byte[0], 0, 255, POSITION_MAX_RIGHT, POSITION_MAX_LEFT);
            arm_servo_update((int8_t)rx.byte[1]);
        }

        if (rx.byte[2]) {
            solenoid_pulse_start();
        }

        return;        
    }

    // Disable the game board by default
    score_stop();
    position_controller_stop();
    motor_set(0);
}
