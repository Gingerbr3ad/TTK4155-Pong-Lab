#include "uart_driver.h"

/* This uart driver implementation is based on the code examples from the offical Atmel AVR ATmega162 documentation and the AVR Libc documentation  */

void uart_init() {
    // Set baud rate with the values calculated by the setbaud macro
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
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
  UDR0 = c; //UDR is the character buffer terminal for the USART device
  return 0;
}

int uart_getchar(FILE *stream) {
/* Wait for data to be received */
while (!(UCSR0A & (1<<RXC0)));
/* Get and return received data from buffer */
return UDR0;
}