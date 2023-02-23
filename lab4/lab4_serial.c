/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

/*
serial_init - Initialize the USART port
EE 459Lx, Rev. 6/3/2022 5
*/
void serial_init(unsigned short ubrr){
  UBRR0 = ubrr ; // Set baud rate
  UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
  UCSR0B |= (1 << RXEN0 ); // Turn on receiver
  UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
  // one stop bit , 8 data bits
}

/*
serial_out - Output a byte to the USART0 port
*/
void serial_out(char ch){
  while(( UCSR0A & (1 << UDRE0 )) == 0);  // do nothing while nothing is received
  UDR0 = ch;
}


/*
  sci_outs - Print the contents of the character string "s" out the SCI
  port. The string must be terminated by a zero byte.
*/
void sci_outs(char *s){
  char ch;
  while((ch = *s++) != '\0') {
    serial_out(ch);
  }
}

/*
serial_in - Read a byte from the USART0 and return it
*/
char serial_in()
{
  while ( !( UCSR0A & (1 << RXC0 )) );
  return UDR0;
}

int main(void)
{
  serial_init (9600);
  DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output

  char serialSend[48];
  while (1) {
    PORTC |= 1 << PC0;      // Set PC0 to a 1 (light up led)
    char c = serial_in();   //received something!
  	PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);         // turn off led for 0.1 seconds
    PORTC |= 1 << PC0;      // Set PC0 to a 1 (light up led)

    if( c == 'a' ||
        c == 'e' ||
        c == 'i' ||
        c == 'o' ||
        c == 'u'   
    ){  
      sprintf(serialSend, "You entered the vowel \"%c\"\r\n", c);
    }
    else{
      sprintf(serialSend, "That was the consonant \"%c\"\r\n", c);
    }
    sci_outs(serialSend);
  }
  return 0;   /* never reached */
}
