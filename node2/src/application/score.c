#include "sam.h"

#include "application/score.h"
#include "drivers/adc_driver.h"
#include "drivers/can_driver.h"

static volatile uint32_t score;
static volatile uint8_t playing;
static volatile uint8_t score_pending;
static volatile uint8_t ticks;

void score_start() {
    __disable_irq();
    score = 0;
    ticks = 0;
    score_pending = 0;
    playing = 1;
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

    if (!adc_state()) {
        playing = 0;
        score_pending = 1;
    } else if (++ticks == 60) {
        ticks = 0;
        score += 50;
        score_pending = 1;
    }
}

void score_send_pending() {
    __disable_irq();
    uint8_t pending = score_pending;
    uint32_t current_score = score;
    uint8_t game_over = !playing;
    score_pending = 0;
    __enable_irq();

    if (!pending) {return;}

    CanMsg message = {.id = SCORE_MESSAGE_ID, .length = 4};
    if (current_score) {
        message.byte[0] = current_score;
        message.byte[1] = current_score >> 8;
        message.byte[2] = current_score >> 16;
        message.byte[3] = current_score >> 24;
        can_send(message);
    }
    if (game_over) {
        message.byte[0] = 0;
        message.byte[1] = 0;
        message.byte[2] = 0;
        message.byte[3] = 0;
        can_send(message);
    }
}
