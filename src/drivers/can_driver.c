#include "drivers/mcp2515_driver.h"
#include "drivers/can_driver.h"

uint8_t can_init(void) {
   if(mcp2515_init() != 0) {
      return 1;
   }


   mcp2515_write(MCP_CNF1, 0x00); // Set CNF1 register (example value), Sets the SWJ to 00 and the BRP to 0
   mcp2515_write(MCP_CNF2, 0x90);
   mcp2515_write(MCP_CNF3, 0x02); // Set CNF3 register (example value)
   mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_LOOPBACK); // Set MCP2515 to loopback mode for testing
   mcp2515_bit_modify(MCP_CANINTE, MCP_RX_INT, MCP_RX_INT); // Enable interrupts for RX0 and RX1

uint8_t value = mcp2515_read(MCP_CANSTAT);
if ((value & MODE_MASK) != MODE_LOOPBACK) {
   return 1;}
   return 0;
}

uint8_t can_send_message(can_message_t *message) {
   // Implement the logic to send a CAN message using MCP2515
   uint8_t id_high = ((*message).id >> 3) & 0xFF; // Extract the high byte of the ID
   uint8_t id_low = ((*message).id << 5) & 0xFF;
  mcp2515_write(MCP_TXB0SIDH, id_high); // Write the high byte of the ID to the TX buffer
   mcp2515_write(MCP_TXB0SIDL, id_low); // Write the low byte of the ID to the TX buffer
   if ((*message).data_length > 8) {
      return 1; // Invalid data length
   }
   else {
      mcp2515_write(MCP_TXB0DLC, (*message).data_length); // Write the data length to the TX buffer
      for (uint8_t i = 0; i < (*message).data_length; i++) {
         mcp2515_write(MCP_TXB0D0 + i, (*message).data[i]); // Write each data byte to the TX buffer
      }
   }
   mcp2515_request_to_send(1);//we are using buffer 1 to send the message
   return 0;
}

can_message_t can_receive_message(void){
   can_message_t message;
uint8_t status = mcp2515_read(MCP_CANINTF);
if(status & MCP_RX0IF) {
uint8_t id_high = mcp2515_read(MCP_RXB0SIDH);
uint8_t id_low = mcp2515_read(MCP_RXB0SIDL);

message.id = ((uint16_t)id_high << 3) | (id_low >> 5);

message.data_length = mcp2515_read(MCP_RXB0DLC) & 0x0F; // Mask to get only the data length bits
for (uint8_t i = 0; i < message.data_length; i++) {
   message.data[i] = mcp2515_read(MCP_RXB0D0 + i);
  
} mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0x00); // Clear the interrupt flag for RX0
}
else if(status & MCP_RX1IF) {
uint8_t id_high = mcp2515_read(MCP_RXB1SIDH);
uint8_t id_low = mcp2515_read(MCP_RXB1SIDL);

message.id = ((uint16_t)id_high << 3) | (id_low >> 5);

message.data_length = mcp2515_read(MCP_RXB1DLC) & 0x0F; // Mask to get only the data length bits
for (uint8_t i = 0; i < message.data_length; i++) {
   message.data[i] = mcp2515_read(MCP_RXB1D0 + i);
   
}
mcp2515_bit_modify(MCP_CANINTF, MCP_RX1IF, 0x00); // Clear the interrupt flag for RX1}
}
   return message;
}

volatile uint8_t can_received_flag = 0;
ISR(INT0_vect) {
    can_received_flag = 1;
}

void can_interrupt_handler(void) {
    if (can_received_flag) { 
        can_received_flag = 0; // Reset the flag after processing
        can_message_t received_message = can_receive_message();
        // Process the received message as needed
       printf("CAN message received:\n");
       printf("ID: %u\n", received_message.id);
         printf("Data Length: %u\n", received_message.data_length);
          printf("Data: ");
          for (uint8_t i = 0; i < received_message.data_length; i++) {
                printf("%u ", received_message.data[i]);
          }
          printf("\n");
    }
}
