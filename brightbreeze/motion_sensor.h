//motion sensor is on pin 6 (PD4)
void motion_sensor_init(){
    DDRD &= ~(1 << DDD4);          // Set pin for input
}
int check_motion(){
    bool motion_detected = ((PIND & (1 << PD4)));
    if (motion_detected) {
        return 1;
    } else {
        return 0;
    }
}







