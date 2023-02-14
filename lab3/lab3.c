#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
  /*Pseudocode:
    Set all I / O pins to zero
    Set all I / O pins to be outputs
    Start of loop
    Trigger pin = 1
    Trigger pin = 0
    I / O pin #2 = 1
    I / O pin #2 = 0
    I / O pin #3 = 1
    I / O pin #3 = 0
    I / O pin # n = 1
    I / O pin # n = 0
    End of loop
  */

  //Pins Available: PB0-PB5, PB7, PC0-PC5, PD0-PD7

  // setting all I/O pins to zero:
    // PORT B
    PORTB &= ~(1 << PB0 );
    PORTB &= ~(1 << PB1);
    PORTB &= ~(1 << PB2);
    PORTB &= ~(1 << PB3);
    PORTB &= ~(1 << PB4);
    PORTB &= ~(1 << PB5);
    PORTB &= ~(1 << PB7);
    
    // PORT C
    PORTC &= ~(1 << PC0);  
    PORTC &= ~(1 << PC1);   
    PORTC &= ~(1 << PC2);   
    PORTC &= ~(1 << PC3);   
    PORTC &= ~(1 << PC4);   
    PORTC &= ~(1 << PC5);   
    
    // PORT D
    PORTD &= ~(1 << PD0);  
    PORTD &= ~(1 << PD1);  
    PORTD &= ~(1 << PD2);  
    PORTD &= ~(1 << PD3);  
    PORTD &= ~(1 << PD4);  
    PORTD &= ~(1 << PD5);  
    PORTD &= ~(1 << PD6);  
    PORTD &= ~(1 << PD7);  
    
    // setting all I/O pins to be outputs
    // PORT B
    DDRB |= 1 << DDB0;
    DDRB |= 1 << DDB1;
    DDRB |= 1 << DDB2;
    DDRB |= 1 << DDB3;
    DDRB |= 1 << DDB4;
    DDRB |= 1 << DDB5;
    DDRB |= 1 << DDB7;
    // PORT C
    DDRC |= 1 << DDC0;
    DDRC |= 1 << DDC1;
    DDRC |= 1 << DDC2;
    DDRC |= 1 << DDC3;
    DDRC |= 1 << DDC4;
    DDRC |= 1 << DDC5;

    // PORT D
    DDRD |= 1 << DDD0;
    DDRD |= 1 << DDD1;
    DDRD |= 1 << DDD2;
    DDRD |= 1 << DDD3;
    DDRD |= 1 << DDD4;
    DDRD |= 1 << DDD5;
    DDRD |= 1 << DDD6;
    DDRD |= 1 << DDD7;
    const int my_delay_ms = 100;
    while (1) {
      //setting trigger pin
      PORTC |= 1 << PC0;      // Set PC0 to a 1
      _delay_ms(my_delay_ms);
      PORTC &= ~(1 << PC0);   // Set PC0 to a 0
      _delay_ms(my_delay_ms);

    //PORT C
      //set and clear IO pin PC1 
      PORTC |= 1 << PC1;
      _delay_ms(my_delay_ms);
      PORTC &= ~(1 << PC1);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PC2
      PORTC |= 1 << PC2;
      _delay_ms(my_delay_ms);
      PORTC &= ~(1 << PC2);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PC3
      PORTC |= 1 << PC3;
      _delay_ms(my_delay_ms);
      PORTC &= ~(1 << PC3);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PC4
      PORTC |= 1 << PC4;
      _delay_ms(my_delay_ms);
      PORTC &= ~(1 << PC4);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PC5
      PORTC |= 1 << PC5;
      _delay_ms(my_delay_ms);
      PORTC &= ~(1 << PC5);
      _delay_ms(my_delay_ms);

    //PORT B
      //set and clear IO pin PB0
      PORTB |= 1 << PB0;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PB0);
      _delay_ms(my_delay_ms);
      
      //set and clear IO pin PB1
      PORTB |= 1 << PC1;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PC1);
      _delay_ms(my_delay_ms);
      
      //set and clear IO pin PB2
      PORTB |= 1 << PC5;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PC2);
      _delay_ms(my_delay_ms);
      
      //set and clear IO pin PB3
      PORTB |= 1 << PB3;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PB3);
      _delay_ms(my_delay_ms);
      
      //set and clear IO pin PB4
      PORTB |= 1 << PB4;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PB4);
      _delay_ms(my_delay_ms);
      
      //set and clear IO pin PB5
      PORTB |= 1 << PB5;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PB5);
      _delay_ms(my_delay_ms);
      
      //set and clear IO pin PB7
      PORTB |= 1 << PB7;
      _delay_ms(my_delay_ms);
      PORTB &= ~(1 << PB7);
      _delay_ms(my_delay_ms);

    // PORT D (0 through 7)
      //set and clear IO pin PD0
      PORTD |= 1 << PD0;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD0);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD1 
      PORTD |= 1 << PD1;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD1);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD2 
      PORTD |= 1 << PD2;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD2);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD3
      PORTD |= 1 << PD3;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD3);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD4
      PORTD |= 1 << PD4;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD4);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD5
      PORTD |= 1 << PD5;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD5);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD6
      PORTD |= 1 << PD6;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD6);
      _delay_ms(my_delay_ms);

      //set and clear IO pin PD7
      PORTD |= 1 << PD7;
      _delay_ms(my_delay_ms);
      PORTD &= ~(1 << PD7);
      _delay_ms(my_delay_ms);
    }

    return 0;   /* never reached */
}
