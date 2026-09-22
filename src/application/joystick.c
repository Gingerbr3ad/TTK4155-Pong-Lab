#include "application/joystick.h"
#include "driver/adc.h"

static joystick_calibration_t calibration;

static int joystick_normalize_axis(int raw, int center, int minimum, int maximum) {
    int result;

    if (raw >= center) {
        if (maximum == center) {
            return 0;
        }

        result = ((raw - center) * 100) / (maximum - center);
    } else {
        if (center == minimum) {
            return 0;
        }

        result = -((center - raw) * 100) / (center - minimum);
    }

    if (result > 100) {
        result = 100;
    }

    if (result < -100) {
        result = -100;
    }

    return result;
}

void joystick_init() {
    int x;
    int y;

    x = adc_read(JOYSTICK_X_CHANNEL);
    y = adc_read(JOYSTICK_Y_CHANNEL);

    calibration.center_x = x;
    calibration.center_y = y;

    calibration.min_x = x;
    calibration.max_x = x;

    calibration.min_y = y;
    calibration.max_y = y;
}

joystick_position_t joystick_read_position() {
    int position;

    int raw_x;
    int raw_y;

    raw_x = adc_read(JOYSTICK_X_CHANNEL);
    raw_y = adc_read(JOYSTICK_Y_CHANNEL);

    if (raw_x < calibration.min_x) {
        calibration.min_x = raw_x;
    }

    if (raw_x > calibration.max_x) {
        calibration.max_x = raw_x;
    }

    if (raw_y < calibration.min_y) {
        calibration.min_y = raw_y;
    }

    if (raw_y > calibration.max_y) {
        calibration.max_y = raw_y;
    }

    position.x = joystick_normalize_axis(raw_x, calibration.center_x, calibration.min_x, calibration.max_x);
    position.y = joystick_normalize_axis(raw_y, calibration.center_y, calibration.min_y, calibration.max_y);
    return position;
}

joystick_direction_t joystick_get_direction() {
    joystick_position_t position;

    position = joystick_read_position();


    if (position.x < -JOYSTICK_DEADZONE) {
        return JOYSTICK_LEFT;
    }

    if (position.x > JOYSTICK_DEADZONE) {
        return JOYSTICK_RIGHT;
    }

    if (position.y > JOYSTICK_DEADZONE) {
        return JOYSTICK_UP;
    }

    if (position.y < -JOYSTICK_DEADZONE) {
        return JOYSTICK_DOWN;
    }

    return JOYSTICK_NEUTRAL;
}