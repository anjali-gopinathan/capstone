#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
// #include <time.h>

#include "OnLCDLib.h"
#include "i2c_functions.h"
#include "temp_sensor.h"
#include "light_sensors.h"
#include "real_time_clock.h"

// #define STATE_INIT 0;
// #define STATE_SCROLL 1;
// const char* BLANK_20CHAR = "                    ";
char buffer[20];


int hoveredoption = 0;
int selectedoption = -1;    // selected option = -1 means nothing has been selected
int caretRow = 1;

bool menu_is_idle = false;
uint8_t i;
uint8_t menu_startidx = 0;
uint16_t outside_brightness;
uint16_t inside_brightness;
int brightness_status;


void setTimeScreen();
void setTemp();
void mainMenu();

bool up_pressed ()     { return ((PIND & (1 << 7)) != 0);}
bool select_pressed () { return ((PINB & (1 << 1)) != 0);}
bool down_pressed ()   { return ((PINB & (1 << 2)) != 0);}

void LCDClearLine(uint8_t y){
    LCDWriteString(BLANK_20CHAR);
    LCDGotoXY(1, y);
}
int main(void)
{
     
    temp_sensor_init();
    light_sensor_init();

    //for RTC
    i2c_init(BDIV);
    RTC_Write_Time(11, 35, 56); //1002 hard coding curr time

    
    //Fan is connected to PB7
    LCDSetup(LCD_CURSOR_NONE);
    
    LCDClear();

    LCDHome();
    
    //mainMenu();
    char* menulist[] = {      //first character of each line to be replaced by ">" character
        //     "********************", // this line is 20 characters. after that it will scroll (200ms). see LCD_SCROLL_SPEED variable in OnLCDLib.h.
        /*0*/  " -BrightBreeze menu-",   //title row doesn't display actual content or do anything
        /*1*/  "  __ deg F IN       ",   
        /*2*/  "  __ deg F OUT      ",
        /*3*/  "  Brighter: [in/out]",
        /*4*/  "  Set time          ",   //has another screen
        /*5*/  "  Set target temp   ",
        /*6*/  "  Set wakeup time   ",      //has another screen
        /*7*/  "  Set bedtime       ", // Man. toggle window
        // /*8*/  "  Man. toggle blinds",
        // /*9*/  "  Man. toggle fan   "
    };
    uint8_t numMenuOptions = sizeof(menulist)/sizeof(menulist[0]);    
    temp_sensor_init();
    light_sensor_init();

    //for RTC
    i2c_init(BDIV);
    RTC_Write_Time(11, 35, 56); //1002 hard coding curr time



    
    while(1){

        mainMenu();


        // standard state machine for adjusting blinds and windows here
        _delay_ms(1000);
    }
    return 0;   /* never reached */
}

void setTimeScreen(){
    //Display on LCD row 1 Current time: 12:34 PM   (HH:MM)
    //Cursor should be on lsb
    //Pressing up/down should increment/decrement the time
    //Select will set the time for that digit and move to next most significant digit
    //If the cursor is on the msb, select will set the time and return to the main menu
    // LCDClear();
    uint8_t i=0;
    for(i=1; i<=4; i++){    //clear all rows
        LCDClearLine(i);
    }
    LCDHome();
    while(1){
        RTC_Read_Clock(0);
        uint8_t hr = bcd2decimal((hour & 0b00011111));
        uint8_t min = bcd2decimal(minute);
        char amOrPm = 'P';
        if (hour & TimeFormat12){
            if(IsItPM(hour)) amOrPm = 'P';
            else amOrPm = 'A';
        }
        sprintf(buffer, "Curr time: %02d:%02d %cM ", hr, min, amOrPm);
        LCDHome();
        LCDWriteString(buffer);
        LCDGotoXY(2,3);
        LCDWriteString("Go back");
        //Curr time: HH:MM AM 

        LCDGotoXY(16,1);    // minutes
        bool settingMin = true, settingHour = false, settingAMPM = false;
        if(up_pressed()){
            if(settingMin){
                if(min == 59) min = 0;
                else min++;
            }
            else if(settingHour){
                if(hr == 12) hr = 1;
                else hr++;
            }
            else if(settingAMPM){
                if(amOrPm == 'A') amOrPm = 'P';
                else amOrPm = 'A';
            }
        }
        else if(down_pressed()){
            if(settingMin){
                if(min == 0) min = 59;
                else min--;
            }
            else if(settingHour){
                if(hr == 1) hr = 12;
                else hr--;
            }
            else if(settingAMPM){
                if(amOrPm == 'A') amOrPm = 'P';
                else amOrPm = 'A';
            }
        }
        else if(select_pressed()){
            
            if(settingMin){ // if done min, move to hours
                settingMin = false;
                settingHour = true;
                settingAMPM = false;
                LCDGotoXY(13,1);    // hours ones digit
            }
            else if(settingHour){   // if done hours, move to AM/PM
                settingHour = false;
                settingMin = false;
                settingAMPM = true;
            }
            else if(settingAMPM){
                //if select is pressed while we're setting AM/PM, we're done setting the time!
                settingAMPM = false;
            }           
        }
        
        if(down_pressed()&&(caretRow==1)){ // if down pressed and there's still room on menu to go down
            caretRow--;
        }
        if(up_pressed()&&(caretRow==2)){
            caretRow++;
        }
        
        if((caretRow==2)&&(select_pressed())){
            // go back call menu function
            mainMenu();
            //return; // leave function 
        }
        LCDGotoXY(1,caretRow);
        LCDWriteString(">");
    }
}


