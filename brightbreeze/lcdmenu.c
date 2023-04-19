#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//LCD Library
#include "OnLCDLib.h"

//Our libraries
#include "i2c_functions.h"
#include "light_sensors.h"
#include "temp_sensor.h"
#include "real_time_clock.h"
#include "fan.h"
#include "motion_sensor.h"

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

//for setTimeScreen
uint8_t hr, min;
uint8_t bedtime_hr, bedtime_min, wakeup_hr, wakeup_min ;
char amOrPm;
char bedtime_amOrPm, wakeup_amOrPm;
char bedtime_amOrPm, wakeup_amOrPm;
bool settingMin, settingHour, settingAMPM, settingTime;//, init_timeScreen = false;

uint8_t target_temp;

void setTimeScreen( uint8_t *hr, uint8_t *min, char *amOrPm);
void setTargetTemp();
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

    //for Real Time Clock
    i2c_init(BDIV);
    RTC_Write_Time(12, 00, 00); //initial curr time is 12:00:00
    
    //get the current time
    RTC_Read_Clock(0);
    hr = bcd2decimal((hour & 0b00011111));
    min = bcd2decimal(minute);
    if (hour & TimeFormat12){
        if(IsItPM(hour)) amOrPm = 'P';
        else amOrPm = 'A';
    }
    else{amOrPm='A';}   //hardcoded to AM
    
    bedtime_hr = hr;
    bedtime_min = min;
    bedtime_amOrPm = amOrPm;

    wakeup_hr = hr;
    wakeup_min = min;
    wakeup_amOrPm = amOrPm;

    target_temp = 75;


    //Fan is connected to PB7
    LCDSetup(LCD_CURSOR_NONE);
    
    LCDClear();

    LCDHome();

    while(1){
        mainMenu();
        // standard state machine for adjusting blinds and windows here
        _delay_ms(1000);
    }
    return 0;   /* never reached */
}

void setTimeScreen( uint8_t *hr, uint8_t *min, char *amOrPm){  //1002 maybe change
    //Display on LCD row 1 Current time: 12:34 PM   (HH:MM)
    //Cursor should be on lsb
    //Pressing up/down should increment/decrement the time
    //Select will set the time for that digit and move to next most significant digit
    //If the cursor is on the msb, select will set the time and return to the main menu
    LCDClear();
    
    
    //make a copy of each of these variables
    uint8_t  hr_userSet, min_userSet;
    char amOrPm_userSet;
    
    hr_userSet = *hr;
    min_userSet = *min;
    amOrPm_userSet = *amOrPm;
 
    int setTime_caretRow = 2;
    settingMin = false; 
    
    settingHour = false; 
    settingAMPM = false;
    settingTime = (settingMin || settingHour || settingAMPM);
    

    sprintf(buffer, "  Curr time: %02u:%02u%cM", *hr, *min, *amOrPm);
    //  Curr time: HH:MMAM
    LCDHome();
    LCDWriteString(buffer);
    LCDGotoXY(1,2);
    sprintf(buffer, "  Set time:  %02u:%02u%cM", hr_userSet, min_userSet, amOrPm_userSet);
    //  Set time:  HH:MMAM
    LCDWriteString(buffer);
    LCDGotoXY(1,3);
    LCDWriteString("  Go back           ");

    // bool isRunning = true;
    while(1){
        //update curr time on the first row
        sprintf(buffer, "  Curr time: %02u:%02u%cM", *hr, *min, *amOrPm);
        LCDHome();
        LCDWriteString(buffer);
        //done updating curr time on first row
        //update caret print
        LCDGotoXY(1,setTime_caretRow);
        LCDWriteString(">");
        
        if(setTime_caretRow == 2){
            _delay_ms(1000);
            if(down_pressed()){ // if down pressed and there's still room on menu to go down
                setTime_caretRow++;
                LCDGotoXY(1,setTime_caretRow-1);
                LCDWriteString(" ");
            }
            else if(select_pressed()){
                settingTime = true;
                settingMin = true;
            }         
        }
        
        else if(setTime_caretRow == 3){
            _delay_ms(1000);
            if(up_pressed()){
                setTime_caretRow--;
                LCDGotoXY(1,setTime_caretRow+1);
                LCDWriteString(" ");
            }
            
            else if(select_pressed()){
                LCDGotoXY(1,setTime_caretRow);
                LCDWriteString(" ");
                setTime_caretRow=1;
                break;
            }
        }

        while(settingTime){
            _delay_ms(1000);

            // update set time on second row
            sprintf(buffer, "  Set time:  %02u:%02u%cM", hr_userSet, min_userSet, amOrPm_userSet);
            //  Set time:  HH:MMAM
            LCDGotoXY(1,2);
            LCDWriteString(buffer);

            LCDGotoXY(17,2);    // minutes
            if(up_pressed()){
                if(settingMin){
                    if(min_userSet == 59) min_userSet = 0;
                    else min_userSet++;
                }
                else if(settingHour){
                    if(hr_userSet == 12) hr_userSet = 1;
                    else hr_userSet++;

          
                }
                else if(settingAMPM){
                    if(amOrPm_userSet == 'A') amOrPm_userSet = 'P';
                    else amOrPm_userSet = 'A';

                }
            }
            else if(down_pressed()){
                if(settingMin){
                    if(min_userSet == 0) min_userSet = 59;
                    else min_userSet--;
                }
                else if(settingHour){
                    if(hr_userSet == 1) hr_userSet = 12;
                    else hr_userSet--;
                }
                else if(settingAMPM){
                    if(amOrPm_userSet == 'A') amOrPm_userSet = 'P';
                    else amOrPm_userSet = 'A';
                }
            }
            else if(select_pressed()){
                
                if(settingMin){ // if done min, move to hours
                    settingMin = false;
                    settingHour = true;
                }
                else if(settingHour){   // if done hours, move to AM/PM
                    LCDGotoXY(14,2);    // hours ones digit
                    settingHour = false;
                    settingAMPM = true;
                }
                else if(settingAMPM){
                    //if select is pressed while we're setting AM/PM, we're done setting the time!
                    LCDGotoXY(18,1);    // AM/PM
                    settingAMPM = false;
                    settingTime = false;
                    setTime_caretRow = 2;
                    break;
                }         
            }
        }
        *hr = hr_userSet;
        *min = min_userSet;
        *amOrPm = amOrPm_userSet;

    }

    _delay_ms(500);
}


