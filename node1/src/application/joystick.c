#include <stdlib.h>

#include "application/joystick.h"
#include "drivers/adc_driver.h"

static joystick_calibration_t calibration;

static uint8_t x_history[JOYSTICK_FILTER_SIZE];
static uint8_t y_history[JOYSTICK_FILTER_SIZE];

static uint16_t x_sum = 0;
static uint16_t y_sum = 0;

static uint8_t history_index = 0;
joystick_position_t prev_position;

static int clamp_raw(int value) {
    if (value < JOYSTICK_RAW_MIN) {
        return JOYSTICK_RAW_MIN;
    }

    if (value > JOYSTICK_RAW_MAX) {
        return JOYSTICK_RAW_MAX;
    }

    return value;
}

static int normalize_axis(int raw, int center, int minimum, int maximum) {
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
    // Button pin setup
    clear_bit(DDRD, JOYSITCK_BUTTON);
    set_bit(PORTD, JOYSITCK_BUTTON);

    prev_position.x = 0;
    prev_position.y = 0;
    x_sum = 0;
    y_sum = 0;
    history_index = 0;

    // Initialize the joystick avarage value with initial readings
    for (uint8_t i = 0; i < JOYSTICK_FILTER_SIZE; i++) {
        x_history[i] = clamp_raw(adc_read(JOYSTICK_X_CHANNEL));
        y_history[i] = clamp_raw(adc_read(JOYSTICK_Y_CHANNEL));

        x_sum += x_history[i];
        y_sum += y_history[i];
    }

    // Calibrate center from average of initial readings
    calibration.center_x = x_sum / JOYSTICK_FILTER_SIZE;
    calibration.center_y = y_sum / JOYSTICK_FILTER_SIZE;

    calibration.min_x = 75;
    calibration.max_x = 230;

    calibration.min_y = 75;
    calibration.max_y = 230;
}

touch_position_t touch_read_position() {
    touch_position_t position;

    int raw_x = adc_read(TOUCH_X_CHANNEL);
    int raw_y = adc_read(TOUCH_Y_CHANNEL);

    position.x = normalize_axis(raw_x, 255 / 2, 0, 255);
    position.y = normalize_axis(raw_y, 255 / 2, 0, 255);

    return position;
}


// Calibration method was found on https://dyadica.co.uk/blog/simple-bluetooth-joystick/
joystick_position_t joystick_read_position() {
    joystick_position_t position;

    int raw_x = clamp_raw(adc_read(JOYSTICK_X_CHANNEL));
    int raw_y = clamp_raw(adc_read(JOYSTICK_Y_CHANNEL));

    // Remove oldest readings from sum
    x_sum -= x_history[history_index];
    y_sum -= y_history[history_index];

    // Store new readings
    x_history[history_index] = raw_x;
    y_history[history_index] = raw_y;

    // Add new readings to sum
    x_sum += raw_x;
    y_sum += raw_y;

    // Advance buffer index
    history_index++;
    if (history_index >= JOYSTICK_FILTER_SIZE) {
        history_index = 0;
    }

    // Calculate moving averages
    int average_x = x_sum / JOYSTICK_FILTER_SIZE;
    int average_y = y_sum / JOYSTICK_FILTER_SIZE;

    position.x = normalize_axis(average_x, calibration.center_x, calibration.min_x, calibration.max_x);
    position.y = normalize_axis(average_y, calibration.center_y, calibration.min_y, calibration.max_y);
    
    if ((abs(position.x - prev_position.x) < 3) && (abs(position.y - prev_position.y) < 3)) {
        return prev_position;
    }

    prev_position = position;
    return position;
}

joystick_direction_t joystick_get_direction(joystick_position_t position) {
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

uint8_t joystick_button_read() {
    return !test_bit(PIND, JOYSITCK_BUTTON);
}
