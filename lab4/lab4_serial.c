#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

/*
serial_init - Initialize the USART port
EE 459Lx, Rev. 6/3/2022 5
*/
void serial_init(unsigned short ubrr){
  UBRR0 = ubrr ; // Set baud rate
  UCSR0B |= (1 << TXEN0 );  // Turn on transmitter
  UCSR0B |= (1 << RXEN0 );  // Turn on receiver
  UCSR0C = (3 << UCSZ00 );  // Set for async . operation , no parity , one stop bit , 8 data bits
}

/*
serial_out - Output a byte to the USART0 port
*/
void serial_out(char ch){
  while(( UCSR0A & (1 << UDRE0 )) == 0);  // do nothing while nothing is received
  UDR0 = ch;
}


/*
  serial_stringout - Print the contents of the character string "s" out the SCI
  port. The string must be terminated by a zero byte.
*/
void serial_stringout(char *s){
  int i=0;
  while(s[i] != '\0'){
    serial_out(s[i++]);
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
  unsigned short ubrr = ( ( 9830400 / 16 ) / 9600) - 1; 
  serial_init(ubrr); 
  _delay_ms(100);         // delay 0.1 seconds

  while (1) {
    char c = serial_in();   //received something!
    char buf[32];
    if( (c == 'a') ||
        (c == 'e') ||
        (c == 'i') ||
        (c == 'o') ||
        (c == 'u')   
    ){  
      snprintf(buf, 32, "You entered the vowel '%c'\r\n", c);
    }
    else{
      snprintf(buf, 32, "That was the consonant '%c'\r\n", c);
    }
    serial_stringout(buf);
  }
  return 0;   /* never reached */
}
