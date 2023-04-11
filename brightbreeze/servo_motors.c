#include <avr/io.h>
#include <util/delay.h>

// #define F_CPU 9830400  // Define the clock frequency of the microcontroller
#define SERVO1_PIN PD5     // Define the pin connected to the servo motor
#define SERVO2_PIN PD3     // Define the pin connected to the servo motor

const uint8_t PULSE_MIN = 25;            
const uint8_t PULSE_MAX = 125;           
const uint8_t SERVO_DELAY = 100; 

void flash_ledpin2();
void change_blinds(uint8_t direction);      // direction = 1: open, direction = -1: close
void change_windows(uint8_t direction);     // direction = 1: open, direction = -1: close

void change_blinds_timer0(uint8_t direction);  
void change_windows_timer2(uint8_t direction);  

void servo1__timer0_init();
void servo2__timer2_init();


int main(void)
{
    
    servo1__timer0_init();
    servo2__timer2_init();
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, red led) for output
    

    while (1)
    {   
        change_windows_timer2(1);
        change_blinds_timer0(1);
        
        flash_ledpin2();
        
        change_windows_timer2(0);
        change_blinds_timer0(0);
        
        flash_ledpin2();
    
    }
}


void servo1__timer0_init(){
    DDRD |= (1 << SERVO1_PIN);   // Set up the data direction register (DDRD) for the servo pin as an output 

    TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
    TCCR0A &= ~(1<<COM0B0); 
    TCCR0B |= (1<<WGM02);

    // set prescalar to 256 (100) see page 142 of atmega328p documentation
	TCCR0B |= (1 << CS02);  //1           
	TCCR0B &= ~(1 << CS01); //0
    TCCR0B &= ~(1 << CS00); //0

    OCR0A = 249;
    OCR0B = 128;    
}

//see page 206 of atmega328p documentation
void servo2__timer2_init(){
    DDRD |= (1 << SERVO2_PIN);   // Set up the data direction register (DDRD) for the servo pin as an output 

    TCCR2A |= (0b11 << WGM20);  // Fast PWM mode, modulus = 256
    TCCR2A |= (0b10 << COM2B0); // Turn D11 on at 0x00 and off at OCR2A
    //OCR2A: see pg 197 of atmega328p documentatino
    OCR2A = 249;                // Initial pulse duty cycle of 50%
    OCR2B = 128;
    
    // set prescalar to 256 (CA22, CA21, CS20 corresponds to 110 for 256 according to page 207 in atmega documentation)
	TCCR2B |= (1 << CS22);  //1
	TCCR2B |= (1 << CS21);  //1 //try this instead maybe: TCCR2B &=100-TCCR2B &= ~(1 << CS21);  //0
    TCCR2B &= ~(1 << CS20); //0    
}

void change_blinds_timer0(uint8_t direction){
    uint8_t pulse_width;

    if(direction == 1){ //open blinds
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 1) {
            OCR0B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
    else{   //close blinds
        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 1) {
            OCR0B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
}
void change_windows_timer2(uint8_t direction){
    uint8_t pulse_width;

    if(direction == 1){ //open blinds
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 1) {
            OCR2B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
    else{   //close blinds
        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 1) {
            OCR2B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
}

void flash_ledpin2(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}