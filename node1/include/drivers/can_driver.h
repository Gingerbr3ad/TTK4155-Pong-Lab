#pragma once

#include "drivers/mcp2515_driver.h"

/*################# CAN MESSAGE IDs #################*/
#define TEST_MESSAGE_ID 0x123
#define JOYSTICK_MESSAGE_ID 0x100
#define SCORE_MESSAGE_ID 0x101
#define GAME_STATUS_MESSAGE_ID 0x102
/*###################################################*/


typedef struct {
    uint16_t id;
    uint8_t length;
    uint8_t data[8];
} can_message_t;

extern volatile uint8_t can_received_flag;

void can_init();
uint8_t can_send(const can_message_t *msg);
uint8_t can_receive(can_message_t *msg);

can_message_t * handle_can_interrupt();

uint8_t can_test_send();
uint8_t can_test_receive(can_message_t rx);
