#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define TCAADDR 0xE0        // address for MUX
#define TSL2591_ADDR 0x52     // address for Light sensor (TSL)

void i2c_init(uint8_t);
uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
               uint8_t *, uint16_t, uint8_t *, uint16_t);
void flash_ledpin2();
void select_lightsensor(uint16_t);

// Find divisors for the UART0 and I2C baud rates
#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
// #define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV ((FOSC / 100000 - 16) / 2 + 1   ) // Puts I2C rate just below 100kHz

int main(void){
    
    i2c_init(BDIV);             // Initialize the I2C port
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, green led) for output
   
    // printf("TSL2591 Light Sensor Test\n\n");
    
    // activate light sensor 0

    // power on TSL light sensor
    // // to send to the command register of one of the TSLs:
    // // (see page 14 of TSL datasheet: https://cdn-learn.adafruit.com/assets/assets/000/078/658/original/TSL2591_DS000338_6-00.pdf?1564168468)
    // // 0b10100110
    // to send to the ENABLE regsiter (beginning of page 15)
    // 0b76543210
    // 0b00000001
    /*
    SET UP I2C WITH i2c_io function
    */
 _delay_ms(3000);
    // uint8_t tsl_id_reg = 0x12;
   //[ctrl register, something to send to that reg]
   uint16_t i;
    for(i=0; i<2; i++){
        select_lightsensor(i);
        // Read 1 bytes from the ID register (of TSL)
        // ID register is 0x12, Device Identification is 0x50
        uint8_t rp[1] = {0x12};
        i2c_io(TSL2591_ADDR, NULL,  0,  NULL,  0, rp , 1);
        if(rp[0] == 0x50){ // if device id is 0x50 (it should be)
            //Write 1 byte to the ENABLE register to set the PON bit. (see page 15 of datasheet).
            // ENABLE is 0x00
            // Power ON is 0x01
            //uint8_t wp[2] = {0x00, 0x01};
            uint8_t wp[1] = {0x01};
            i2c_io(TSL2591_ADDR, NULL,  0,  wp,  1, NULL, 0); 

            //Write 1 bytes to the CONTROL register to set AGAIN to medium and ATIME to 400ms.
            // CONTROL REG is 0x01
            // 0b0001
            //uint8_t wp2[2] = {0x01, 0b01010011};
            uint8_t wp2[1] = {0b0001011};
            i2c_io(TSL2591_ADDR, NULL,  0,  wp2,  1, NULL, 0);
        }
    }
    // select ls 0 by communicating with the MUX

    //now that ls 0 is selected, we can talk to 0x29 (TSL) and power on LS 0
    
    _delay_ms(3000);

        
    //uint16_t visibleLight_0 = 0;
    //uint16_t visibleLight_1 = 0;
    uint16_t visibleLight[2] = {0,0};
    //read sensor values
    while (1) {
        uint16_t channel;   
        for (channel = 0; channel < 2; channel++) {
            //select channel
            select_lightsensor(channel);
            // Write 1 byte to the ENABLE register to set PON and AEN bits.  This
            // starts a conversion
            //uint8_t wp1[2] = {0x00, 0b000000011};
            uint8_t wp1[1] = {0b00000011};
            i2c_io(TSL2591_ADDR, NULL,  0,  wp1,  1, NULL, 0);


            // Read 1 byte from the STATUS register.  If the AVALID bits is zero repeat
            // this.  (see page 20) 
            // STATUS is 0x13
            uint8_t rp1[1] = {0x13};
            i2c_io(TSL2591_ADDR, NULL,  0,  NULL,  0, rp1, 1);
            if (rp1[0] == 0x00){
                flash_ledpin2();
            }

            // Read 4 bytes from C0DATAL register.  This gives you the contents of the
            // C0DATAL, C0DATAH, C1DATAL and C1DATAH registers (page 21) that contain the
            // sensor results
            uint8_t rp2[4] = {0x14, 0, 0, 0};
            i2c_io(TSL2591_ADDR, NULL,  0,  NULL,  0, rp2, 4);
            visibleLight[channel] = rp2[0];
                    
            // Write 1 byte to the ENABLE to set PON but leave AEN a zero
            //uint8_t wp2[2] = {0x00, 0x01};
            uint8_t wp2[1] = {0x0000001};
            i2c_io(TSL2591_ADDR, NULL,  0,  wp2,  1, NULL, 0);
        }
        if(visibleLight[0] != visibleLight[1]){
            //set LED on
            flash_ledpin2();    
        }
        else{

        }
        
        _delay_ms(1000);

    }

    
}

