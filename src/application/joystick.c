#include "joystick.h"
#include "adc.h"

#define JOYSTICK_X_CHANNEL 0
#define JOYSTICK_Y_CHANNEL 1

#define JOYSTICK_DEADZONE 20


typedef struct
{
    uint8_t center_x;
    uint8_t center_y;

    uint8_t min_x;
    uint8_t max_x;

    uint8_t min_y;
    uint8_t max_y;

} joystick_calibration_t;


/* Calibration values are only used inside joystick.c */
static joystick_calibration_t calibration;


/*
 * Converts one ADC axis from a raw value
 * into a value from -100 to +100.
 */
static int16_t joystick_normalize_axis(
    uint8_t raw,
    uint8_t center,
    uint8_t minimum,
    uint8_t maximum)
{
    int16_t result;

    /*
     * Positive side of the joystick:
     * center -> maximum
     */
    if (raw >= center)
    {
        /*
         * Prevent division by zero before
         * the joystick has been moved.
         */
        if (maximum == center)
        {
            return 0;
        }

        result =
            ((int16_t)(raw - center) * 100)
            / (maximum - center);
    }

    /*
     * Negative side of the joystick:
     * minimum -> center
     */
    else
    {
        if (center == minimum)
        {
            return 0;
        }

        result =
            -((int16_t)(center - raw) * 100)
            / (center - minimum);
    }


    /*
     * Make sure the result stays
     * between -100 and +100.
     */
    if (result > 100)
    {
        result = 100;
    }

    if (result < -100)
    {
        result = -100;
    }

    return result;
}


/*
 * Call this while the joystick is centered.
 */
void joystick_init(void)
{
    uint8_t x;
    uint8_t y;

    /*
     * Ask the ADC driver for the current
     * joystick X and Y values.
     */
    x = adc_read(JOYSTICK_X_CHANNEL);
    y = adc_read(JOYSTICK_Y_CHANNEL);


    /*
     * The joystick should be centered now,
     * so save these values as the center.
     */
    calibration.center_x = x;
    calibration.center_y = y;


    /*
     * At startup, the only values we have
     * seen are the center values.
     */
    calibration.min_x = x;
    calibration.max_x = x;

    calibration.min_y = y;
    calibration.max_y = y;
}


/*
 * Reads the joystick and returns:
 *
 * x = -100 ... +100
 * y = -100 ... +100
 */
joystick_position_t joystick_read_position(void)
{
    joystick_position_t position;

    uint8_t raw_x;
    uint8_t raw_y;


    /*
     * Get new measurements from the ADC driver.
     */
    raw_x = adc_read(JOYSTICK_X_CHANNEL);
    raw_y = adc_read(JOYSTICK_Y_CHANNEL);


    /*
     * AUTO-CALIBRATION FOR X
     */

    if (raw_x < calibration.min_x)
    {
        calibration.min_x = raw_x;
    }

    if (raw_x > calibration.max_x)
    {
        calibration.max_x = raw_x;
    }


    /*
     * AUTO-CALIBRATION FOR Y
     */

    if (raw_y < calibration.min_y)
    {
        calibration.min_y = raw_y;
    }

    if (raw_y > calibration.max_y)
    {
        calibration.max_y = raw_y;
    }


    /*
     * Convert the raw ADC values into
     * percentages from -100 to +100.
     */
    position.x = joystick_normalize_axis(
        raw_x,
        calibration.center_x,
        calibration.min_x,
        calibration.max_x
    );

    position.y = joystick_normalize_axis(
        raw_y,
        calibration.center_y,
        calibration.min_y,
        calibration.max_y
    );


    return position;
}


/*
 * Converts the analog joystick position
 * into a simple direction.
 */
joystick_direction_t joystick_get_direction(void)
{
    joystick_position_t position;

    position = joystick_read_position();


    if (position.x < -JOYSTICK_DEADZONE)
    {
        return JOYSTICK_LEFT;
    }

    if (position.x > JOYSTICK_DEADZONE)
    {
        return JOYSTICK_RIGHT;
    }

    if (position.y > JOYSTICK_DEADZONE)
    {
        return JOYSTICK_UP;
    }

    if (position.y < -JOYSTICK_DEADZONE)
    {
        return JOYSTICK_DOWN;
    }


    return JOYSTICK_NEUTRAL;
}