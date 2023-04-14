
//ground of fan goes to third pin on transistor
//red wire goes to 12v
// ground the black 12v

void fan_init(){
    //Fan is connected to PB7
    DDRB |= 1 << DDB7;          // Set PORTB bit 7 for output
    PORTB &= ~(1 << PB7);      // Set PB7 to a 0

}

void fan_on(){
    PORTB |= 1 << PB7;      // Set PB7 to a 1 (on)
    _delay_ms(5000);

} 

void fan_off(){
    PORTB &= ~(1 << PB7);   // Set PB7 to a 0
    _delay_ms(5000);
}
