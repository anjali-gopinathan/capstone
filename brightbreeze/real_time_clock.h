

#define DS1307_Read_Mode   0xD1  // DS1307 ID in read mode
#define DS1307_Write_Mode  0xD0  // DS1307 ID in write mode
#define DS1307Z_ADDR       0x68  //address for real time clock (0110 1000)

#define TimeFormat12		0x40	/* Define 12 hour format */
#define AMPM			    0x20

uint8_t second, minute, hour;

bool IsItPM(char hour_)
{
	if(hour_ & (AMPM))
	    return 1;
	else
	    return 0;
}
uint8_t bcd2decimal(uint8_t t){
	//this function gives 4-bit binary coded decimal to decimal 
	uint8_t i = (t / 16)*10 + (t % 16);
	return i;
}
uint8_t decimal2bcd(uint8_t t) {
    //this function gives decimal to 4-bit binary coded decimal
    uint8_t i = (t / 10) * 16 + (t % 10);
    return i;
}
void RTC_Read_Clock(uint8_t read_clock_address)
{
    uint8_t read_data[3];

    i2c_io(DS1307_Write_Mode, &read_clock_address, 1, NULL, 0, NULL, 0);
    i2c_io(DS1307_Read_Mode, NULL, 0, NULL, 0, read_data, 3);

    second = read_data[0];
    minute = read_data[1];
    hour = read_data[2];
}
void RTC_Write_Time(uint8_t hh, uint8_t mm, uint8_t ss){
    uint8_t write_data[4];
    write_data[0] = 0x00; // Start from address 0x00 (seconds)
    write_data[1] = decimal2bcd(ss);
    write_data[2] = decimal2bcd(mm);
    write_data[3] = decimal2bcd(hh);
    i2c_io(DS1307_Write_Mode, write_data, 4, NULL, 0, NULL, 0);
}

// int main(void){
//     LCDSetup(LCD_CURSOR_BLINK);
//     LCDClear();
// 	char buffer[20];
    
//     //initialization
//     i2c_init(BDIV);
//     RTC_Write_Time(11, 35, 56); //1002 hard coding curr time
//     while(1){    
//         RTC_Read_Clock(0);	/* Read clock with second add. i.e location is 0 */
//         if (hour & TimeFormat12)	
//         {
//             sprintf(buffer, "%02d:%02d:%02d  ", bcd2decimal((hour & 0b00011111)), bcd2decimal(minute), bcd2decimal(second));
//             if(IsItPM(hour))
//                 strcat(buffer, "PM");
//             else
//                 strcat(buffer, "AM");
//             LCDGotoXY(1,1);
//             LCDWriteString(buffer);
//         }
        
//         else
//         {
//             sprintf(buffer, "%02d:%02d:%02d  ", bcd2decimal((hour & 0b00011111)), bcd2decimal(minute), bcd2decimal(second));
//             LCDGotoXY(1,1);
//             LCDWriteString(buffer);
//         }
//     }
//     return 0;
// }