//LED is on PB4 (pin 18)
//This does share a pin with pin 1 of the USB connector.
//This is okay because the LED is an output.
void led_init(){
    //Fan is connected to PB7
    DDRB |= 1 << DDB4;          // Set PORTB bit 4 for output
    PORTB &= ~(1 << PB4);      // Set PB4 to a 0
}
void led_on(){
    PORTB |= 1 << PB4;      // Set PB4 to a 1 (on)
} 
void led_off(){
    PORTB &= ~(1 << PB4);   // Set PB4 to a 0
}
