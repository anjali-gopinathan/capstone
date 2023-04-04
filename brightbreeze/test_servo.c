#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 9830400  // Define the clock frequency of the microcontroller
#define SERVO_PIN PB2     // Define the pin connected to the servo motor
const uint16_t PULSE_MIN = 128;            // min width pulse (.5ms)
const uint16_t PULSE_MAX = 3000;           // max width pulse (2.3ms)
const uint16_t SERVO_DELAY = 100;       // allow servo to move 

void flash_ledpin2();
void servo1_init();
void change_blinds(uint8_t direction);      // direction = 1: open, direction = -1: close
void change_windows(uint8_t direction);     // direction = 1: open, direction = -1: close

int main(void)
{
    
    servo1_init();
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, red led) for output

    while (1)
    {   
        change_blinds(1);
        flash_ledpin2();
        change_blinds(0);
        flash_ledpin2();
    
    }
}

void servo1_init(){
    DDRB |= (1 << SERVO_PIN);   // Set up the data direction register (DDRD) for the servo pin as an output

    TCCR1B |= (1<<WGM13)|(1<<WGM12);
    TCCR1A |= (1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
    TCCR1A &= ~(1<<COM1B0); 
    
    // set prescalar 
    TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS12);
	TCCR1B |= (1 << CS11);

    // OCR1B = PULSE_MIN; OCR1A = PULSE_MAX
    OCR1A = 40000;
    OCR1B = 3000;    
}
void change_blinds(uint8_t direction){
    uint16_t pulse_width;

    if(direction == 1){ //open blinds
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 100) {
            OCR1B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
    else{   //close blinds
        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 100) {
            OCR1B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
}

void change_windows(uint8_t direction){
    uint16_t pulse_width;

    if(direction == 1){ //open windows
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 100) {
            OCR1B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
    else{ //close windows
        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 100) {
            OCR1B = pulse_width;
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