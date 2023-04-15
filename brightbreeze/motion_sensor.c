#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>


#include "OnLCDLib.h"
 


//motion sensor is on pin 6 (PD4)
void motion_sensor_init();


int main(void)
{    
    motion_sensor_init();
    LCDSetup(LCD_CURSOR_BLINK);
    LCDHome();
    LCDClear();
    //LCDWriteString("hello");


    
    while (1) {
        bool motion_detected = ((PIND & (1 << PD4)));
        //_delay_ms(10);
        if(motion_detected){
            //delay_ms(100);
            //_delay_ms(50);
            //LCDClear();
            LCDGotoXY(1, 1);
            LCDWriteString("                  ");
            LCDGotoXY(1, 1);
            LCDWriteString("motion detected");
        }
        else{
            //_delay_ms(100);
            //_delay_ms(50);
            //LCDClear();
            LCDGotoXY(1, 1);
            LCDWriteString("                  ");
            LCDGotoXY(1, 1);
            LCDWriteString("no motion detected");
        }
        _delay_ms(10);

        
        
    }

    

    return 0;   /* never reached */
}

void motion_sensor_init(){
    DDRD &= ~(1 << DDD4);          // Set pin for input
}






