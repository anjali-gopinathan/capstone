#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, red led) for output
    //Fan is connected to PB7
    DDRB |= 1 << DDB7;          // Set PORTB bit 7 for output
    PORTB &= ~(1 << PB7);      // Set PB7 to a 0
    flash_redled();

    while (1) {
        PORTB |= 1 << PB7;      // Set PB7 to a 1 (on)
        _delay_ms(3000);
        PORTB &= ~(1 << PB7);   // Set PB7 to a 0
        _delay_ms(1000);
        flash_redled();
    }

    return 0;   /* never reached */
}

void flash_redled(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}