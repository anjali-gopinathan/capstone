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
#include "servo_motors.h"
#include "led.h"

const int MOTION_THRESHOLD_TIME = 2;
const int BUTTONLESS_THRESHOLD = 10;
// const char* BLANK_20CHAR = "                    ";
char buffer[20];

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
      //      "Motion             "
};
uint8_t numMenuOptions = sizeof(menulist)/sizeof(menulist[0]);    


int hoveredoption = 0;
int selectedoption = -1;    // selected option = -1 means nothing has been selected
int caretRow = 1;

bool menu_activated = true;
uint8_t i;
uint8_t menu_startidx = 0;


uint8_t curr_hr, curr_min;
char curr_amOrPm;

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
void mainMenu(int *brightness_status, uint8_t *inside_temp, uint8_t *outside_temp);
void main_state_machine(int *brightness_status, uint8_t *inside_temp, uint8_t *outside_temp);


uint16_t hr_to_min(uint8_t hr, uint8_t min);
void test_state_machine();

bool up_pressed ()     { return ((PIND & (1 << 7)) != 0);}
bool select_pressed () { return ((PINB & (1 << 1)) != 0);}
bool down_pressed ()   { return ((PINB & (1 << 2)) != 0);}

void debounce(){
    _delay_ms(5);
    if(up_pressed() || select_pressed() || down_pressed()){
        _delay_ms(5);
    }
}

void LCDClearLine(uint8_t y){
    LCDGotoXY(1, y);
    LCDWriteString(BLANK_20CHAR);
    LCDGotoXY(1, y);
}

//for main state machine
void update_motion_time();
uint16_t motionless_time;
uint16_t timestamp_of_last_motion;

void update_buttonless_time();
uint16_t buttonless_time=0;
uint16_t timestamp_of_last_buttonpress;
bool is_window_open;
bool is_blind_open;

void splashScreen();
int main(void)
{
    LCDSetup(LCD_CURSOR_NONE);
    LCDClear();
    LCDHome();

    splashScreen();
    _delay_ms(1000);   
    /* Initialize all sensors */

    // I2C is for Real Time Clock and Light Sensors
    i2c_init(BDIV);

    RTC_Write_Time(2, 00, 00); //initial curr time is 12:00:00

    light_sensor_init();

    motion_sensor_init();
    motionless_time = 0;
    timestamp_of_last_motion = 0;

    temp_sensor_init();

    is_window_open = false;
    is_blind_open = false;

    servo1__timer0_init();
    change_blinds_timer0(0, &is_blind_open);

    servo2__timer2_init();
    change_windows_timer2(0, &is_window_open);

    fan_init(); //Fan is connected to PB7. set it as an output, then turn it off
    led_init(); //LED is connected to PB4. set it as an output, then turn it off

    
    //get the current time
    RTC_Read_Clock(0, &amOrPm);
    hr = bcd2decimal((hour & 0b00011111));
    min = bcd2decimal(minute);
    amOrPm = 'P';

    
    bedtime_hr = 10;
    bedtime_min = 0;
    bedtime_amOrPm = 'P';

    wakeup_hr = 5;
    wakeup_min = 0;
    wakeup_amOrPm = 'A';

    target_temp = 79;
    
    timestamp_of_last_buttonpress = 0;

    while(1){
        /* mainMenu() is the screen that displays the current temperatures inside and outside  */
       // update_buttonless_time();

        /* main_state_machine() is the standard state machine for adjusting blinds, windows, and fan*/
        
        // FOR BRIGHTNESS CALCULATIONS: channel 0 is outside, channel 1 is inside
        // 0 means same, 1 means inside brighter, -1 means outside brighter
        int brightness_status = get_lightStatus(get_light_values(0), get_light_values(1));
        uint8_t inside_temp = get_Temp(1);
        uint8_t outside_temp = get_Temp(0);

        // if(up_pressed() || down_pressed() || select_pressed()){
        //     menu_activated = true;
        //     timestamp_of_last_buttonpress = hr_to_min(hr, min);
        //     buttonless_time = 0;
        // }

        // else{
        //     buttonless_time = (hr_to_min(hr, min) - timestamp_of_last_buttonpress); 
        // }
       
        
        // if((buttonless_time > BUTTONLESS_THRESHOLD)){
        //     menu_activated = false;

        // }

    
        
        if(menu_activated){
            mainMenu(&brightness_status, &inside_temp, &outside_temp);
            
        }
        else{
            LCDClear();
            LCDHome();
            //Print buttonless time
            sprintf(buffer, "Menu idle");
            LCDWriteString(buffer);
        }
        main_state_machine(&brightness_status, &inside_temp, &outside_temp);
        _delay_ms(500);
    }
    return 0;   /* never reached */
}

