#include "drivers/uart_driver.h"
#include <avr/interrupt.h> 

#define UART0_RECEIVE_INTERRUPT  USART_RXC_vect
#define BufferSize 64 
/* This uart driver implementation is based on the code examples from the offical Atmel AVR ATmega162 documentation and the AVR Libc documentation  */
volatile char uart_buffer[BufferSize]; // Buffer to store received characters
volatile uint8_t line_ready = 0; // Index for the head of the buffer
volatile uint8_t char_count = 0; // Count of characters received
void uart_init() {
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

    sei(); // Enable global interrupts
}  

int uart_putchar(char c, FILE *stream) {
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c; // UDR is the character buffer terminal for the USART device
  return 0;
}
/*This method was taken from ControllersTech AVR UART interrupt example, just that we 
implemented a method to store c in a buffer until so it becomes a string '\n'. Adresses used were from the Atmega162 datasheet
and interruot function was taken from AVR library*/
ISR(UART0_RECEIVE_INTERRUPT) {
  char received_char = UDR0; // Get the received character from the USART data register
  if(received_char == '\n') {
    
    uart_buffer[char_count] = '\0'; // Null-terminate the string

    line_ready = 1; // Set the line ready flag when a newline character is received
  } else {
  
    if(char_count < BufferSize-1) {
      uart_buffer[char_count++] = received_char; // Increment the character count and store the recivied charachters in the buffer
     
    }
  }
}

/*
int uart_getchar(FILE *stream) {
    stream = stream; // Avoid unused parameter warning

    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));

    // Get and return received data from buffer
    return UDR0;
}
*/

