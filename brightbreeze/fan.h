// 12V DC Motor
// Ground (black wire) of fan goes to third pin (collector) on transistor
// +12V (red wire) of fan goes to 12v power source
// First pin (emitter) of transistor goes to common ground
// Base of transistor goes to a resistor, which then goes to pin 7 on PORTB
// Ground from the 12v power source goes to common ground

void fan_init(){
    //Fan is connected to PB7
    DDRB |= 1 << DDB7;          // Set PORTB bit 7 for output
    PORTB &= ~(1 << PB7);      // Set PB7 to a 0
}
void fan_on(){
    PORTB |= 1 << PB7;      // Set PB7 to a 1 (on)
} 
void fan_off(){
    PORTB &= ~(1 << PB7);   // Set PB7 to a 0
}
