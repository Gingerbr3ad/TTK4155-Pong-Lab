 
#include <util/delay.h>
#include <unistd.h>
#include <avr/io.h>

#define set_bit(reg, bit) (reg |= (1 << bit))
#define clear_bit(reg, bit) (reg &= ~(1 << bit ))
#define test_bit(reg, bit) (reg & (1 << bit))
#define loop_until_bit_is_set(reg, bit) while(!test_bit(reg, bit))
#define loop_until_bit_is_clear(reg, bit) while(test_bit(reg, bit))

#define WAVE_PIN PB1

int main(void) {
    set_bit(DDRB, WAVE_PIN);
    
    while(1) {
        set_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
        clear_bit(PORTB, WAVE_PIN);
        _delay_ms(500);
    }
}