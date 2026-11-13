#include "sam.h"

#include "application/score.h"
#include "drivers/adc_driver.h"
#include "drivers/can_driver.h"

static volatile uint32_t score;
static volatile uint8_t playing;
static volatile uint8_t score_pending;
static volatile uint8_t ticks;
static uint8_t beam_armed;

void score_start() {
    __disable_irq();
    score = 0;
    ticks = 0;
    score_pending = 0;
    playing = 1;
    beam_armed = 0;
    __enable_irq();
}

void score_stop() {
    __disable_irq();
    playing = 0;
    score_pending = 0;
    __enable_irq();
}

void score_system_tick() {
    if (!playing) {return;}

    uint8_t beam_clear = adc_state();

    // Wait until the beam has been clear for two samples
    if (!beam_armed) {
        if (beam_clear) {beam_armed = 1;}
        return;
    }

    // Detect a goal
    if (!beam_clear) {
        playing = 0;
        score_pending = 1;
        return;
    }

    // Update score every second
    if (++ticks >= 60) {
        ticks = 0;
        score += 50;
        score_pending = 1;
    }
}

void score_send_pending() {
    __disable_irq();
    uint8_t pending = score_pending;
    uint32_t value = playing ? score : 0;
    score_pending = 0;

    __enable_irq();

    if (!pending) {return;}

    CanMsg message = {
        .id = SCORE_MESSAGE_ID,
        .length = 4
    };

    message.byte[0] = value;
    message.byte[1] = value >> 8;
    message.byte[2] = value >> 16;
    message.byte[3] = value >> 24;

    can_send(message);
}