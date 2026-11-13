#include "application/game_runner.h"
#include "application/position_controller.h"
#include "application/motor.h"
#include "application/servo.h"
#include "application/score.h"

#include "drivers/encoder_driver.h"
#include "drivers/solenoid_driver.h"

volatile uint8_t game_config = 1u;

static uint8_t speed_control_bit;
static uint8_t position_control_bit;
static uint8_t slider_control_bit;
static uint8_t normal_mode_control_bit;
static uint8_t hyper_mode_control_bit;
static uint8_t game_on_control_bit;
static uint8_t hyper_telemetry_ticks;

CanMsg hyper_motor_message = {
    .id = HYPER_MOTOR_MESSAGE_ID,
    .length = 1,
    .byte = {0x00}
};

void game_config_update() {
    speed_control_bit = (game_config >> 0) & 1;
    position_control_bit = (game_config >> 1) & 1;
    slider_control_bit = (game_config >> 2) & 1;
    hyper_mode_control_bit = (game_config >> 3) & 1;
    game_on_control_bit = (game_config >> 7) & 1;

    hyper_telemetry_ticks = 0;

    if (game_on_control_bit) {
        // Start the game
        score_start();

        if (position_control_bit || slider_control_bit) {
            position_controller_start();
        } else {
            position_controller_stop();
            motor_set(0);
        }
    } else {
        // End the game
        score_stop();
        position_controller_stop();
        motor_set(0);
        arm_servo_update(0);
        solenoid_set_state(0);
    }
}

void game_run(CanMsg rx) {
    if (!game_on_control_bit) {return;}

    if (speed_control_bit) {
        motor_set(((int8_t)rx.byte[0]) * 6);
        arm_servo_update((int8_t)rx.byte[1]);
    } else if (position_control_bit) {
        position_controller_map_target(-(int8_t)rx.byte[0], -100, 100, POSITION_MAX_RIGHT, POSITION_MAX_LEFT);
        arm_servo_update((int8_t)rx.byte[1]);

    } else if (slider_control_bit) {
        position_controller_map_target(255 - (int16_t)rx.byte[0], 0, 255, POSITION_MAX_RIGHT, POSITION_MAX_LEFT);
        if ((rx.byte[2] >> 1) & 1) {
            arm_servo_update(-100);
        } else if ((rx.byte[2] >> 2) & 1) {
            arm_servo_update(100);
        } else {
            arm_servo_update((int8_t)rx.byte[1]);
        }
    }

    if (rx.byte[2] & 1) {
        solenoid_pulse_start();
    }
}

void game_hyper_system_tick() {
    if (!game_on_control_bit || !hyper_mode_control_bit) {return;}
    if (++hyper_telemetry_ticks < 5) {return;}
    hyper_telemetry_ticks = 0;

    int32_t percent = (int32_t)(((int64_t)(encoder_read() - POSITION_MAX_RIGHT) * 100) / (POSITION_MAX_LEFT - POSITION_MAX_RIGHT));
    if (percent < 0) {percent = 0;}
    if (percent > 100) {percent = 100;}

    percent = 100 - percent;

    hyper_motor_message.byte[0] = (uint8_t)percent;
    can_send(hyper_motor_message);
}