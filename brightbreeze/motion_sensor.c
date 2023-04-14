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
    LCDWriteString("hello");


    
    while (1) {
        bool motion_detected = ((PIND & (1 << PD4)));
        if(motion_detected){
            _delay_ms(500);
            LCDWriteString("motion detected");
        }
        else{
            LCDWriteString("no motion detected");
        }
        
    }

    return 0;   /* never reached */
}

void motion_sensor_init(){
    DDRD &= ~(1 << DDD4);          // Set pin for input

}