/*
  i2c_io - write and read bytes to a slave I2C device

  This funtions write "an" bytes from array "ap" and then "wn" bytes from array
  "wp" to I2C device at address "device_addr".  It then reads "rn" bytes from
  the same device to array "rp".

  Return values (might not be a complete list):
        0    - Success
        0x20 - NAK received after sending device address for writing
        0x30 - NAK received after sending data
        0x38 - Arbitration lost with address or data
        0x48 - NAK received after sending device address for reading

  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

This "i2c_io" I2C routine is an attempt to provide an I/O function for both
reading and writing, rather than have separate functions.

I2C writes consist of sending a stream of bytes to the slave device.  In some
cases the first few bytes may be the internal address in the device, and then
the data to be stored follows.  For example, EEPROMs like the 24LC256 require a
two-byte address to precede the data.  The DS1307 RTC requires a one-byte
address.

I2C reads often consist of first writing one or two bytes of internal address
data to the device and then reading back a stream of bytes starting from that
address.  Some devices appear to claim that that reads can be done without
first doing the address writes, but so far I haven't been able to get any to
work that way.

This function does writing and reading by using pointers to three arrays "ap",
"wp", and "rp".  The function performs the following actions in this order:
    If "an" is greater than zero, then "an" bytes are written from array "ap"
    If "wn" is greater then zero, then "wn" bytes are written from array "wp"
    If "rn" is greater then zero, then "rn" byte are read into array "rp"
Any of the "an", "wn", or "rn" can be zero.

The reason for separate "ap" and "wp" arrays is that the address data can be
taken from one array (ap), and then the write data from another (wp) without
requiring that the contents be merged into one array before calling the
function.  This means the following three calls all do exactly the same thing.

    i2c_io(0xA0, buf, 100, NULL, 0, NULL, 0);
    i2c_io(0xA0, NULL, 0, buf, 100, NULL, 0);
    12c_io(0xA0, buf, 2, buf+2, 98, NULL, 0);

In all cases 100 bytes from array "buf" will be written to the I2C device at
bus address 0xA0.

A typical write with a 2-byte address is done with

    i2c_io(0xA0, abuf, 2, wbuf, 50, NULL, 0);

A typical read with a 1-byte address is done with

    i2c_io(0xD0, abuf, 1, NULL, 0, rbuf, 20);
*/

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn)
{
    uint8_t status, send_stop, wrote, start_stat;

    status = 0;
    wrote = 0;
    send_stop = 0;

    if (an > 0 || wn > 0) {
        wrote = 1;
        send_stop = 1;

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);  // Send start condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x08)                 // Check that START was sent OK
            return(status);

        TWDR = device_addr & 0xfe;          // Load device address and R/W = 0;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x18) {               // Check that SLA+W was sent OK
            if (status == 0x20)             // Check for NAK
                goto nakstop;               // Send STOP condition
            return(status);                 // Otherwise just return the status
        }

        // Write "an" data bytes to the slave device
        while (an-- > 0) {
            TWDR = *ap++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }

        // Write "wn" data bytes to the slave device
        while (wn-- > 0) {
            TWDR = *wp++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }

        status = 0;                         // Set status value to successful
    }

    if (rn > 0) {
        send_stop = 1;

        // Set the status value to check for depending on whether this is a
        // START or repeated START
        start_stat = (wrote) ? 0x10 : 0x08;

        // Put TWI into Master Receive mode by sending a START, which could
        // be a repeated START condition if we just finished writing.
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
                                            // Send start (or repeated start) condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != start_stat)           // Check that START or repeated START sent OK
            return(status);

        TWDR = device_addr  | 0x01;         // Load device address and R/W = 1;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Send address+r
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x40) {               // Check that SLA+R was sent OK
            if (status == 0x48)             // Check for NAK
                goto nakstop;
            return(status);
        }

        // Read all but the last of n bytes from the slave device in this loop
        rn--;
        while (rn-- > 0) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read byte and send ACK
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x50)             // Check that data received OK
                return(status);
            *rp++ = TWDR;                   // Read the data
        }

        // Read the last byte
        TWCR = (1 << TWINT) | (1 << TWEN);  // Read last byte with NOT ACK sent
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x58)                 // Check that data received OK
            return(status);
        *rp++ = TWDR;                       // Read the data

        status = 0;                         // Set status value to successful
    }
    
nakstop:                                    // Come here to send STOP after a NAK
    if (send_stop)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Send STOP condition

    return(status);
}


/*
  i2c_init - Initialize the I2C port
*/
void i2c_init(uint8_t bdiv)
{
    TWSR = 0;                           // Set prescalar for 1
    TWBR = bdiv;                        // Set bit rate register
}

void flash_ledpin2(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}

// Will select a light register that is 0 or 1
// use ~5kohm resistor. to test if that's an appropriate resistance, test w oscope on each side of the mux
void select_lightsensor(uint16_t channel){
    uint8_t datatosend = 0x00;
    /*if(channel == 0){
        datatosend = 1 << 0;
    }
    else if(channel == 1){
        datatosend = 1 << 1;
    }*/
    // channel 0 is 0bXXXXXXX1
    // channel 1 is 0bXXXXXX1X
    datatosend = 1 << channel;
    // uint8_t ctrl_reg = 0x01;    //according to TSL2591 page 16
    i2c_io(TCAADDR, NULL, 0, &datatosend, 1, NULL, 0);
}
/*float calculate_lux(uint16_t ch0_data, uint16_t ch1_data) {
    float lux = 0.0;
    float ratio = 0.0;

    if (ch0_data != 0) {
        ratio = (float)ch1_data / (float)ch0_data;
    }

    if (ratio < 0.5) {
        lux = (0.0304 * ch0_data) - (0.062 * ch0_data * pow(ratio, 1.4));
    } else if (ratio < 0.61) {
        lux = (0.0224 * ch0_data) - (0.031 * ch1_data);
    } else if (ratio < 0.8) {
        lux = (0.0128 * ch0_data) - (0.0153 * ch1_data);
    } else if (ratio < 1.3) {
        lux = (0.00146 * ch0_data) - (0.00112 * ch1_data);
    }

    return lux;
}*/
