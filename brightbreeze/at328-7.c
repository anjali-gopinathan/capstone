/*********************************************************************
*       at328-7.c - Demonstrate reading and writing to an I2C EEPROM
*
*       This program store some information in a 24LC256 (32k x 8)
*       I2C EEPROM and then read it back and reports the results out
*       the RS-232 interface.
*       If the button on PC1 is pressed it writes and then reads.
*       If the button is not pressed it only does the reading part.
*

* Revision History
* Date     Author      Description
* 01/28/08 A. Weber    First Release for MC68HC908JL16
* 02/12/08 A. Weber    Clean up code a bit
* 03/03/08 A. Weber    More code cleanups
* 04/22/08 A. Weber    Added "one" variable to make warning go away
* 04/14/11 A. Weber    Adapted for ATmega168
* 05/12/12 A. Weber    Modified for new I2C routines
* 11/18/13 A. Weber    Renamed for ATmega328P
* 04/01/15 A. Weber    Replaced sprintf's with snprintf's
*********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

void i2c_init(uint8_t);
uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
               uint8_t *, uint16_t, uint8_t *, uint16_t);

void sci_init(uint8_t);
void sci_out(char);
void sci_outs(char *);

uint8_t rdprom(uint8_t *, uint16_t, uint16_t);
uint8_t wrprom(uint8_t *, uint16_t, uint16_t);

// Find divisors for the UART0 and I2C baud rates
#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz

/* Address of the EEPROM on the I2C bus */
#define EEPROM_ADDR  0xA0

/* Address where we store test data in the EEPROM */
#define DATA_ADDR  500

/* Test strings to store in the EEPROM */
char *strs[] = {
    "1: EE459Lx at328-7.c - 5/12/12",
    "2: ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "3: AVR test of 24LC256 EEPROM"
};

uint8_t rdata[32];              // Buffer for reading data from EEPROM

#define OSTR_SIZE   80

char ostr[OSTR_SIZE];           // Buffer for creating strings to go out UART0

int main(void) {
    uint8_t status;
    uint8_t n;
    char *sp;

    PORTC |= (1 << PC1);        // Enable pull-up for switch on PORTC bit 1

    i2c_init(BDIV);             // Initialize the I2C port

    sci_init(MYUBRR);           // Initialize the SCI port

    sci_outs("\r\nat328-7.c\r\n");

    // Read the data at the address.  Stored string should be 31 characters or less.
    memset(rdata, 0, 32);
    status = rdprom(rdata, 32, DATA_ADDR);
    snprintf(ostr, OSTR_SIZE, "Read \"%s\" from EEPROM %d\r\n", rdata, DATA_ADDR);
    sci_outs(ostr);
    snprintf(ostr, OSTR_SIZE, "Status = 0x%2.2x\r\n", status);
    sci_outs(ostr);

    if ((PINC & 0x02) == 0) {   // If button down, write data
        // Figure out which string is there now and write a different one
        n = rdata[0] - '0' - 1;
        if (n < 3) {            // Should be 0, 1 or 2 for the strings above
            n++;
            if (n >= 3)
                n = 0;
            sp = strs[n];
        }
        else
            sp = strs[0];

        // Write data at the address
        snprintf(ostr, OSTR_SIZE, "Storing \"%s\" at EEPROM %d\r\n", sp, DATA_ADDR);
        sci_outs(ostr);
        n = strlen(sp) + 1;
        status = wrprom((uint8_t *) sp, n, DATA_ADDR);
        snprintf(ostr, OSTR_SIZE, "Status = 0x%2.2x\r\n", status);
        sci_outs(ostr);

        // Read back the data just written
        memset(rdata, 0, 32);
        status = rdprom(rdata, 32, DATA_ADDR);
        snprintf(ostr, OSTR_SIZE, "Read \"%s\" from EEPROM %d\r\n", rdata, DATA_ADDR);
        sci_outs(ostr);
        snprintf(ostr, OSTR_SIZE, "Status = 0x%2.2x\r\n", status);
        sci_outs(ostr);

    }
    else {
        snprintf(ostr, OSTR_SIZE, "Bypassing storing data\r\n");
        sci_outs(ostr);
    }

    while (1) {                 // Loop forever
    }
}

/*
  wrprom - write bytes to the EEPROM over the I2C bus
*/
uint8_t wrprom(uint8_t *p, uint16_t n, uint16_t a)
{
    uint16_t maxw;              // Maximum bytes to write in the page
    uint8_t status;
    uint8_t adata[2];           // Array to hold the address

    while (n > 0) {
        adata[0] = a >> 8;      // Put EEPROM address in adata buffer,
        adata[1] = a & 0xff;    // MSB first, LSB second
        // We can write up to the next 64 byte boundary,
        // but no more than is left to write
        maxw = 64 - (a % 64);   // Max for this page
        if (n < maxw)
            maxw = n;           // Number left to write in page
        status = i2c_io(EEPROM_ADDR, adata, 2, p, maxw, NULL, 0);
        if (status != 0)
            return(status);
        _delay_ms(5);           // Wait 5ms for EEPROM to write
        p += maxw;              // Increment array address
        a += maxw;              // Increment address
        n -= maxw;              // Decrement byte count
    }
    return(0);
}

/*
  rdprom - read bytes from the EEPROM over the I2C bus
*/
uint8_t rdprom(uint8_t *p, uint16_t n, uint16_t a)
{
    uint8_t status;
    uint8_t adata[2];           // Array to hold the address

    adata[0] = a >> 8;          // Put EEPROM address in adata buffer,
    adata[1] = a & 0xff;        // MSB first, LSB second

    status = i2c_io(EEPROM_ADDR, adata, 2, NULL, 0, p, n);
    return(status);
}

/* ----------------------------------------------------------------------- */

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

/* ----------------------------------------------------------------------- */

/*
  sci_init - Initialize the SCI port
*/
void sci_init(uint8_t ubrr) {
    UBRR0 = ubrr;            // Set baud rate
    UCSR0B |= (1 << TXEN0);  // Turn on transmitter
    UCSR0C = (3 << UCSZ00);  // Set for asynchronous operation, no parity, 
                             // one stop bit, 8 data bits
}

/*
  sci_out - Output a byte to SCI port
*/
void sci_out(char ch)
{
    while ((UCSR0A & (1<<UDRE0)) == 0);
    UDR0 = ch;
}

/*
  sci_outs - Print the contents of the character string "s" out the SCI
  port. The string must be terminated by a zero byte.
*/
void sci_outs(char *s)
{
    char ch;

    while ((ch = *s++) != (char) '\0')
        sci_out(ch);
}
