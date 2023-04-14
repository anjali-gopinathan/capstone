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

int main(void){
    LCDSetup(LCD_CURSOR_BLINK);			/* Initialize LCD */

    uint16_t inside_brightness;
    uint16_t outside_brightness;



    light_sensor_init();

    while(1){

        //channel 0 is outside, channel 1 is inside
        outside_brightness= get_light_values(0);
        inside_brightness= get_light_values(1);

        // 0 means same, 1 means inside brighter, -1 means outside brighter
        int brightness_status = get_lightStatus(outside_brightness, inside_brightness);




        _delay_ms(500);
    }
}



//use later
void flash_redled(){
    //DDRC |= 1 << DDC0; 
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}