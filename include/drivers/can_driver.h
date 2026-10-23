#ifndef CAN_H
#define CAN_H

#include <stdint.h>
extern volatile uint8_t can_received_flag;
typedef struct {
    uint32_t id; // CAN ID
    uint8_t data_length; // Data Length Code
    uint8_t data[8]; // Data bytes
} can_message_t;

uint8_t can_init(void);
uint8_t can_send_message(can_message_t *message);
can_message_t can_receive_message(void);
void can_interrupt_handler(void);

#endif