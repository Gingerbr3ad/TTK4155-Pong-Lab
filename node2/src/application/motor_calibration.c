#include <stdlib.h>

#include "sam.h"
#include "time.h"

#include "application/motor.h"
#include "application/motor_calibration.h"
#include "application/position_controller.h"
#include "drivers/encoder_driver.h"

#define CALIBRATION_EFFORT       250
#define CALIBRATION_SAMPLE_MS     10u
#define CALIBRATION_STALL_MS      300u
#define CALIBRATION_TIMEOUT_MS  15000u
#define CALIBRATION_SETTLE_MS     100u


static int32_t move_to_stop(uint8_t dir) {
    int32_t previous = encoder_read();
    uint32_t still_ms = 0;

    if (dir) {motor_set(CALIBRATION_EFFORT);}
    else {motor_set(-CALIBRATION_EFFORT);}

    // Try to move in one direction untill the carriage has stopped or the timeout was reached
    for (uint32_t elapsed = 0; elapsed < CALIBRATION_TIMEOUT_MS;) {
        time_spinFor(msecs(CALIBRATION_SAMPLE_MS));
        elapsed += CALIBRATION_SAMPLE_MS;

        int32_t position = encoder_read();
        int64_t change = (int64_t)position - previous;

        // If the encoder value changed by more than 2 reset the still time, else increment it
        if (abs(change) >= 2) {
            previous = position;
            still_ms = 0;
        } else {still_ms += CALIBRATION_SAMPLE_MS;}

        if (still_ms >= CALIBRATION_STALL_MS) {
            motor_set(0);
            time_spinFor(msecs(CALIBRATION_SETTLE_MS));
            return encoder_read();
        }
    }

    motor_set(0);
    return 0;
}

uint8_t motor_calibrate() {
    position_controller_stop();

    int32_t left;
    int32_t right;

    if (!(left = move_to_stop(0))) {return 0;}
    if (!(right = move_to_stop(1))) {return 0;}

    POSITION_MAX_LEFT = left;
    POSITION_MAX_RIGHT = right;
    POSITION_CENTER = (left + right)/2;

    return 1;
}