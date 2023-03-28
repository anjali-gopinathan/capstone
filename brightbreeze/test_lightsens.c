#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
// #include <float.h>

#define TCAADDR 0x70        // address for MUX
#define TSL2591_ADDR 0x29     // address for Light sensor (TSL)
/* Assign a unique ID to this sensor at the same time */
// Adafruit_TSL2591 out_ls0 = {1};  // outside light sensor is on sda/scl 0
// Adafruit_TSL2591 in_ls1  = {2};  //  inside light sensor is on sda/scl 1

void i2c_init(uint8_t);
uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
               uint8_t *, uint16_t, uint8_t *, uint16_t);
void mux_start(uint8_t);
void mux_write(uint8_t);
void mux_stop();
uint16_t tsl_read_register(uint8_t reg);
// float calculate_lux(uint16_t ch0_data, uint16_t ch1_data);
void flash_ledpin2();

// Find divisors for the UART0 and I2C baud rates
#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
// #define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV ((FOSC / 100000 - 16) / 2 + 1   ) // Puts I2C rate just below 100kHz

int main(void){
    /* steps: what address is it using, 
    find in datasheet what command to send to talk to a certain channel. 
    should see an ACK back on the oscope.

    at beginning of program put a delay of 3s. 
    put oscope on clock and data line. press SINGLE to show the next occurrence
    */
    i2c_init(BDIV);             // Initialize the I2C port
    // if light sensor senses BRIGHT, light LED
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, green led) for output
    // //flash led
    // PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    // _delay_ms(100);
    // PORTC |= 1 << PC0;      // Set PC0 to a 1
    // _delay_ms(1000);
    // PORTC &= ~(1 << PC0);   // Set PC0 to a 0

    printf("TSL2591 Light Sensor Test\n\n");
    // scan MUX for i2c addresses

    /* Initialise the 1st sensor */
    
    _delay_ms(3000);
    uint8_t channel;
    for (channel = 0; channel < 2; channel++) {
        mux_start(channel);
        // flash_ledpin2();
        
        mux_write((TCAADDR << 1) | TW_WRITE);
        // flash_ledpin2();

        mux_write(channel);    //changed from passing 1 << channel
        // flash_ledpin2();
        
        mux_stop();       
        // flash_ledpin2();

        mux_start(channel);
        // flash_ledpin2();
        
        mux_write((TCAADDR << 1) | TW_WRITE);
        // flash_ledpin2();
        
        mux_write(0x80 | 0x00); // COMMAND | ENABLE register
        // flash_ledpin2();
        
        mux_write(0x01 | 0x02); // Power ON and ALS enable
        mux_stop();        
        _delay_ms(100);
        //printf("Address for channel 0: %f Lux\n", event.light);

    }

    /* Display some basic information on this sensor */
    // printf("selecting OUTSIDE light sensor (sda/scl 0)...\n");
    // mux_select(0);
    
    // printf("\nselecting INSIDE light sensor (sda/scl 1)...\n");
    // mux_select(1);
        
    uint16_t visibleLight_0 = 0;
    // float visibleLux0 = 0.0f;
    // float visibleLux1 = 0.0f;
    uint16_t visibleLight_1 = 0;
    while (1) {
        // flash_ledpin2();

        for (channel = 0; channel < 2; channel++) {
            //select channel
            mux_start(channel);
            mux_write((TCAADDR << 1) | TW_WRITE);
            mux_write(channel); // changed from 1 << channel
            mux_stop(); 
            
            // get channel data
            uint16_t ch0_data = tsl_read_register(0x14);
            uint16_t ch1_data = tsl_read_register(0x16);
            // Combine the lower and upper bytes to obtain the 16-bit values
            ch0_data |= (tsl_read_register(0x15) << 8); // CH0 upper byte
            ch1_data |= (tsl_read_register(0x17) << 8); // CH1 upper byte
            // 0 to 65535 (2^16 - 1)
            if(channel == 0){
                visibleLight_0 = ch0_data - ch1_data;   // call a function to fix this
                // visibleLux0 = calculate_lux(ch0_data, ch1_data);
                // printf("Sensor %d - CH0: %d, CH1: %d, LUX = %f\n", channel, ch0_data, ch1_data, visibleLight_0);
            }
            else{
                visibleLight_1 = ch0_data - ch1_data; 
                // visibleLux1 = calculate_lux(ch0_data, ch1_data);
                // printf("Sensor %d - CH0: %d, CH1: %d, LUX = %f\n", channel, ch0_data, ch1_data, visibleLight_1);
            }

        }


        if(visibleLight_0 != visibleLight_1){
            //set LED on
            //PORTC |= 1 << PC0;      // Set PC0 to a 1
            flash_ledpin2();

        }
        else{
            //set LED off
            //PORTC &= ~(1 << PC0);   // Set PC0 to a 0

        }
        // uint16_t als_data = 0;
        // mux_start(0);
        // mux_write((TSL2591_ADDR << 1) | TW_WRITE);
        // mux_write(0x80 | 0x20 | reg); // COMMAND | DATA register
        // mux_start(0);
        // mux_write((TSL2591_ADDR << 1) | TW_READ);
        // als_data = TWDR;
        // mux_stop();
        // printf("OUTSIDE sensor illuminance: %f Lux\n", event.light);
        
        // PORTC |= 1 << PC0;      // Set PC0 to a 1
        // _delay_ms(1000);
        // PORTC &= ~(1 << PC0);   // Set PC0 to a 0
        // _delay_ms(500);
        /*
        figure out the different addresses 
        */
        // mux_select(0);
        // getEvent(&out_ls0, &event);

        // mux_select(1);
        // getEvent(&in_ls1, &event);
        // printf("INSIDE sensor illuminance: %f Lux\n", event.light);
        
        _delay_ms(1000);
        // if((/*LS0 port and address*/) == /*some value that equates to BRIGHT*/){
        //     // turn ON LED
        //     PORTC |= 1 << PC0;      // Set PC0 to a 1
        // }
        // else{   // in this case LS0 is in some range that equates to DARK

        // }
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

void mux_start(uint8_t i)
{
    if (i>7 ) {
        return;
    }
    
    // Start I2C transmission (on TCAADDR)
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set

    // TWDR = TCAADDR // Load mux address 
    // TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
    // while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
}
void mux_write(uint8_t i)   // 3/27 ISSUE: problem occurs when we pass i = (1 << channel) where channel is 0 or 1
{
    // Write to address i
    TWDR = 1 << i; // Load i address 
    // TWDR = i;    // maybe set TWDR to the address instead of shifted?
    TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
    while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set


}
void mux_stop()
{ 
    // Stop I2C transmission
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); 
    while(TWCR & (1 << TWSTO));
    
}
uint16_t tsl_read_register(uint8_t reg){
    uint16_t data = 0;
    mux_start(0);
    mux_write((TSL2591_ADDR << 1) | TW_WRITE);
    mux_write(0x80 | 0x20 | reg); // COMMAND | DATA register
    mux_start(0);
    mux_write((TSL2591_ADDR << 1) | TW_READ);
    data = TWDR;
    mux_stop();
    return data;
}
/*
float calculate_lux(uint16_t ch0_data, uint16_t ch1_data) {
    float lux = 0.0f;
    float ratio = 0.0f;
    float ch0f = (float)ch0_data;
    float ch1f = (float)ch1_data;

    if (ch0f != 0.0) {
        ratio = ch1f / ch0f;
    }

    if (ratio < 0.5f) {
        lux = (0.0304f * ch0f) - (0.062f * ch0f * pow(ratio, 1.4f));
    } else if (ratio < 0.61f) {
        lux = (0.0224f * ch0f) - (0.031f * ch1f);
    } else if (ratio < 0.8f) {
        lux = (0.0128f * ch0f) - (0.0153f * ch1f);
    } else if (ratio < 1.3f) {
        lux = (0.00146f * ch0f) - (0.00112f * ch1f);
    }

    return lux;
}*/
void flash_ledpin2(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}