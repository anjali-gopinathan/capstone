/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, red led) for output
    //Fan is connected to PB7
    DDRB |= 1 << DDB7;          // Set PORTB bit 7 for output
    PORTB &= ~(1 << PB7);      // Set PB7 to a 0
    flash_ledpin2();

    while (1) {
        PORTB |= 1 << PB7;      // Set PB7 to a 1 (on)
        _delay_ms(500);
        PORTB &= ~(1 << PB7);   // Set PB7 to a 0
        _delay_ms(1000);
        flash_ledpin2();
    }

    return 0;   /* never reached */
}

void flash_ledpin2(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}