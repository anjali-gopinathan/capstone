#include <avr/io.h>
#include <util/delay.h>
#include "OnLCDLib.h"

#define STATE_INIT 0;
#define STATE_SCROLL 1;
const char* menulist[] = {      //first character of each line to be replaced by ">" character
    "********************", // this line represents 20 characters. after that it will scroll (200ms). see LCD_SCROLL_SPEED variable in OnLCDLib.h.
    "  BrightBreeze menu ", 
    "  MM/DD/YY HH:MM [set]",   //24 hour time? 12 hour time and do am/pm?. this option can take you to the next screen
    "  DD deg F IN [set]",
    "  DD deg F OUT",
    "  Man. toggle window",
    "  Man. toggle blinds",
    "  Man. toggle fan",
    "  ||ADVANCED||",
    "  Brightness IN:  DIM",
    "  Brightness OUT: BRIGHT"
}
uint8_t numMenuOptions = sizeof(menulist)/sizeof(menulist[0]);    

int selectedoption = 0;     // where the caret is. note that this is 0-indexed, but the LCD lines are 1-indexed
int main(void)
{

    //Fan is connected to PB7
    LCDSetup(LCD_CURSOR_BLINK);
    // flash_redled();
    // uint8_t seconds = 0;
    // uint8_t minutes = 0;
    LCDClear();
    
    while(1){
        LCDHome();
        if(needToUpdateMenu)
        uint8_t menu_startidx = 1;
        for(uint8_t i = menu_startidx; i<=(menu_startidx+3); i++){  //i is menu index
            if(i < numMenuOptions){
                uint8_t row = i - menu_startidx + 1;
                LCDGotoXY(1, row);
                LCDClearLine(row);
                // LCDWriteString(">")
                LCDWriteString(menulist[i]);

            }
        }
        LCDWriteString("BrightBreeze menu");    
        LCDGotoXY(1,1)
        // seconds += 1;
        _delay_ms(1000);
    }
    return 0;   /* never reached */
}
void LCDClearLine(uint8_t y){
    LCDWriteString("                    ");
    LCDGotoXY(1, y);

}
void moveSelector(uint8_t fromRow, uint8_t toRow);