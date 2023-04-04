#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 9830400  // Define the clock frequency of the microcontroller
#define SERVO_PIN PB2     // Define the pin connected to the servo motor
const uint16_t PULSE_MIN = 128;            // min width pulse (.5ms)
const uint16_t PULSE_MAX = 3000;           // max width pulse (2.3ms)
const uint16_t SERVO_DELAY = 250;       // allow servo to move 

void flash_ledpin2();
uint16_t set_pwm_1(uint8_t angle_value);
uint8_t constrain(uint8_t value, uint8_t min, uint8_t max);

int main(void)
{
    // Set up the data direction register (DDRD) for the servo pin as an output
    DDRB |= (1 << SERVO_PIN);

    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, red led) for output
    // Set up the Timer/Counter1 (TC1) for Fast PWM mode with a prescaler of 8
    //TCCR1A=(1<<COM1B0)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM 
    // TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // this line sets

    TCCR1A = (1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
    TCCR1A &= ~(1<<COM1B0);
    TCCR1B = (1<<WGM13)|(1<<WGM12);
    
    // set prescalar using cs00, cs01, or cs02
    // TCCR1B = (1<<CS02)|(1<<CS01);

    // Set the servo motor to its neutral position (1.5 ms pulse width)
   // OCR1A = PULSE_MIN;  // 375 = (1.5 ms / 20 us) * 8

    OCR1B = PULSE_MIN;
    
    while (1)
    {   
        uint16_t pulse_width;
        // Move the servo from left to right
        for (pulse_width = PULSE_MIN; pulse_width <= PULSE_MAX; pulse_width += 10) {
            OCR1B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }

        // Move the servo from right to left
        for (pulse_width = PULSE_MAX; pulse_width >= PULSE_MIN; pulse_width -= 10) {
            OCR1B = pulse_width;
            _delay_ms(SERVO_DELAY);
        }



        uint16_t i;
        for (i = 0; i < 180; i++)
        {
            
            // uint8_t duty_cycle = (750 + i * 5)/2;  // Calculate the duty cycle based on the servo position
            // uint8_t servo_pulse = map(duty_cycle % 180, 0, 180, PULSE_MIN, PULSE_MAX);
            // set_pwm_1((duty_cycle % 30) + 6);
            set_pwm_1(i);
            // OCR1A = duty_cycle / 2;  // Set the duty cycle for the servo
            _delay_ms(SERVO_DELAY);  // Wait before moving the servo to the next position
             flash_ledpin2();
            
        }
        
        uint16_t j;
        // int duty_cycle;
        // Move the servo from maximum to minimum position
        for (j = 180; j >= 0; j--)
        {
            // duty_cycle = (750 + j * 5)/2;  // Calculate the duty cycle based on the servo position
            
            
            // OCR1A = duty_cycle / 2;  // Set the duty cycle for the servo
            // set_pwm_1((duty_cycle % 30) + 6);
            set_pwm_1(j);
            _delay_ms(SERVO_DELAY);  // Wait 20 ms before moving the servo to the next position
            
            
        }
    }
}
	
// adapted from https://wellys.com/posts/avr_c_servos/
uint16_t set_pwm_1(uint8_t angle_value)
{
//    uint8_t angle = constrain(angle_value, PULSE_MIN, PULSE_MAX);
    uint16_t pulse_width = PULSE_MIN + (uint32_t)(angle_value * (PULSE_MAX - PULSE_MIN)) / 180;

    OCR1B = pulse_width;
   _delay_ms(SERVO_DELAY);
   return 0;        // fix for later: make this return the analog reading
}

uint8_t constrain(uint8_t value, uint8_t min, uint8_t max){
    if(value<min) return min;
    else if (value>max) return max;
    return value;
}
void flash_ledpin2(){
    // PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    // _delay_ms(100);
    PORTC |= (1 << PC0);    // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
}