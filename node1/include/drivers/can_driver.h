#pragma once

#include "drivers/mcp2515_driver.h"


typedef struct {
    uint16_t id;
    uint8_t length;
    uint8_t data[8];
} can_message_t;

void can_init();
uint8_t can_send(const can_message_t *msg);
uint8_t can_receive(can_message_t *msg);

uint8_t can_test_send();
