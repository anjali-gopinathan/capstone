#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"                // Declarations of the LCD functions

/* This function not declared in lcd.h since
   should only be used by the routines in this file. */
void lcd_writenibble(unsigned char);

/* Define a couple of masks for the bits in Port B and Port D */
// #define DATA_BITS ((1 << PD7)|(1 << PD6)|(1 << PD5)|(1 << PD4))
// #define CTRL_BITS ((1 << PB1)|(1 << PB0))

#define DATA_BITS ((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC3))
#define CTRL_BITS ((1 << PD0)|(1 << PD1))
#define MASKUPPERBITS 0b11110000
#define MASKLOWERBITS 0b00001111
// #define MASKBITS 0b00001111


/*
  lcd_init - Do various things to initialize the LCD display
*/
void lcd_init(void)
{ 
    // DDRD |= 0b00000011; // Set DDRD pin PD0 and PD1 i.e pins 2 and 3 on ATmega 
    // DDRC |= 0b00001111; // Set DDRC pin 0,1,2,3 i.e pins 23,24,25,26 on ATmega
    DDRC |= DATA_BITS;	    // Set the DDR register bits for ports B and D
    DDRD |= CTRL_BITS;      // Take care not to affect any unnecessary bits
    // Take care not to affect any unnecessary bits
    // flash_redled();

    _delay_ms(40);              // Delay at least 40ms after vdd rises

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b00110000
    // _delay_ms(5);               // Delay at least 4msec
    _delay_us(39);

    lcd_writenibble(0x20);      // Use lcd_writenibble to send 0b0011
    lcd_writenibble(0xC0);      // Use lcd_writenibble to send 0b0011
    
    _delay_us(39);             // Delay at least 100usec

    lcd_writenibble(0x20);      // Use lcd_writenibble to send 0b0011, no delay needed
    lcd_writenibble(0xC0);      // Use lcd_writenibble to send 0b0011
    
    _delay_us(37);             // Delay at least 100usec

    lcd_writenibble(0x00);      
    lcd_writenibble(0xD0);   
    
    _delay_us(37);             // Delay at least 100usec
    
    lcd_writenibble(0x00);      
    lcd_writenibble(0x10);      
    
    _delay_ms(2);               // Delay at least 2ms

    lcd_writenibble(0x00);      
    lcd_writenibble(0x70);      
    
    // function set: 4-bit interface, 2 lines: 
    // lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

    // lcd_writecommand(0x0f);     // Display and cursor on
}

/*
  lcd_moveto - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    unsigned char pos;
    if(row == 0) {
        pos = 0x80 + col;       // 1st row locations start at 0x80
    }
    else if(row == 1){
        pos = 0xc0 + col;       // 2nd row locations start at 0xc0
    }
    else if(row == 2){
        pos = 0x100 + col;
    }
    else {
        pos = 0x140 + col;
        
    }
    lcd_writecommand(pos);      // Send command
}

/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    int i = 0;
    while (str[i] != '\0') {    // Loop until next charater is NULL byte
        lcd_writedata(str[i]);  // Send the character
        i++;
    }
}

/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char cmd)
{
    /* Clear PD1 to 0 for a command transfer */
    PORTD &= ~(1 << PD1);   //just changed this -anjali

    /* Call lcd_writenibble to send UPPER four bits of "cmd" argument */
    lcd_writenibble(cmd);

    /* Call lcd_writenibble to send LOWER four bits of "cmd" argument */
    
    lcd_writenibble(cmd<<4);
    //lcd_writenibble((cmd & MASKLOWERBITS) << 4);

    /* Delay 2ms */
     _delay_ms(2);  

}

/*
  lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(unsigned char dat)
{
    /* Set PD1 to 1 for a data transfer */
    PORTD |= (1<< PD1);

    /* Call lcd_writenibble to send UPPER four bits of "dat" argument */
    lcd_writenibble(dat);
    

    /* Call lcd_writenibble to send LOWER four bits of "dat" argument */
    
    lcd_writenibble(dat<<4);
    //lcd_writenibble((dat & MASKLOWERBITS) << 4);

    /* Delay 2ms */
    _delay_ms(2);

    

}

/*
  lcd_writenibble - Output the UPPER four bits of "lcdbits" to the LCD
*/
void lcd_writenibble(unsigned char lcdbits)
{
    /* Load PORTC, bits 7-4 with bits 7-4 of "lcdbits" */
    PORTC &= ~MASKUPPERBITS;
    PORTC |= (lcdbits & MASKUPPERBITS);
    /* Make E signal (PD0) go to 1 and back to 0 */
    PORTD |= (1<< PD0);
    PORTD |= (1<< PD0);
    PORTD &= ~(1<<PD0);

}

void flash_redled(){
    PORTD &= ~(1 << PD2);   // Set PC0 to a 0
    _delay_ms(100);
    PORTD |= (1 << PD2);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTD &= ~(1 << PD2);   // Set PC0 to a 0
    _delay_ms(100);

}

int main(void){
    DDRD |= (1 << PD2);
    lcd_init();
    lcd_writecommand(1);
    lcd_moveto(0,2);
    char buf[20];
    snprintf(buf, 20, "hello");
    lcd_stringout(buf);
    //lcd_stringout("hello");
    return 0;
}        