#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

//motion sensor is on pin 6 (PD4)
void flash_redled();

int main(void)
{
    DDRD |= 1 << DDD2;          
    
    
    DDRD &= ~(1 << DDD4);          // Set pin for input
    
    flash_redled();
    while (1) {
        bool motion_detected = ((PIND & (1 << PD4)));

        if(motion_detected){
            PORTD |= (1 << PD2);   // Set PC0 to a 1
            _delay_ms(500);
        }
        else{
            PORTD &= ~(1 << PD2);   // Set PC0 to a 0
        }
    }

    return 0;   /* never reached */
}

void flash_redled(){
    PORTD &= ~(1 << PD2);   // Set PC0 to a 0
    _delay_ms(100);
    PORTD |= (1 << PD2);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTD &= ~(1 << PD2);   // Set PC0 to a 0
    _delay_ms(100);

}
