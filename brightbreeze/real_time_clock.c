#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "OnLCDLib.h"
#include "i2c_functions.h"


#define DS1307_Read_Mode   0xD1u  // DS1307 ID in read mode
#define DS1307_Write_Mode  0xD0u  // DS1307 ID in write mode
#define DS1307Z_ADDR 0x68       //address for real time clock


int main(void){
    LCDSetup(LCD_CURSOR_BLINK);

    uint8_t wp;
    uint8_t wp2;
    uint8_t rp[1];
    uint8_t rp_data[4];

    i2c_io(DS1307Z_ADDR, NULL, 0, &DS1307_Write_Mode, 1, rp, 1);

    if(rp[0] == 0x68){
        LCDWriteString("Got the addy"); 

    }

}

void RTC_init(){
    i2c_init(BDIV);
    uint8_t control_reg = 0x07;
    uint8_t wp = 0x00;

    //write to control reg to disable SQW
    i2c_io(DS1307Z_ADDR, &control_reg, 1, &wp, 1, NULL, 0);
}

void start_RTC_write(){
    i2c_io(DS1307Z_ADDR, &DS1307_CONTROL_REG, 1, &DS1307_Write_Mode, 1, NULL, 0);
}


void start_RTC_read(){
    i2c_io(DS1307Z_ADDR, NULL, 0, &DS1307_Read_Mode, 1, NULL, 0);
}