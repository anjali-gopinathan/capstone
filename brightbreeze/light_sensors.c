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

// (TSL datasheet: https://cdn-learn.adafruit.com/assets/assets/000/078/658/original/TSL2591_DS000338_6-00.pdf?1564168468)


void select_lightsensor(uint16_t);

int main(void){

    LCDSetup(LCD_CURSOR_BLINK);
    
    i2c_init(BDIV);             // Initialize the I2C port   

    uint16_t i;
    uint8_t wp;
    uint8_t wp2;
    uint8_t rp[1];
    uint8_t rp_data[4];
    for(i=0; i<2; i++){
        select_lightsensor(i);
        // Read 1 bytes from the ID register (of TSL)
        // ID register is 0x12, Device Identification is 0x50
        wp = 0b10110010; // from cmd reg: 5 LSBs are 0x12 in binary. wp is b2.
        i2c_io(TSL2591_ADDR, NULL,  0,  &wp,  1, rp , 1);   //overwrites rp
        if(rp[0] == 0x50){ // if device id is 0xE0 (it should be)
            //Write 1 byte to the ENABLE register to set the PON bit. (see page 15 of datasheet).
            // Power ON in ENABLE is 0x01
            wp2 = 0b00000001; //in the enable reg   
            wp =  0b10100000; //in the command reg  -- this is hex A0
            i2c_io(TSL2591_ADDR, &wp,  1, &wp2,  1, NULL, 0); 

            //Write 1 bytes to the CONTROL register to set AGAIN to medium and ATIME to 400ms.
            wp2 = 0b00010011; //in the control register -- this is hex 13
            wp =  0b10100001; //in the command register -- this is hex A1
            i2c_io(TSL2591_ADDR, &wp,  1,  &wp2,  1, NULL, 0);
        }
    }
    
   //_delay_ms(4000);

        
    uint16_t visibleLight[2];
    int count = 0;
    uint16_t visible_infared_light= 0;
    uint16_t infared_light= 0;
    uint16_t visible_light= 0;

    //read sensor values
    while (1) {
        uint16_t channel;   
        for (channel = 0; channel < 2; channel++) {
            //select channel
            select_lightsensor(channel);
            // Write 1 byte to the ENABLE register to set PON and AEN bits.  This
            // starts a conversion
            wp2 = 0b00000011; //in the enable reg   --0x03
            wp =  0b10100000; //in the command reg  --0xA0
            i2c_io(TSL2591_ADDR, &wp,  1,  &wp2,  1, NULL, 0);


            // Read 1 byte from the STATUS register.  If the AVALID bits is zero repeat this.  (see page 20) 
            // STATUS is 0x13
            wp = 0b10110011;    //--this is hex b3
            i2c_io(TSL2591_ADDR, &wp,  1,  NULL,  0, rp, 1);
            while ((rp[0] & 0x01) == 0){ //to check if rp1[0]'s LSB is 1: if(rp1[0] & 0b00000001 == 0)
                //flash_redled();
                wp = 0b10110011;
                i2c_io(TSL2591_ADDR, &wp,  1,  NULL,  0, rp, 1);
            }

           // _delay_ms(4000);

            // Read 4 bytes from C0DATAL register.  This gives you the contents of the
            // C0DATAL, C0DATAH, C1DATAL and C1DATAH registers (page 21) that contain the sensor results
            wp = 0b10110100;
            i2c_io(TSL2591_ADDR, NULL,  0,  &wp,  1, rp_data, 4);

            //taking in and combining the data bits into infared and infared+visible light
            visible_infared_light = ((uint16_t) rp_data[1] << 8) | rp_data[0]; //uppper bits OR with lower bits
            infared_light = ((uint16_t) rp_data[3] << 8) | rp_data[2];
           
            //subtracting the two to get visible light (still need to convert to lux)
            visible_light = visible_infared_light - infared_light;
            visibleLight[channel] = visible_light;

            // lab room with all lights on gave value of 8755

            // Write 1 byte to the ENABLE to set PON but leave AEN a zero
            wp =  0b10100000;
            wp2 = 0b00000001;   //enable reg: 3 MSBs are 0
            i2c_io(TSL2591_ADDR, &wp,  1,  &wp2,  1, NULL, 0);

        }

        if(abs((int)(visibleLight[1] - visibleLight[0])) < 200){
            LCDGotoXY(1,1);
            LCDWriteString("basically same");

        }
        else if(visibleLight[0] < visibleLight[1]){
            LCDGotoXY(1,1);
            LCDWriteString("inside is brighter than outside");
        }
        else if(visibleLight[0] > visibleLight[1]){
            LCDGotoXY(1,1);
            LCDWriteString("outside is brighter than inside");
        }
        
        _delay_ms(1000);

    }

    
}


// Will select a light register that is 0 or 1
// use ~5kohm resistor. to test if that's an appropriate resistance, test w oscope on each side of the mux
void select_lightsensor(uint16_t channel){
    uint8_t datatosend = 0x00;
    if(channel == 0){
        datatosend = 0b00000001;
    }
    else if(channel == 1){
        datatosend = 0b00000010;
    }
    //datatosend = 1 << channel;
    // uint8_t ctrl_reg = 0x01;    //according to TSL2591 page 16
    i2c_io(TCAADDR, NULL, 0, &datatosend, 1, NULL, 0);
}

