#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "OnLCDLib.h"
#include "i2c_functions.h"
#include "temp_sensor.h"
#include "light_sensors.h"
#define STATE_INIT 0;
#define STATE_SCROLL 1;
// const char* BLANK_20CHAR = "                    ";



void LCDClearLine(uint8_t y){
    LCDWriteString(BLANK_20CHAR);
    LCDGotoXY(1, y);
}
int main(void)
{
    char* menulist[] = {      //first character of each line to be replaced by ">" character
        //     "********************", // this line is 20 characters. after that it will scroll (200ms). see LCD_SCROLL_SPEED variable in OnLCDLib.h.
        /*0*/  "-BrightBreeze menu-",   //title row doesn't display actual content or do anything
        /*1*/  "  __ deg F IN",   
        /*2*/  "  __ deg F OUT",
        /*3*/  "  Brighter: [in/out]",
        /*4*/  "  View/set date/time",   //has another screen
        /*5*/  "  Set target temp",
        /*6*/  "  Set wakeup time",      //has another screen
        /*7*/  "  Man. toggle window",
        /*8*/  "  Man. toggle blinds",
        /*9*/  "  Man. toggle fan"
    };
    uint8_t numMenuOptions = sizeof(menulist)/sizeof(menulist[0]);    
    temp_sensor_init();
    light_sensor_init();
    int hoveredoption = 0;
    int selectedoption = -1;    // selected option = -1 means nothing has been selected
    int caretRow = 1;
    
    //Fan is connected to PB7
    LCDSetup(LCD_CURSOR_BLINK);
    
    LCDClear();
    
    bool menu_is_idle = false;
    uint8_t i;
    uint8_t menu_startidx = 0;
    uint16_t outside_brightness;
    uint16_t inside_brightness;
    int brightness_status;
    while(1){
        bool up_pressed = ((PIND & (1 << 7)) == 0);    
        bool select_pressed = ((PINB & (1 << 1)) == 0);
        bool down_pressed = ((PINB & (1 << 2)) == 0);
        _delay_ms(500);

        LCDHome();
        if(!menu_is_idle){
            //uint8_t menu_startidx = 0;
            // uint8_t i;
            for(i = menu_startidx; i<=(menu_startidx+3); i++){  //i is menu index
                if(i < numMenuOptions){
                    uint8_t row = i - menu_startidx + 1;
                    LCDGotoXY(1, row);
                    LCDClearLine(row);
                    // LCDWriteString(">")
                    if(i == 1){ //menu index 1 is inside temp
                        sprintf(menulist[i], "  %2d deg F IN     ", get_Temp(1));
                    }
                    else if(i == 2){    //menu index 2 is outside temp
                        sprintf(menulist[i], "  %2d deg F OUT    ", get_Temp(0));
                    }
                    else if(i == 3){    //menu index 3 is brighter: in/out
                        // 0 means same, 1 means inside brighter, -1 means outside brighter

                        //channel 0 is outside, channel 1 is inside
                        outside_brightness= get_light_values(0);
                        inside_brightness= get_light_values(1);
                        brightness_status = get_lightStatus(outside_brightness, inside_brightness);
                        LCDGotoXY(1,4);
                        LCDClearLine(4);
                        LCDWriteInt(brightness_status,1);
                        // if(brightness_status == 1){
                        //     sprintf(menulist[i], "  Brighter: IN");
                        // }
                        // else if(brightness_status == -1){
                        //     sprintf(menulist[i], "  Brighter: OUT");
                        // }
                        // else if(brightness_status == 0){
                        //     sprintf(menulist[i], "  Brightness: SAME");
                        // }
                    }
                    else if(i==4){
                        sprintf(menulist[i], "  View/set date/time");
                    }
                    else if(i==5){
                        sprintf(menulist[i], "  Set target temp");
                    }
                    else if(i==6){
                        sprintf(menulist[i], "  Set wakeup time");
                    }
                    else if(i==7){
                        sprintf(menulist[i], "  Man. toggle window");
                    }
                    else if(i==8){
                        sprintf(menulist[i], "  Man. toggle blinds");
                    }
                    else if(i==9){
                        sprintf(menulist[i], "  Man. toggle fan");
                    }
                    LCDWriteString(menulist[i]);

                }
            }
        

            if(!down_pressed && (hoveredoption < numMenuOptions-1)){ // if down pressed and there's still room on menu to go down
                hoveredoption++;
                if(caretRow < 4){   // if it's row 1 2 or 3. (row 4 means leave the caret on row 4 and scroll the screen)
                    caretRow++;
                }
                else{
                    menu_startidx++;
                    // debugging 
                    // LCDGotoXY(1,1);
                    // LCDClearLine(1);
                    // LCDWriteInt(menu_startidx,1);
                    // LCDWriteInt(hoveredoption,1);
                }
            }
            else if(!up_pressed && (hoveredoption > 1)){  //if up pressed and there's still room on menu to go up
                hoveredoption--;
                if(caretRow > 1){
                    caretRow--;
                }
                else{
                    menu_startidx--;
                }
            }
            else if(!select_pressed){
                selectedoption = hoveredoption;
                // call function for each option
            }

            // if(selectedoption == ){
            //     // do nothing

            // }

            LCDGotoXY(1,caretRow);
            LCDWriteString(">");
        }


        // standard state machine for adjusting blinds and windows here
        _delay_ms(1000);
    }
    return 0;   /* never reached */
}