void setTargetTemp(){
    LCDClear();
    
    //make a copy of each of these variables
    int setTime_caretRow = 2;
    bool settingTemp = false; 

    sprintf(buffer, "  Curr temp: %02u", get_Temp(1));
    LCDHome();
    LCDWriteString(buffer);
    LCDGotoXY(1,2);
    sprintf(buffer, "  Target temp: %02u", target_temp);

    LCDWriteString(buffer);
    LCDGotoXY(1,3);
    LCDWriteString("  Go back           ");

    // bool isRunning = true;
    while(1){
        //update caret print
        LCDGotoXY(1,setTime_caretRow);
        LCDWriteString(">");
        
        if(setTime_caretRow == 2){
            _delay_ms(1000);
            if(down_pressed()){ // if down pressed and there's still room on menu to go down
                LCDGotoXY(1,setTime_caretRow);
                LCDWriteString(" ");
                setTime_caretRow++;
            }
            else if(select_pressed()){
                settingTemp = true;
            }         
        }
        
        else if(setTime_caretRow == 3){
            _delay_ms(1000);
            if(up_pressed()){
                setTime_caretRow--;
                LCDGotoXY(1,setTime_caretRow+1);
                LCDWriteString(" ");
            }
            
            else if(select_pressed()){
                LCDGotoXY(1,setTime_caretRow);
                LCDWriteString(" ");
                break;
            }
        }

        while(settingTemp){
            _delay_ms(1000);

            // update set temp on second row
            sprintf(buffer, "  Target temp: %02u", target_temp);
            LCDGotoXY(1,2);
            LCDWriteString(buffer);

            LCDGotoXY(16,2);    // minutes
            if(up_pressed()){
                target_temp ++;
            }
            else if(down_pressed()){
               target_temp--;
            }
            else if(select_pressed()){
                setTime_caretRow = 2;
                settingTemp = false;
                break;        
            }
        }

    }

    _delay_ms(500);
}


void mainMenu(){
    char* menulist[] = {      //first character of each line to be replaced by ">" character
        //     "********************", // this line is 20 characters. after that it will scroll (200ms). see LCD_SCROLL_SPEED variable in OnLCDLib.h.
        /*0*/  " -BrightBreeze menu-",   //title row doesn't display actual content or do anything
        /*1*/  "  __ deg F IN       ",   
        /*2*/  "  __ deg F OUT      ",
        /*3*/  "  Brighter: [in/out]",
        /*4*/  "  Set time          ", // calls setTimeScreen()
        /*5*/  "  Set target temp   ", // calls setTargetTemp()
        /*6*/  "  Set wakeup time   ", // calls setTimeScreen()
        /*7*/  "  Set bedtime       ", // calls setTimeScreen() 
    };
    uint8_t numMenuOptions = sizeof(menulist)/sizeof(menulist[0]);    

    //  while(1){
         if(!menu_is_idle){
            //uint8_t menu_startidx = 0;
            // uint8_t i;
            for(i = menu_startidx; i<=(menu_startidx+3); i++){  //i is menu index
                if(i < numMenuOptions){
                    uint8_t row = i - menu_startidx + 1;
                    // LCDWriteString(">")
                    if(i == 1){ //menu index 1 is inside temp
                        sprintf(menulist[i], "  %2u deg F IN       ", get_Temp(1));
                    }
                    else if(i == 2){    //menu index 2 is outside temp
                        sprintf(menulist[i], "  %2u deg F OUT      ", get_Temp(0));
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
                    LCDGotoXY(1, row);
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
                    //here 
                    
                    RTC_Read_Clock(0);
                    hr = bcd2decimal((hour & 0b00011111));
                    min = bcd2decimal(minute);
                    if (hour & TimeFormat12){
                        if(IsItPM(hour)) amOrPm = 'P';
                        else amOrPm = 'A';
                    }
                    else{amOrPm='A';}   //hardcoded to AM

                    //call setTimeScreen 
                    setTimeScreen(&hr, &min, &amOrPm);
                    RTC_Write_Time(hr, min, 0);
                }
                else if (selectedoption==5){
                    // set target temp 
                    setTargetTemp();
                    // now compare target temp with current temp and decide if to open a window
                }
                else if (selectedoption==6){
                    setTimeScreen(&wakeup_hr, &wakeup_min, &wakeup_amOrPm);
                    
                }
                else if (selectedoption==7){
                    setTimeScreen(&bedtime_hr, &bedtime_min, &bedtime_amOrPm);
                    
                }
                else if (selectedoption==8){
                    
                }
                else if (selectedoption==9){
                    
                }
            }
            LCDGotoXY(1,caretRow);
            LCDWriteString(">");
        }
    //  }
}

