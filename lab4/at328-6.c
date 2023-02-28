/*************************************************************
*       at328-6.c - Demonstrate interface to a serial LCD display
*
*       This program will print a message on an LCD display
*       using a serial interface.  It is designed to work with a
*       Matrix Orbital LK204-25 using an RS-232 interface.
*
* Revision History
* Date     Author      Description
* 11/07/07 A. Weber    First Release
* 02/26/08 A. Weber    Code cleanups
* 03/03/08 A. Weber    More code cleanups
* 04/22/08 A. Weber    Added "one" variable to make warning go away
* 04/11/11 A. Weber    Adapted for ATmega168
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);

void sci_init(void);
void sci_out(char);
void sci_outs(char *);

char str1[] = "12345678901234567890";
char str2[] = ">> USC EE459L <<";
char str3[] = ">> at328-6.c <<<";
char str4[] = "-- April 11, 2011 --";

#define FOSC 9830400		// Clock frequency
#define BAUD 19200              // Baud rate used by the LCD
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

int main(void) {

    sci_init();                 // Initialize the SCI port
    
    lcd_init();                 // Initialize the LCD

    lcd_moveto(0, 0);
    lcd_stringout(str1);        // Print string on line 1
    lcd_moveto(1, 2);
    lcd_stringout(str2);        // Print string on line 2
    lcd_moveto(2, 2);
    lcd_stringout(str3);        // Print string on line 3
    lcd_moveto(3, 0);
    lcd_stringout(str4);        // Print string on line 4

    while (1) {                 // Loop forever
    }

    return 0;   /* never reached */
}

/*
  lcd_init - Initialize the LCD
*/
void lcd_init()
{
    _delay_ms(250);             // Wait 500msec for the LCD to start up
    _delay_ms(250);
    sci_out(0xfe);              // Clear the screen
    sci_out(0x58);
}

/*
  moveto - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    sci_out(0xfe);              // Set the cursor position
    sci_out(0x47);
    sci_out(col + 1);
    sci_out(row + 1);
}


/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    sci_outs(str);              // Output the string
}

/* ----------------------------------------------------------------------- */

/*
  sci_init - Initialize the SCI port
*/
void sci_init(void) {
    UBRR0 = MYUBRR;          // Set baud rate
    UCSR0B |= (1 << TXEN0);  // Turn on transmitter
    UCSR0C = (3 << UCSZ00);  // Set for asynchronous operation, no parity, 
                             // one stop bit, 8 data bits
}

/*
  sci_out - Output a byte to SCI port
*/
void sci_out(char ch)
{
    while ( (UCSR0A & (1<<UDRE0)) == 0);
    UDR0 = ch;
}

/*
  sci_outs - Print the contents of the character string "s" out the SCI
  port. The string must be terminated by a zero byte.
*/
void sci_outs(char *s)
{
    char ch;

    while ((ch = *s++) != '\0')
        sci_out(ch);
}
