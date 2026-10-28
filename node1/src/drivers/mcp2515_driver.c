#include "drivers/mcp2515_driver.h"
#include "drivers/spi_driver.h"

#include <stdint.h>

uint8_t mcp2515_init () {
uint8_t value;
spi_init (); // Initialize SPI
mcp2515_reset(); // Send reset - command
// Self-test
value = mcp2515_read(MCP_CANSTAT);
if (( value & MODE_MASK ) != MODE_CONFIG ) {
printf (" MCP2515 is NOT in configuration mode after reset !\n" );
return 1;
}
return 0;
}

uint8_t mcp2515_read(uint8_t address){
   
    uint8_t message[] = {MCP_READ, address, 0x00};
    uint8_t recivied[sizeof(message)];
    spi_wr(message, sizeof(message), can_ss, recivied);
    return recivied[sizeof(recivied) - 1];
}

void mcp2515_write(uint8_t address, uint8_t data){
 uint8_t message[] = {MCP_WRITE, address, data};
 spi_w(message, sizeof(message), can_ss);

}
;
void mcp2515_request_to_send(uint8_t buffer){
    uint8_t Buffers[] = {MCP_RTS_TX0, MCP_RTS_TX1, MCP_RTS_TX2};
    spi_w(&Buffers[buffer-1], 1, can_ss);
};
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
    uint8_t bit_mod[] = {MCP_BITMOD, address, mask, data};
    spi_w(bit_mod, sizeof(bit_mod), can_ss);
};

void mcp2515_reset(void){
    uint8_t reset[] = {MCP_RESET};
   spi_w(reset, 1, can_ss);
};
uint8_t mcp2515_read_status(void){
    uint8_t message[] = {MCP_READ_STATUS, 0x00};
    uint8_t received[sizeof(message)];
    spi_wr(message, sizeof(message), can_ss, received);
    return received[sizeof(received) - 1];
};
