// (TSL datasheet: https://cdn-learn.adafruit.com/assets/assets/000/078/658/original/TSL2591_DS000338_6-00.pdf?1564168468)

const char* BLANK_20CHAR = "                    ";

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

void light_sensor_init(){
    uint16_t i;
    uint8_t wp;
    uint8_t wp2;
    uint8_t rp[1];
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
}

uint16_t get_light_values(uint16_t channel){
    //uint16_t visibleLight[2];
    uint16_t visible_infared_light= 0;
    uint16_t infared_light= 0;
    uint16_t visible_light= 0;

    uint8_t wp;
    uint8_t wp2;
    uint8_t rp[1];
    uint8_t rp_data[4];

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
        wp = 0b10110011;
        i2c_io(TSL2591_ADDR, &wp,  1,  NULL,  0, rp, 1);
    }
    
    // Read 4 bytes from C0DATAL register.  This gives you the contents of the
    // C0DATAL, C0DATAH, C1DATAL and C1DATAH registers (page 21) that contain the sensor results
    wp = 0b10110100;
    i2c_io(TSL2591_ADDR, NULL,  0,  &wp,  1, rp_data, 4);

    //taking in and combining the data bits into infared and infared+visible light
    visible_infared_light = ((uint16_t) rp_data[1] << 8) | rp_data[0]; //uppper bits OR with lower bits
    infared_light = ((uint16_t) rp_data[3] << 8) | rp_data[2];
    
    //subtracting the two to get visible light (still need to convert to lux)
    visible_light = visible_infared_light - infared_light;
    //visibleLight[channel] = visible_light;

    // Write 1 byte to the ENABLE to set PON but leave AEN a zero
    wp =  0b10100000;
    wp2 = 0b00000001;   //enable reg: 3 MSBs are 0
    i2c_io(TSL2591_ADDR, &wp,  1,  &wp2,  1, NULL, 0);

    return visible_light;
}

int get_lightStatus(uint16_t outside_value, uint16_t inside_value){
        // return 0 if same brightness
        if(abs((int)(inside_value - outside_value)) <= 2000){       //if the difference is less than 2000
            return 0;
        }
        // return 1 is inside is brighter
        else if(outside_value < inside_value){
            return 1;
        }
        // return -1 if outside is brighter 
        else if(outside_value > inside_value){
            return -1;
        }
        else{
            return 0;
        }
}


