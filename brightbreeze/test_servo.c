#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 9830400  // Define the clock frequency of the microcontroller
#define SERVO_PIN PD5     // Define the pin connected to the servo motor

void flash_ledpin2();

int main(void)
{
    // Set up the data direction register (DDRD) for the servo pin as an output
    DDRD |= (1 << SERVO_PIN);

    DDRC |= 1 << DDC0;          // Set PORTC bit 0 (pin 23, green led) for output
    
    // Set up the Timer/Counter1 (TC1) for Fast PWM mode with a prescaler of 8
    TCCR1A=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM 
    TCCR1B=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);
    ICR1=40000;

    // Set the servo motor to its neutral position (1.5 ms pulse width)
    OCR1A = 1500;  // 375 = (1.5 ms / 20 us) * 8
    
    while (1)
    {
        // // Set the servo motor to its maximum position (2 ms pulse width)
        // OCR1A = 2000;        // 500 = (2 ms / 20 us) * 8
        // _delay_ms(2000);    // Wait for 2 seconds

        // // Set the servo motor to its minimum position (1 ms pulse width)
        // OCR1A = 4000;        // 250 = (1 ms / 20 us) * 8
        // _delay_ms(2000);    // Wait for 2 seconds

        // // Set the servo motor to its neutral position (1.5 ms pulse width)
        // //OCR1A = 1500;        // 375 = (1.5 ms / 20 us) * 8
        // //_delay_ms(2000);    // Wait for 2 seconds

        // flash_ledpin2();    // Flash the green LED
        // Move the servo from minimum to maximum position
    int i;
    for (i = 0; i < 180; i++)
    {
        int duty_cycle = 750 + i * 5;  // Calculate the duty cycle based on the servo position
        OCR1A = duty_cycle / 2;  // Set the duty cycle for the servo
        _delay_ms(20);  // Wait 20 ms before moving the servo to the next position
        
    }
    
    int j;
    int duty_cycle;
    // Move the servo from maximum to minimum position
    for (j = 180; j >= 0; j--)
    {
        duty_cycle = 750 + j * 5;  // Calculate the duty cycle based on the servo position
        OCR1A = duty_cycle / 2;  // Set the duty cycle for the servo
        _delay_ms(20);  // Wait 20 ms before moving the servo to the next position
        flash_ledpin2();
    }
    }
}

void flash_ledpin2(){
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
    PORTC |= (1 << PC0);    // Set PC0 to a 1
    _delay_ms(1000);
    PORTC &= ~(1 << PC0);   // Set PC0 to a 0
    _delay_ms(100);
}