#include <avr/io.h>
#include <util/delay.h>
#include "fan.h"

int main(void){
    fan_init();
    while(1){
        fan_on();
        _delay_ms(6000);
        fan_off();
        _delay_ms(5000);
    }
}