#include "drivers/can_driver.h"
#include "drivers/mcp2515.h"

#include <avr/pgmspace.h>


void can_init() {
    if (!mcp_init()) {
        printf_P(PSTR("CAN: MCP2515 initialization failed.\n"));
    }
}

uint8_t can_send(const can_message_t *msg) {
    // Check that the message is a valid CAN message
    if (!msg || msg->id > 0x7FF || msg->length > 8) {
        return 1;
    }

    // Make sure TXB0 is available (TXREQ = bit 3)
    if (mcp_read(MCP_TXB0CTRL) & (1 << 3)) {
        return 1;
    }

    // Configure the 11-bit identifier
    mcp_write(MCP_TXB0CTRL + 1, msg->id >> 3);
    mcp_write(MCP_TXB0CTRL + 2, (msg->id & 0x07) << 5);

    // No extended identifier
    mcp_write(MCP_TXB0CTRL + 3, 0);
    mcp_write(MCP_TXB0CTRL + 4, 0);

    // Data length
    mcp_write(MCP_TXB0CTRL + 5, msg->length);

    // Load message data
    for (uint8_t i = 0; i < msg->length; i++) {
        mcp_write(MCP_TXB0CTRL + 6 + i, msg->data[i]);
    }

    // Request transmission from TXB0
    mcp_request_to_send();

    return 0;
}

uint8_t can_receive(can_message_t *msg) {
    if (!msg) {
        return 1;
    }

    uint8_t flags = mcp_read(MCP_CANINTF);

    // Check whether either receive buffer has data
    if (!(flags & (MCP_RX0IF | MCP_RX1IF))) {
        return 1;
    }

    uint8_t rx_flag;
    uint8_t address;

    // Find in which recieve buffer the data is stored
    if (flags & MCP_RX0IF) {
        rx_flag = MCP_RX0IF;
        address = MCP_RXB0SIDH;
    } else {
        rx_flag = MCP_RX1IF;
        address = MCP_RXB1SIDH;
    }

    // Read standard 11-bit identifier
    uint8_t sidh = mcp_read(address);
    uint8_t sidl = mcp_read(address + 1);
    msg->id = ((uint16_t)sidh << 3) | (sidl >> 5);

    // If the message lenght is >8, return fail
    msg->length = mcp_read(address + 4) & 0x0F;
    if (msg->length > 8) {
        mcp_bit_modify(MCP_CANINTF, rx_flag, 0);
        return 1;
    }

    // Read the message data by iterating through the data registers
    for (uint8_t i = 0; i < msg->length; i++) {
        msg->data[i] = mcp_read(address + 5 + i);
    }

    // Clear the corresponding receive interrupt flag
    mcp_bit_modify(MCP_CANINTF, rx_flag, 0);

    return 0;
}

uint8_t can_test_send() {
    const can_message_t tx = {
        .id = 0x123,
        .length = 3,
        .data = {0xAA, 0xBB, 0xCC}
    };

    if (can_send(&tx)) {
        return 1;
    } else {
        return 0;
    }
}
