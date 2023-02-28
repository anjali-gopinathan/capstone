#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

/*
    serial_init - Initialize the USART port
*/
void serial_init ( unsigned short ubrr ) {
    UBRR0 = ubrr ; // Set baud rate
    UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
    UCSR0B |= (1 << RXEN0 ); // Turn on receiver
    UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity, one stop bit , 8 data bits
}

/*
    serial_out - Output a byte to the USART0 port
*/
void serial_out ( char ch )
{
    while (( UCSR0A & (1 << UDRE0 )) == 0);
    UDR0 = ch;
}

/*
    serial_in - Read a byte from the USART0 and return it
*/
char serial_in ()
{
    while ( !( UCSR0A & (1 << RXC0 )) );
    return UDR0;
}

/*
    wrapper to send array of chars
*/
void serial_stringout(char *s) 
{
    int i = 0; 
    while(s[i] != '\0') {
        serial_out(s[i++]);
    }
}


int main(void)
{
    // Voltage Level Test
    unsigned short ubrr = ( ( 7372800 / 16 ) / 9600) - 1; 
    serial_init(ubrr); 

    _delay_ms(100);

    while (1) {
        char in = serial_in(); 

        char buf[30];
        if ((in == 'a') || (in == 'e') || (in == 'i') || (in == 'o') || (in == 'u') || (in == 'y')) {
            snprintf(buf, 30, "You entered the vowel '%c'\n", in);
            serial_stringout(buf);
        } else {
            snprintf(buf, 30, "You entered the consonant '%c'\n", in);
            serial_stringout(buf);
        }
    }

    return 0;   /* never reached */
}