#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 9830400  // Define the clock frequency of the microcontroller
#define SERVO1_PIN PD5     // Define the pin connected to the servo motor
#define SERVO2_PIN PD3     // Define the pin connected to the servo motor

// const uint16_t PULSE_MIN = 128;            // min width pulse (.5ms)
// const uint16_t PULSE_MAX = 3000;           // max width pulse (2.3ms)
// const uint16_t SERVO_DELAY = 50;       // allow servo to move 
const uint16_t PULSE_MIN = 128;            
const uint16_t PULSE_MAX = 3000;           
const uint16_t SERVO_DELAY = 100; 

void flash_ledpin2();
void servo1_init();
void change_blinds(uint8_t direction);      // direction = 1: open, direction = -1: close
void change_windows(uint8_t direction);     // direction = 1: open, direction = -1: close

void change_blinds_timer0(uint8_t direction);  
void change_windows_timer2(uint8_t direction);  

void servo1__timer0_init();
void servo2__timer2_init();


int main(void)
{
    
    servo1__timer0_init();
    //servo2__timer2_init();
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, red led) for output
    

    while (1)
    {   
        change_blinds_timer0(1);
        flash_ledpin2();
        change_blinds_timer0(0);
        flash_ledpin2();
    
    }
}

// void servo1_init(){
//     DDRB |= (1 << SERVO_PIN);   // Set up the data direction register (DDRD) for the servo pin as an output

//     TCCR1B |= (1<<WGM13)|(1<<WGM12);
//     TCCR1A |= (1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
//     TCCR1A &= ~(1<<COM1B0); 
    
//     // set prescalar 
//     TCCR1B &= ~(1 << CS10);
// 	TCCR1B &= ~(1 << CS12);
// 	TCCR1B |= (1 << CS11);

//     // OCR1B = PULSE_MIN; OCR1A = PULSE_MAX
//     OCR1A = 40000;
//     OCR1B = 3000;    
// }

void servo1__timer0_init(){
    DDRD |= (1 << SERVO1_PIN);   // Set up the data direction register (DDRD) for the servo pin as an output 

    TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
    TCCR0A &= ~(1<<COM0B0); 
    TCCR0B |= (1<<WGM02);
    
    // set prescalar 
    TCCR0B &= ~(1 << CS00);
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= (1 << CS01);            

    // OCR1B = PULSE_MIN; OCR1A = PULSE_MAX
    OCR0A = PULSE_MAX;
    OCR0B = PULSE_MIN;    
}

void servo2__timer2_init(){
    DDRD |= (1 << SERVO2_PIN);   // Set up the data direction register (DDRD) for the servo pin as an output 

    TCCR2A |= (1<<COM2B1)|(1<<WGM21)|(1<<WGM20);
    TCCR2A &= ~(1<<COM2B0); 
    TCCR2B |= (1<<WGM22);
    
    // set prescalar 
    TCCR2B &= ~(1 << CS20);
	TCCR2B &= ~(1 << CS22);
	TCCR2B |= (1 << CS21);

    // OCR1B = PULSE_MIN; OCR1A = PULSE_MAX
    OCR2A = 2000;
    OCR2B = 10;    
}

void change_blinds_timer0(uint8_t direction){
    uint8_t pulse_width;

    if(direction == 1){ //open blinds
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 50) {
            OCR0B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
    else{   //close blinds
        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 50) {
            OCR0B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
}
void change_windows_timer2(uint8_t direction){
    uint8_t pulse_width;

    if(direction == 1){ //open blinds
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 50) {
            OCR2B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
    else{   //close blinds
        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 50) {
            OCR2B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }
    }
}

// void change_blinds(uint8_t direction){
//     uint16_t pulse_width;

//     if(direction == 1){ //open blinds
//         // Move the servo from left to right
//         for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 20) {
//             OCR1B = pulse_width;
//             _delay_ms(SERVO_DELAY);
//         }
//     }
//     else{   //close blinds
//         // Move the servo from right to left
//         for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 20) {
//             OCR1B = pulse_width;
//             _delay_ms(SERVO_DELAY);
//         }
//     }
// }

// void change_windows(uint8_t direction){
//     uint16_t pulse_width;

//     if(direction == 1){ //open windows
//         // Move the servo from left to right
//         for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 100) {
//             OCR1B = pulse_width;
//             _delay_ms(SERVO_DELAY);
//         }
//     }
//     else{ //close windows
//         // Move the servo from right to left
//         for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 100) {
//             OCR1B = pulse_width;
//             _delay_ms(SERVO_DELAY);
//         }
//     }
// }

void flash_ledpin2(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);   // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);

}