void setTargetTemp(){
    //LCDClear();
    uint8_t i=0;
    for(i=1; i<=4; i++){    //clear all rows
        LCDClearLine(i);
    }

    LCDHome();
    LCDGotoXY(1, 3);
    int temp = get_Temp(1); // gets current inside temperature for user to adjust 
    LCDWriteString("Set target temp: ");
    LCDWriteInt(temp, 3); 
    LCDGotoXY(2,3);
    LCDWriteString("Go back");

    // int hoveredoption = 0;
    // int selectedoption = -1;    

    while(1){
        if ((!select_pressed())&&(caretRow==1)){
            if(up_pressed()){
                temp++;
            }
            else if ((down_pressed())&&(caretRow==1)){
                temp--;
            }
            LCDGotoXY(1,18);
            LCDWriteInt(temp,2);
        }
        
        if(down_pressed()&&(caretRow==1)){ // if down pressed and there's still room on menu to go down
            caretRow--;
        }
        if(up_pressed()&&(caretRow==2)){
            caretRow++;
        }
        
        if((caretRow==2)&&(select_pressed())){
            // go back call menu function
            mainMenu();
            //return; // leave function 
        }
        LCDGotoXY(1,caretRow);
        LCDWriteString(">");
    }
}

void mainMenu(){

    //  while(1){
         if(!menu_is_idle){
            //uint8_t menu_startidx = 0;
            // uint8_t i;
            for(i = menu_startidx; i<=(menu_startidx+3); i++){  //i is menu index
                if(i < numMenuOptions){
                    uint8_t row = i - menu_startidx + 1;
                    LCDGotoXY(1, row);
                    // LCDWriteString(">")
                    if(i == 1){ //menu index 1 is inside temp
                        sprintf(menulist[i], "  %2d deg F IN       ", get_Temp(1));
                    }
                    else if(i == 2){    //menu index 2 is outside temp
                        sprintf(menulist[i], "  %2d deg F OUT      ", get_Temp(0));
                    }
                    else if(i == 3){    //menu index 3 is brighter: in/out
                        // 0 means same, 1 means inside brighter, -1 means outside brighter

                        //channel 0 is outside, channel 1 is inside
                        outside_brightness= get_light_values(0);
                        inside_brightness= get_light_values(1);
                        brightness_status = get_lightStatus(outside_brightness, inside_brightness);
                        if(brightness_status == 1){
                            sprintf(menulist[i], "  Brighter: IN      ");
                        }
                        else if(brightness_status == -1){
                            sprintf(menulist[i], "  Brighter: OUT     ");
                        }
                        else {//if(brightness_status == 0){
                            sprintf(menulist[i], "  Brightness: SAME  ");
                        }
                    }
                    
                    LCDWriteString(menulist[i]);

                }
            }
        

            if(down_pressed() && (hoveredoption < numMenuOptions-1)){ // if down pressed and there's still room on menu to go down
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
            else if(up_pressed() && (hoveredoption > 1)){  //if up pressed and there's still room on menu to go up
                hoveredoption--;
                if(caretRow > 1){
                    caretRow--;
                }
                else{
                    menu_startidx--;
                }
            }
            else if(select_pressed()){
                selectedoption = hoveredoption;
                // call function for each option
                if (selectedoption==1){
                    
                }
                else if (selectedoption==2){
     
                }
                else if (selectedoption==3){
                    
                }
                else if (selectedoption==4){
                    setTimeScreen();
                }
                else if (selectedoption==5){
                    // set target temp 
                    setTargetTemp();
                    // now compare target temp with current temp and decide if to open a window
                }
                else if (selectedoption==6){
                    
                }
                else if (selectedoption==7){
                    
                }
                else if (selectedoption==8){
                    
                }
                else if (selectedoption==9){
                    
                }
            }

            // if(selectedoption == ){
            //     // do nothing

            // }

            LCDGotoXY(1,caretRow);
            LCDWriteString(">");
        }
    //  }
}

