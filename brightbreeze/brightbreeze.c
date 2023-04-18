
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "OnLCDLib.h"

//Our libraries
#include "i2c_functions.h"
#include "light_sensors.h"
#include "temp_sensor.h"
#include "real_time_clock.h"
#include "fan.h"
#include "motion_sensor.h"

int main(void){
    uint16_t inside_brightness;
    uint16_t outside_brightness;
    uint8_t inside_temp;
    uint8_t outside_temp;

    //initialize all sensors
    light_sensor_init();
    motion_sensor_init();
    temp_sensor_init();
    DDRC |= 1 << DDC0;      //initialize LED as output
    servo1__timer0_init();
    servo2__timer2_init();
    fan_init();

    //setting all sensors to starting state
    turn_led_off();
    fan_off();
    change_windows_timer2(0);
    change_blinds_timer0(0);
    uint8_t motion_time = 0;
    uint8_t time_since_last_moved = 0;
    uint8_t sunset_time = 0;

    while(1){

        //channel 0 is outside, channel 1 is inside
        outside_brightness= get_light_values(0);
        inside_brightness= get_light_values(1);

        // 0 means same, 1 means inside brighter, -1 means outside brighter
        int brightness_status = get_lightStatus(outside_brightness, inside_brightness);

        inside_temp = get_Temp(1);
        outside_temp = get_Temp(0);

        bool motion = check_motion();

        bool fan_on_condition = ((inside_temp >= 80) && (outisde_temp >= 80) && (motion_time <= 30));
        
        //need to set sunset time
        bool light_on_condition = (((brightness_status == 1) || (brightness_status == 0) || (curr_time >= sunset_time)) && (motion_time <= 30));
        
        
        // need to set the time variables
        bool end_of_day_condition = ((start_time > curr_time) && (end_time < curr_time));
        time_since_last_moved = curr_time;

        //check if it's end of day
        if(end_of_day_condition){
            fan_off();
            //0 means close window
            change_windows_timer2(0);
            change_blinds_timer0(0);
            turn_led_off();
            motion_time = 0;

        }

        //if its not the end of the day, enter checking state
        else{
            
            //check if theres motion
            if(motion){
                //time_since_last_moved = curr_time;
                motion_time = 0;
            }

            else{
               // motion_time = time_since_last_moved - curr_time;
            }
            
            //check if we should turn fan on
            if(fan_on_condition){
                fan_on();
                //0 means close window
                change_windows_timer2(0);
            }

            //check if we should turn fan off
            else if(!fan_on_condition){
                fan_off();

                //check if we should open windows instead
                if((outside_temp < inside_temp) && (motion_time <= 30) && (outside_temp >=65)){
                    change_windows_timer2(1);
                }
            }

            //check if we should turn light on
            //need to set curr_time and sunset_time
            if(light_on_condition){
                turn_led_on();
                change_blinds_timer0(0);
            }

            //check if we should turn light off
            else if(!light_on_condition){
                turn_led_off();

                if((brightness_status == -1) && (motion_time >= 30)){
                    change_blinds_timer0(1);
                }
            }

        }


        _delay_ms(500);
    }

    // never reached
    return 0;
}



void turn_led_on(){
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(100);

}

void turn_led_off(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
}