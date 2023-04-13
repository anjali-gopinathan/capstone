#include <avr/io.h>
#include <util/delay.h>

#include "OnLCDLib.h"

int main(void) {
        button1_init();
        button2_init();
        button3_init();

        LCDSetup(LCD_CURSOR_BLINK);			/* Initialize LCD */
	    LCDClear();			/* Clear LCD */
	    LCDGotoXY(1,1);		/* Enter column and row position */

        while(1){
            //LCDHome();
            _delay_ms(500);
            if ((PIND & (1 << PD7)) ){ // Checks if Button1 is being pressed
                LCDClear();
                LCDGotoXY(1,1);
                LCDWriteString("Button 1 pressed");
                _delay_ms(500);
            }
            // else if ((PINB & (1 << PB1)) ){ // Checks if Button1 is being pressed
            //     LCDClear();
            //     LCDGotoXY(1,1);
            //     LCDWriteString("Button 2 pressed");
            //     _delay_ms(500);
            // }
            // else if ((PINB & (1 << PD2)) ){ // Checks if Button1 is being pressed
            //     LCDClear();
            //     LCDGotoXY(1,1);
            //     LCDWriteString("Button 3 pressed");
            //     _delay_ms(500);
            // }
            else{
                LCDClear();
                LCDGotoXY(1,1);
                LCDWriteString("no button pressed");
                _delay_ms(500);
            }
        }

    return 0;   /* never reached */
}


void button1_init(){
    // button1 is on pin13 (PD7)
    DDRD &= ~(1 << 7);   // Set pin for input
    PORTD |= (1 << 7); //Enable internal pullup
    //PORTD = 0b10000000;
}

void button2_init(){
    // button2 is on pin15 (PB1)
    DDRB &= ~(1 << 1);   // Set pin for input
    PORTB |= (1 << 1); //Enable internal pullup
}

void button3_init(){
    // button3 is on pin16 (PB2)
    DDRB &= ~(1 << 2);   // Set pin for input
    PORTB |= (1 << 2); //Enable internal pullup
}
