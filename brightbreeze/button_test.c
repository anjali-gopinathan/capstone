#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "OnLCDLib.h"

int main(void) {
    button1_init();
    button2_init();
    button3_init();

    LCDSetup(LCD_CURSOR_BLINK);			/* Initialize LCD */
	LCDClear();			/* Clear LCD */
	LCDGotoXY(1,1);		/* Enter column and row position */

        while(1){
            bool up_pressed = ((PIND & (1 << 7)) == 0);    
            bool select_pressed = ((PINB & (1 << 1)) == 0);
            bool down_pressed = ((PINB & (1 << 2)) == 0);

            _delay_ms(500);
            
            if (up_pressed==0){ // Checks if Button1 is being pressed    
                LCDClear();
                LCDGotoXY(1,1);
                LCDWriteString("up button pressed");
                _delay_ms(200);
            }
            else if (select_pressed==0){ // Checks if Button1 is being pressed
                LCDClear();
                LCDGotoXY(1,1);
                LCDWriteString("select button pressed");
                _delay_ms(200);
            }
            else if  (down_pressed==0){ // Checks if Button1 is being pressed
                LCDClear();
                LCDGotoXY(1,1);
                LCDWriteString("down button pressed");
                _delay_ms(200);
            }
            else{
                LCDClear();
                LCDGotoXY(1,1);
                LCDWriteString("no button pressed");
                _delay_ms(200);
            }
            // if(button==1){
            //     LCDClear();
            //     LCDGotoXY(1,1);
            //     LCDWriteString("Button 1 pressed");
            //     _delay_ms(500);
            //     button=0;
            // }
            // else if (button==2){
            //     LCDClear();
            //     LCDGotoXY(1,1);
            //     LCDWriteString("Button 2 pressed");
            //     _delay_ms(500);
            //     button=0;

            // }
            // else if (button ==3){
            //     LCDClear();
            //     LCDGotoXY(1,1);
            //     LCDWriteString("Button 3 pressed");
            //     _delay_ms(500);
            //     button=0;
            // }
            // else{
            //     LCDClear();
            //     LCDGotoXY(1,1);
            //     LCDWriteString("no button pressed");
            //     _delay_ms(500);
            // }
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

// void up_pressed(){
//     bool up = ((PIND & (1 << 7))==0);
//     if (!up){
//         _delay_ms(5);
//         while( ((PIND & (1 << 7)) == 0)){}
//         _delay_ms(5); 
//         return 1;
//     }
//     else{
//         return 0;
//     }
// }

// void select_pressed(){
//     bool select = ((PINB & (1 << 1))==0);
//     if (select ==0){
//         _delay_ms(5);
//         while( ((PINB & (1 << 1)) == 0)){}
//         _delay_ms(5); 
//         return 1;
//     }
//     else{
//         return 0;
//     }
// }

// void down_pressed(){
//     bool down = ((PINB & (1 << 2))==0);
//     if (down ==0){
//         _delay_ms(5);
//         while( ((PINB & (1 << 2)) == 0)){}
//         _delay_ms(5); 
//         return 1;
//     }
//     else{
//         return 0;
//     }
// }