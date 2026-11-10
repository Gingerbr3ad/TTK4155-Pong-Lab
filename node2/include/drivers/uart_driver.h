
// UART is connected to the ATmega16U2 on the Arduino, which acts as a USB serial device.
// This module hooks into the stdio functions, by spoofing several system calls
//
// Unfortunately, for some unknown reason `scanf` does not work as expected. Use 
// `uart_flush` and `sscanf` instead (see example below)
//
// Internally, receiving is handled with interrupts and a ring buffer, so no characters
// are lost, until the buffer is full. If necessary, you can change this buffer size in 
// uart.c
#define BufferSize 64

extern volatile uint8_t uart_recieved_flag;

typedef enum {
    UART_COMMAND_NONE,
    UART_COMMAND_TEST,
    UART_COMMAND_CAN_TEST,
    UART_COMMAND_SHOOT,
} uart_command_t;


// Initialize. Hooks stdio functions (like `printf`)
void uart_init();
void uart_tx(uint8_t val);
uint8_t uart_rx(uint8_t* val);
int uart_flush(char* buf, int len);

uart_command_t handle_uart_interrupt();
   