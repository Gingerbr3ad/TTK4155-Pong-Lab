#include "drivers/uart_driver.h"
#include <avr/pgmspace.h>

/* This uart driver implementation is based on the code examples from the offical Atmel AVR ATmega162 documentation and the AVR Libc documentation  */
#define uart_buffer ((volatile char *)(uintptr_t)UART_RECEIVE_BUFFER_ADDR) // Buffer to store received characters
#define uart_parse_buffer ((char *)(uintptr_t)UART_PARSE_BUFFER_ADDR)
volatile uint8_t uart_recieved_flag = 0; // Index for the head of the buffer
volatile uint8_t char_count = 0; // Count of characters received

void uart_init() {
    // Since the uart buffer is placed in the external ram, we initilize it manually
    for (uint8_t i = 0; i < BufferSize; ++i) {
        uart_buffer[i] = '\0';
    }

    // Set baud rate with the values calculated by the setbaud macro
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
    // Set frame format: 8data, 2stop bit #TODO: CHECK if this is the format we want
    UCSR0C = (1<<URSEL0)|(1<<USBS0)|(3<<UCSZ00);

    // Decides to turn on Asynchronus double speed based on the setbaud macro calculations
    // If baud with 2x enable would result in smaller error it will be chosen and 2x will be enabled
    #if USE_2X
    UCSR0A |= (1 << U2X0);
    #else
    UCSR0A &= ~(1 << U2X0);
    #endif
}  

int uart_putchar(char c, FILE *stream) {
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c; // UDR is the character buffer terminal for the USART device
  return 0;
}

/* This method was taken from ControllersTech AVR UART interrupt example, just that we 
implemented a method to store c in a buffer until'\n' */
ISR(USART0_RXC_vect) {
  cli();

  char received_char = UDR0; // Get the received character from the USART data register

  if(received_char == '\n') {
    uart_buffer[char_count] = '\0'; // Null-terminate the string

    uart_recieved_flag = 1; // Set the uart recieved flag when a newline character is received
    char_count = 0; // Reset the character count for the next line
  } else {
    if(char_count < BufferSize-1) {
      uart_buffer[char_count++] = received_char; // Increment the character count and store the recivied charachters in the buffer
    }
  }

  sei();
}

int uart_getchar(FILE *stream) {
    stream = stream; // Avoid unused parameter warning

    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));

    // Get and return received data from buffer
    return UDR0;
}

uart_command_t handle_uart_interrupt() {
  uart_command_t command = UART_COMMAND_NONE;

  cli();
  // Since strcmp() can't handle a volatile string we make a copy of the character buffer
  for (uint8_t i = 0; i < BufferSize; i++) {uart_parse_buffer[i] = uart_buffer[i];}
  uart_recieved_flag = 0; // Reset the uart recieved flag
  sei();
  
  printf_P(PSTR("######################## UART RECIEVED #######################\n"));
  if(!strcmp_P(uart_parse_buffer, PSTR("C_TEST"))) {
    command = UART_COMMAND_TEST;
  } else if(!strcmp_P(uart_parse_buffer, PSTR("C_OLED_CLEAR"))) {
    command = UART_COMMAND_OLED_CLEAR;
  } else if(!strcmp_P(uart_parse_buffer, PSTR("C_OLED_CHECKERS"))) {
    command = UART_COMMAND_OLED_CHECKERS;
  } else if(!strcmp_P(uart_parse_buffer, PSTR("C_PRINT_DEBUG"))) {
    command = UART_COMMAND_PRINT_DEBUG;
  } else if(!strcmp_P(uart_parse_buffer, PSTR("C_SRAM_TEST"))) {
    command = UART_COMMAND_SRAM_TEST;
  } else if(!strcmp_P(uart_parse_buffer, PSTR("C_CAN_TEST"))) {
    command = UART_COMMAND_CAN_LOOPBACK_TEST;
  } else if (!strcmp_P(uart_parse_buffer, PSTR("C_UI_UP"))) {
    command = UART_COMMAND_UI_UP;
  } else if (!strcmp_P(uart_parse_buffer, PSTR("C_UI_DOWN"))) {
    command = UART_COMMAND_UI_DOWN;
  } else if (!strcmp_P(uart_parse_buffer, PSTR("C_UI_SELECT"))) {
    command = UART_COMMAND_UI_SELECT;
  } else if (!strcmp_P(uart_parse_buffer, PSTR("C_UI_BACK"))) {
    command = UART_COMMAND_UI_BACK;
  } else if (!strcmp_P(uart_parse_buffer, PSTR("C_GAME_OVER"))) {
    command = UART_COMMAND_GAME_FINISHED;
  }
  else {
    command = UART_COMMAND_NONE;
    printf_P(PSTR("I've recieved this from UART (Command not recognized): %s \n"), uart_parse_buffer);
  }

  return command;
}
