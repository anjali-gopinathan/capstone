#include <avr/io.h>
#include <util/delay.h>
 
int check_motion(){
    bool motion_detected = ((PIND & (1 << PD4)));
    return (int)motion_detected;
    // if(motion_detected){
    //     return 1;
    // }

    // else{
    //     return 0;
    // }

}

//motion sensor is on pin 6 (PD4)
void motion_sensor_init(){
    DDRD &= ~(1 << DDD4);          // Set pin for input
}






