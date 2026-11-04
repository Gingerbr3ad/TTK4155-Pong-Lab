#include "application/input.h"
#include "drivers/spi_driver.h"

#define IO_READ_SLIDER 0x02
#define IO_READ_BUTTONS 0x04
#define DOWNCLOCK 50

static joystick_position_t joystick_position;
static touch_position_t touch_position;
static joystick_direction_t previous_direction;
static joystick_direction_t direction_edge;

static uint32_t down_buttons;
static uint32_t pressed_buttons;
static uint32_t released_buttons;
static input_buttons_t board_buttons; // Board button state in 3 8bit masks

static input_slider_t slider;

static joystick_direction_t injected_direction;
static uint32_t injected_buttons;

static uint8_t downclock_counter;

// Special SPI read to account for the time delays specified in the IO board documentation
static void io_read(uint8_t command, uint8_t *data, uint8_t length) {
    spi_w(&command, 1, io_ss);
    _delay_us(40);

    for (uint8_t i = 0; i < length; ++i) {
        uint8_t dummy = 0x00;
        spi_wr(&dummy, 1, io_ss, &data[i]);

        if (i + 1 < length) {
            _delay_us(2);
        }
    }
}

static uint32_t read_buttons() {
    // Read the button states from the IO
    uint8_t data[3];
    io_read(IO_READ_BUTTONS, data, sizeof(data));

    // Ignore unused bits
    board_buttons.right = data[0] & 0x3F;
    board_buttons.left = data[1] & 0x7F;
    board_buttons.nav = data[2] & 0x1F;

    // Creates a 32 bit mask for all of the buttons, and shifts the *right* buttons into the mask
    uint32_t buttons = (uint32_t)board_buttons.right << INPUT_BUTTON_R1;
    // Adds teh *left* buttons into the button mask
    buttons |= (uint32_t)board_buttons.left << INPUT_BUTTON_L1;
    // Adds the *nav* buttons to the button mask
    buttons |= (uint32_t)board_buttons.nav << INPUT_BUTTON_NAV_CENTER;
    // Adds the joystick buttin to the button mask if its high, this needs 1UL to make sure the correct 32u int is used
    if (joystick_button_read()) {
        buttons |= 1UL << INPUT_BUTTON_JOYSTICK;
    }

    return buttons;
}

void input_init() {
    joystick_init();

    joystick_position = (joystick_position_t){0, 0};
    touch_position = (touch_position_t){0, 0};
    previous_direction = JOYSTICK_NEUTRAL;
    direction_edge = JOYSTICK_NEUTRAL;
    injected_direction = JOYSTICK_NEUTRAL;
    injected_buttons = 0;
    down_buttons = 0;
    pressed_buttons = 0;
    released_buttons = 0;
    board_buttons = (input_buttons_t){0, 0, 0};
    slider = (input_slider_t){0, 0};
    downclock_counter = 0;
}

// Takes the input values on each loop
void input_update() {
    // Clear previous input events
    direction_edge = JOYSTICK_NEUTRAL;
    pressed_buttons = 0;
    released_buttons = 0;

    if (++downclock_counter < DOWNCLOCK) {
        return;
    }
    downclock_counter = 0;

    joystick_position = joystick_read_position();
    touch_position = touch_read_position();
    joystick_direction_t direction = joystick_get_direction(joystick_position);

    // If the direction has changed, register it as the edge, else reports neutral edge (no movement)
    direction_edge = JOYSTICK_NEUTRAL;
    if (direction != previous_direction) {
        direction_edge = direction;
    }

    // If a direction was injected, it overwrites the next input update with it
    if(injected_direction != JOYSTICK_NEUTRAL) {
        direction_edge = injected_direction;
        injected_direction = JOYSTICK_NEUTRAL;
    }

    uint32_t buttons = read_buttons();
    pressed_buttons = buttons & ~down_buttons;
    released_buttons = down_buttons & ~buttons;
    down_buttons = buttons;

    // Injected button presses get added to the hardware presses for one update.
    pressed_buttons |= injected_buttons;
    injected_buttons = 0;

    uint8_t data[2];
    io_read(IO_READ_SLIDER, data, sizeof(data));
    slider.position = data[0];
    slider.size = data[1];
}

joystick_position_t input_joystick_position_read() {
    return joystick_position;
}

joystick_direction_t input_joystick_direction_read() {
    return previous_direction;
}

// Returns the direction edge
joystick_direction_t input_joystick_direction_edge() {
    return direction_edge;
}

touch_position_t input_touch_position_read() {
    return touch_position;
}

uint8_t input_button_pressed(input_button_t button) {
    if ((unsigned int)button >= INPUT_BUTTON_COUNT) {
        return 0;
    }
    return (pressed_buttons & (1UL << button)) != 0;
}

uint8_t input_button_down(input_button_t button) {
    if ((unsigned int)button >= INPUT_BUTTON_COUNT) {
        return 0;
    }
    return (down_buttons & (1UL << button)) != 0;
}

uint8_t input_button_released(input_button_t button) {
    if ((unsigned int)button >= INPUT_BUTTON_COUNT) {
        return 0;
    }
    return (released_buttons & (1UL << button)) != 0;
}

uint32_t input_buttons_read() {
    return down_buttons;
}

input_slider_t input_slider_read() {
    return slider;
}

void input_inject_joystick(joystick_direction_t direction) {
    injected_direction = direction;
}

void input_inject_button(input_button_t button) {
    if ((unsigned int)button >= INPUT_BUTTON_COUNT) {
        return;
    }
    injected_buttons |= 1UL << button;
}

