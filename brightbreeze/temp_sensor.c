

// #define DHT11_PIN 6
#define OUTTEMP_PIN PD2    //pin4 is PD2
#define INTEMP_PIN PB0 // inside temp pin (pin 14) is PB0
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

void temp_sensor_init();
uint8_t get_Temp(uint8_t location);


void temp_sensor_init(){        
    DDRD &= ~(1 << DDD2);          // Set outside pin PD2 for input (pin 4)
    DDRB &= ~(1 << DDB0);          // Set inside pin PB0 for input (pin 14)

}

/*
 * ATmega16_DHT11_Project_File.c
 *
 * http://www.electronicwings.com
 */ 

void Request(uint8_t location)				/* Microcontroller send start pulse/request */
{
    // location = 0 means outside sensor which is on portD and location=1 means inside sensor which is portB
	if (location == 0){ // 0 is outside 
        DDRD |= (1<<OUTTEMP_PIN);
        PORTD &= ~(1<<OUTTEMP_PIN);	/* set to low pin */
        _delay_ms(20);			/* wait for 20ms */
        PORTD |= (1<<OUTTEMP_PIN);	/* set to high pin */
    }
    else if (location == 1) { // 1 is inside
        DDRB |= (1<<INTEMP_PIN);
        PORTB &= ~(1<<INTEMP_PIN);	/* set to low pin */
        _delay_ms(20);			/* wait for 20ms */
        PORTB |= (1<<INTEMP_PIN);	/* set to high pin */
    }

}

void Response(uint8_t location)				/* receive response from DHT11 */
{
    // location = 0 means outside sensor which is on portD and location=1 means inside sensor which is portB
	if (location == 0){ // 0 is outside 
        DDRD &= ~(1<<OUTTEMP_PIN);
        while(PIND & (1<<OUTTEMP_PIN));
        while((PIND & (1<<OUTTEMP_PIN))==0);
        while(PIND & (1<<OUTTEMP_PIN));
    }
    else if(location ==1){
        DDRB &= ~(1<<INTEMP_PIN);
        while(PINB & (1<<INTEMP_PIN));
        while((PINB & (1<<INTEMP_PIN))==0);
        while(PINB & (1<<INTEMP_PIN));
    }
}

uint8_t Receive_data(uint8_t location)			/* receive data */
{	
    // location = 0 means outside sensor which is on portD and location=1 means inside sensor which is portB
	if (location == 0){ // 0 is outside 
        uint8_t q;
        for (q=0; q<8; q++)
        {
            while((PIND & (1<<OUTTEMP_PIN)) == 0);  /* check received bit 0 or 1 */
            _delay_us(30);
            if(PIND & (1<<OUTTEMP_PIN))/* if high pulse is greater than 30ms */
                c = (c<<1)|(0x01);	/* then its logic HIGH */
            else			/* otherwise its logic LOW */
                c = (c<<1);
            while(PIND & (1<<OUTTEMP_PIN));
        }
    }
    else if (location ==1){
        uint8_t q;
        for (q=0; q<8; q++)
        {
            while((PINB & (1<<INTEMP_PIN)) == 0);  /* check received bit 0 or 1 */
            _delay_us(30);
            if(PINB & (1<<INTEMP_PIN))/* if high pulse is greater than 30ms */
            c = (c<<1)|(0x01);	/* then its logic HIGH */
            else			/* otherwise its logic LOW */
            c = (c<<1);
            while(PINB & (1<<INTEMP_PIN));
        }
    }
	return c;
}


// location = 0 means outside sensor which is on portD and location=1 means inside sensor which is portB
uint8_t get_Temp(uint8_t location){
    Request(location);		/* send start pulse */
    Response(location);		/* receive response */
	I_RH=Receive_data(location);	/* store first eight bit in I_RH */
	D_RH=Receive_data(location);	/* store next eight bit in D_RH */
	I_Temp=Receive_data(location);	/* store next eight bit in I_Temp */
	D_Temp=Receive_data(location);	/* store next eight bit in D_Temp */
	CheckSum=Receive_data(location);/* store next eight bit in CheckSum */

    uint8_t temp_fahrenheit = 0;
    if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum){
		LCDGotoXY(1,1);
		LCDWriteString("Error");
	}
    else{
        temp_fahrenheit = (uint8_t)(((((float)I_Temp  + (float)(D_Temp*0.1f)))  * (float)((float)(9.0f)/5.0f)) +32);
    }
    return temp_fahrenheit;    
}