void splashScreen(){
    LCDGotoXY(1,1); LCDWriteString("|    Welcome to    |");
    LCDGotoXY(1,2); LCDWriteString("|   BrightBreeze   |");
    LCDGotoXY(1,3); LCDWriteString("|Air & Light System|");
    LCDGotoXY(1,4); LCDWriteString("|  Powering on...  |");
}

/*  Display on LCD row 1 Current time: 12:34 PM   (HH:MM)
 *  Pressing up/down should increment/decrement the time
 *  Select will set the time for that digit and move to next most significant digit
 *  If the cursor is on the msb, select will set the time and return to the main menu
*/
void setTimeScreen( uint8_t *hr, uint8_t *min, char *amOrPm){
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
        _delay_ms(500);
        if(setTime_caretRow == 2){
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
            _delay_ms(500);

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
    //timestamp_of_last_buttonpress = hr_to_min(*hr, *min);
    _delay_ms(200);
}

void setTargetTemp(){
    //LCDClear();
    
    //make a copy of each of these variables
    int setTime_caretRow = 2;
    bool settingTemp = false; 
    //uint8_t curr_temp = get_Temp(1);
    // LCDWriteInt(curr_temp, 2);

    // sprintf(buffer, "  Curr temp: %02u", curr_temp);
    // LCDHome();
    // LCDWriteString(buffer);
    LCDGotoXY(1,2);
    sprintf(buffer, "  Target temp: %02u", target_temp);
    LCDWriteString(buffer);
    
    LCDGotoXY(1,3);
    LCDWriteString("  Go back           ");

    while(1){
        //update caret print
        LCDGotoXY(1,setTime_caretRow);
        LCDWriteString(">");
        _delay_ms(50);
        
        if(setTime_caretRow == 2){
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
            _delay_ms(50);

            // update set temp on second row
            sprintf(buffer, "  Target temp: %02u   ", target_temp);
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

}

void mainMenu(int *brightness_status, uint8_t *inside_temp, uint8_t *outside_temp){

    //uint8_t menu_startidx = 0;
    // uint8_t i;
    for(i = menu_startidx; i<=(menu_startidx+3); i++){  //i is menu index
        if(i < numMenuOptions){
            uint8_t row = i - menu_startidx + 1;
            // LCDWriteString(">")
            if(i == 1){ //menu index 1 is inside temp
                sprintf(menulist[i], "  %2u deg F IN       ", *inside_temp);
            }
            else if(i == 2){    //menu index 2 is outside temp
                sprintf(menulist[i], "  %2u deg F OUT      ", *outside_temp);
            }
            else if(i == 3){    //menu index 3 is brighter: in/out
                if(*brightness_status == 1){
                    sprintf(menulist[i], "  Brighter: IN      ");
                }
                else if(*brightness_status == -1){
                    sprintf(menulist[i], "  Brighter: OUT     ");
                }
                else {//if(brightness_status == 0){
                    sprintf(menulist[i], "  Brightness: SAME  ");
                }
            }
            // else if(i == 8){
            //     // sprintf(menulist[i], "  Motion %02u  ", check_motion());
            //     sprintf(menulist[i], "   %02d, %03u min  ", motionless_time, buttonless_time);
            // }
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
        //debounce();
    }
    else if(up_pressed() && (hoveredoption > 1)){  //if up pressed and there's still room on menu to go up
        hoveredoption--;
        if(caretRow > 1){
            caretRow--;
        }
        else{
            menu_startidx--;
        }
        //debounce();
    }
    else if(select_pressed()){
        selectedoption = hoveredoption;
        // call function for each option
        if (selectedoption==4){
            //here 
            
            RTC_Read_Clock(0, &amOrPm);
            hr = bcd2decimal((hour & 0b00011111));
            min = bcd2decimal(minute);
            //call setTimeScreen 
            setTimeScreen(&hr, &min, &amOrPm);
            RTC_Write_Time(hr, min, 0);
        }
        else if (selectedoption==5){
            // set target temp 
            LCDClear();
            sprintf(buffer, "  Curr temp: %02u", *inside_temp);
            LCDHome();
            LCDWriteString(buffer);
            setTargetTemp();
            // now compare target temp with current temp and decide if to open a window
        }
        else if (selectedoption==6){
            setTimeScreen(&wakeup_hr, &wakeup_min, &wakeup_amOrPm);  
        }
        else if (selectedoption==7){
            setTimeScreen(&bedtime_hr, &bedtime_min, &bedtime_amOrPm);  
        }
        //menu_activated = false;
        debounce();
    }
    LCDGotoXY(1,caretRow);
    LCDWriteString(">");
}

void update_buttonless_time(){
    if( down_pressed() || up_pressed() || select_pressed()){
        timestamp_of_last_buttonpress = hr_to_min(hr, min);
        buttonless_time = 0;
    }
    else{
        // buttonless_time = hr_to_min(curr_hr, curr_min) - timestamp_of_last_buttonpress;  
        buttonless_time = (hr_to_min(hr, min) - timestamp_of_last_buttonpress); 
    }
}
uint16_t hr_to_min(uint8_t hr, uint8_t min){
    uint16_t tim1 = ((uint16_t)(hr)*60) + (uint16_t)(min);
    return tim1;
}

void main_state_machine(int *brightness_status, uint8_t *inside_temp, uint8_t *outside_temp){
    RTC_Read_Clock(0, &amOrPm);
    hr = bcd2decimal((hour & 0b00011111));
    min = bcd2decimal(minute);
    
    uint16_t curr_min = (((uint16_t)(hr))*60) + (uint16_t)(min);
    uint16_t sunset_min = (((uint16_t)(7))*60) + (uint16_t)(0);
    uint16_t bedtime_in_min =  (((uint16_t)(bedtime_hr))*60) + (uint16_t)(bedtime_min);
    uint16_t wakeup_in_min = (((uint16_t)(wakeup_hr))*60) + (uint16_t)(wakeup_min);
    
    sunset_min += 720;

    if(amOrPm == 'P'){
        curr_min += 720;   //720 min is 12 hours
    }
    
    if(bedtime_amOrPm == 'P'){
        bedtime_in_min += 720;   //720 min is 12 hours
    }

    if(wakeup_amOrPm == 'P'){
        wakeup_in_min += 720;   //720 min is 12 hours
    }

    //turn on light if it's (dark outside or it's after sunset) and there's motion inside recently ie people are home and awake
    bool light_on_condition = (((curr_min > sunset_min) || (*brightness_status != -1))
                                && (motionless_time <= MOTION_THRESHOLD_TIME));

    // It's end of day if current time is (past bedtime and before midnight) or (after midnight and before wakeup time)
    bool end_of_day_condition = ((curr_min > bedtime_in_min) && (curr_min < 1439)) 
                                || ((curr_min > 0) && (curr_min < wakeup_in_min));
        
    bool motion = check_motion();

    bool fan_on_condition = (((*inside_temp >= 80) && (*outside_temp >= 80) && (motionless_time <= MOTION_THRESHOLD_TIME))
                        ||   ((*inside_temp > target_temp) && (*outside_temp < target_temp))
                            );
    
    //check if it's end of day
    if(end_of_day_condition){
        fan_off();
        change_windows_timer2(0, &is_window_open);      //0 means close window
        change_blinds_timer0(0, &is_blind_open);
        led_off();
        motionless_time = 0;
    }

    //if its not the end of the day, enter checking state
    else{
        
        //check if theres motion
        if(motion){
            timestamp_of_last_motion = hr_to_min(curr_hr, curr_min);
            motionless_time = 0;
        }

        else{
            motionless_time = hr_to_min(curr_hr, curr_min) - timestamp_of_last_motion;
            
        }

        
        //check if we should turn fan on
        if(fan_on_condition){
            fan_on();
            //0 means close window
            change_windows_timer2(0, &is_window_open);
        }

        //check if we should turn fan off
        else if(!fan_on_condition){
            fan_off();

            //check if we should open windows instead
            if((*outside_temp < *inside_temp) && (motionless_time <= MOTION_THRESHOLD_TIME) && (*outside_temp >=65)){
                change_windows_timer2(1, &is_window_open);
            }
        }

        //check if we should turn light on
        //need to set curr_time and sunset_time
        if(light_on_condition){
            led_on();

            change_blinds_timer0(0, &is_blind_open);
        }

        //check if we should turn light off
        else if(!light_on_condition){
            led_off();
            
            // led_on();//DEBUG LED ON (remove later)
            if((*brightness_status == -1) && (motionless_time <= MOTION_THRESHOLD_TIME)){
                change_blinds_timer0(1, &is_blind_open);
            }
        }
    }
}

void test_state_machine(){
   //hardcode the times we want to test
    wakeup_hr = 5;
    wakeup_min = 0;
    wakeup_amOrPm = 'A';
   
    bedtime_hr = 10;
    bedtime_min = 0;
    bedtime_amOrPm = 'P';

    hr = 3;
    min = 0;
    amOrPm = 'P';

    //channel 0 is outside, channel 1 is inside
    uint16_t outside_brightness= get_light_values(0);
    uint16_t inside_brightness= get_light_values(1);
    uint16_t brightness_status = get_lightStatus(outside_brightness, inside_brightness);
    LCDGotoXY(1,1);
    if(brightness_status == 1){
        LCDWriteString("  Brighter: IN      ");
        fan_on();
    }
    else if(brightness_status == -1){
         LCDWriteString("  Brighter: OUT     ");
         fan_off();
    }
    else {//if(brightness_status == 0){
        LCDWriteString("  Brighter: SAME    ");
        fan_off();
    }

    LCDGotoXY(1,2);
    sprintf(buffer, "Inside temp:  %02u    ", get_Temp(1));
    LCDWriteString(buffer);

    LCDGotoXY(1,3);
    sprintf(buffer, "Outside temp: %02u    ", get_Temp(0));
    LCDWriteString(buffer);

    LCDGotoXY(1,4);
    if(check_motion()){
        LCDWriteString("Motion              ");
        // fan_on();
        // _delay_ms(5000);
    }
    else{
        LCDWriteString("NO Motion           ");
        // fan_off();
        // _delay_ms(5000);
    }
    //  bool end_of_day_condition = ((compareTime(hr, min, amOrPm, wakeup_hr, wakeup_min, wakeup_amOrPm) == -1) 
    //                                 && (compareTime(hr, min, amOrPm, bedtime_hr, bedtime_min, bedtime_amOrPm) == 1) );

      //check if it's end of day
    // if(!motion){
    //     fan_off();
    //     change_windows_timer2(0);
    //     change_blinds_timer0(0);
    //     //motionless_time = 0;

    // }

    // //if its not the end of the day, enter checking state
    // else{
    //     fan_on();
    //     change_windows_timer2(1);
    //     change_blinds_timer0(1);
        

    // }


}
