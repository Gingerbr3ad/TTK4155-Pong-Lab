#include "drivers/mcp2515_driver.h"
#include "drivers/spi_driver.h"

#define MCP_SELECT select_slave(mcp2515_ss)
#define MCP_DESELECT deselect_slave(mcp2515_ss);

//TODO: Upgrade this with sequential spi writes to reduce spi bandwidth

uint8_t mcp_read(uint8_t address) {
    MCP_SELECT;

    spi_write(MCP_READ);
    spi_write(address);
    uint8_t value = spi_read();

    MCP_DESELECT;

    return value;
}

void mcp_write(uint8_t address, uint8_t value) {
    MCP_SELECT;

    spi_write(MCP_WRITE);
    spi_write(address);
    spi_write(value);

    MCP_DESELECT;
}

void mcp_request_to_send() {
    MCP_SELECT;

    // REMEMBER THAT THIS IS ONLY FOR TX0
    spi_write(MCP_RTS_TX0);

    MCP_DESELECT;
}

uint8_t mcp_read_status() {
    MCP_SELECT;

    uint8_t status;
    spi_write(MCP_READ_STATUS);
    status = spi_read();

    MCP_DESELECT;

    return status;
}

void mcp_bit_modify(uint8_t address, uint8_t mask, uint8_t value) {
    MCP_SELECT;

    spi_write(MCP_BITMOD);
    spi_write(address);
    spi_write(mask);
    spi_write(value);

    MCP_DESELECT;
}

void mcp_reset() {
    MCP_SELECT;

    spi_write(MCP_RESET);

    MCP_DESELECT;
}

uint8_t mcp_init() {
    // Configure CS and INT pins
    DDRB |= (1 << PB0);
    MCP_DESELECT;

    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2); // Enable pull-up

    // Software reset
    mcp_reset();

    _delay_ms(10);

    // Verify configuration mode
    if ((mcp_read(MCP_CANSTAT) & MODE_MASK) != MODE_CONFIG) {
        return 0;
    }

    // CAN timing: 125 kbps with 16 MHz crystal
    mcp_write(MCP_CNF1, 0x03);
    mcp_write(MCP_CNF2, 0xAC);
    mcp_write(MCP_CNF3, 0x03);

    // Verify setup
    if (mcp_read(MCP_CNF1) != 0x03 ||
        mcp_read(MCP_CNF2) != 0xAC ||
        mcp_read(MCP_CNF3) != 0x03) {
        return 0;
    }

    // Accept all messages, enable RXB0 rollover
    mcp_write(MCP_RXB0CTRL, 0x64);
    mcp_write(MCP_RXB1CTRL, 0x60);

    // Clear pending interrupt flags
    mcp_write(MCP_CANINTF, 0x00);

    // Enable RX0 and RX1 interrupts
    mcp_write(MCP_CANINTE, 0x03);

    // Request loopback mode
    mcp_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);

    // Verify successful mode transition
    for (uint8_t i = 0; i < 20; i++) {
        uint8_t mode = mcp_read(MCP_CANSTAT) & MODE_MASK;

        if (mode == MODE_NORMAL) {
            return 1;
        }

        _delay_ms(1);
    }

    return 0;
}
