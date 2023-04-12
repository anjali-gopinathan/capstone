#include <avr/io.h>
#include <util/delay.h>
#include "OnLCDLib.h"

int main(void)
{
    //Fan is connected to PB7
    LCDSetup(LCD_CURSOR_BLINK);
    // flash_redled();
    // uint8_t seconds = 0;
    // uint8_t minutes = 0;
    while(1){
        LCDHome();
        LCDClear();
        LCDWriteString("Hello world");

        // seconds += 1;
        _delay_ms(1000);
    }
    return 0;   /* never reached */
}
