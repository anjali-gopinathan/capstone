/*___________________________________________________________________________________________________

Title:
	OnLCDLib.h v2.2

Description:
	This is a library used for interfacing with standard alpha numeric LCD modules using 4 or 8 bit modes.
	Supports 16x1, 16x2, 16x4, 20x4, 20x2, 32x2, 40x2 LCD displays.
	
	For complete details visit
	https://www.programming-electronics-diy.xyz/2016/08/lcd-interfacing-library-4-and-8-bit-mode.html

Author:
 	Liviu Istrate
	istrateliviu24@yahoo.com

Donate:
	Software development takes time and effort so if you find this useful consider a small donation at:
	paypal.me/alientransducer
_____________________________________________________________________________________________________*/


/* ----------------------------- LICENSE - GNU GPL v3 -----------------------------------------------

* This license must be included in any redistribution.

* Copyright (c) 2016 Liviu Istrate, www.programming-electronics-diy.xyz (istrateliviu24@yahoo.com)

* Project URL: https://www.programming-electronics-diy.xyz/2016/08/lcd-interfacing-library-4-and-8-bit-mode.html

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.

--------------------------------- END OF LICENSE --------------------------------------------------*/


/*_______________________________________________________________________________

FEATURES
--------
* 	Converts integers to strings (positive or negative) and displays them on screen
	Optionally the numbers can be padded with zeros
* 	It does not use external libraries to convert integers to strings thus reducing size on MCU
*	Data can be displayed at a certain position using only one function
*	Support for 8 and 4 bit mode
*	Support for different LCD screens
*	LCD backlight dimming or turn off using PWM
*	Wrap the text to a new line
*	Display double height digits and separator for digital clocks using custom fonts


HOW TO USE
----------
1. Setting things up
- Bellow in the setup section modify the setup as needed
- In your main function, use  "LCDSetup(cursorStyle)":
	"cursorStyle" can be: LCD_CURSOR_BLINK, LCD_CURSOR_ULINE, LCD_CURSOR_NONE
	
2. Interfacing with LCD
STRINGS
- Send a string to the LCD:
	"LCDWriteString(aString)"
- Send a string to a specific location. x is character position, y is line/row number.
  x and y can start from 0 or 1 depending how user prefers:
	"LCDWriteStringXY(x, y, aString)"

INTEGERS
- Send an integer number:
	"LCDWriteInt(number, nr_of_digits)"
	"nr_of_digits" - number of digits. If the number to be displayed has less digits than "nr_of_digits"
	then it will be padded with zeros. E.g:
	uint16_t ADC_results = 120;
	LCDWriteInt(ADC_results, 5);
	will display "00120"
	LCDWriteInt(ADC_results, 3);
	will display "120".
	You can also display negative numbers.
- Send an integer number to a specific location:
	"LCDWriteIntXY(x, y, number, nr_of_digits)"
	
FLOATS (We all float down here)
- Display a float number
	LCDWriteFloat(float_nr, nr_of_digits)
	float_nr: can be a positive or negative float number with two decimal places precision. E.g: 1.23, 0.12, -0.3.
	nr_of_digits: make it 0 if you don't want the number to be padded with zeros. E.g:
	float temperature = 27.5;
	LCDWriteFloat(temperature, 0);
	will display "27.5".
	LCDWriteFloat(temperature, 3);
	will display "027.5" to make the part before the point 3 digits (027)
- Send a float number to a specific location:
	"LCDWriteFloatXY(x, y, float_nr, nr_of_digits)"
	
BIG DIGITS
- Double height digits:
	- Custom double height digits 1 character wide font can be found in "double_height_sharp_digits.h".
	Copy data from there in this file in the variable "LCD_custom_chars". Custom char must be 
	uncommented in the setup section. "nrOfDigits" has the same function as in "LCDWriteInt" function.
		"LCDWriteIntBig(int16_t number, int8_t nrOfDigits)"
	
	- Custom double height digits 3 characters wide font can be found in "double_height_3_characters_round_digits.h".
	Same as above but needs to be used with "LCDWriteIntBig3Chars" function.
	"LCDWriteIntBig3Chars(int16_t number, int8_t nrOfDigits)"
	
BIG SEPARATOR
- Big 2 lines height separator. Can be used in conjunction with big digits to make a digital clock.
The function displays ":" but bigger, on two lines.
	"LCDWriteBigSeparator(void)"
	
LCD COMMANDS
- Move the cursor to a specific location:
	"LCDGotoXY(character_position, row_number)"
- Clear display and DDRAM:
	"LCDClear()"
- Go to character 1, line 1
	"LCDHome()"
	
BACKLIGHT PWM
- Dim LCD backlight using Fast PWM Timer0, channel B (OC0B pin), OCR0A as TOP.
Frequency is set to 400 to prevent flickering. "brightness" can be between 0 - 100.
"0" will turn off the backlight and the LCD without clearing DDRAM thus saving power.
"100" will turn LED backlight fully on and stop PWM.
Circuit: a small signal transistor can be used with emitter connected to ground.
Connect OC0B pin to the base of transistor through a resistor. Connect LCD backlight anode to Vcc and
cathode to collector.
	"LCDBacklightPWM(uint8_t brightness)"

3. Animations
- Scroll a string from right to left. Needs to be uncommented in setup section:
	"LCDScrollText(aString)"
	
4. Utils
- Find characters positions where lines start and end. Needs to be uncommented in setup section:
  Puts an x and increments cursor position showing the current position.
  LCD_X_POS_DELAY in how fast to increment the cursor.
	"LCDFindCharPositions()"

Tips:
- It is faster to replace something with spaces than to use "LCDClear" function.
Use clear function only to clear entire screen.
__________________________________________________________________________________*/

#ifndef OnLCDLib_H
#define OnLCDLib_H

/*************************************************************
	INCLUDES
**************************************************************/
#include <avr/io.h>
#include <util/delay.h>

/*************************************************************
	SYSTEM SETTINGS
**************************************************************/
#define TRUE			1
#define FALSE			0

/*************************************************************
	USER SETUP SECTION
**************************************************************/

// x is the port letter
#define LCD_DATA_DDR 						DDRC 	// Data bus (DB0 to DB7 on LCD pins)
#define LCD_DATA_PORT 						PORTC
#define LCD_DATA_PIN 						PINC 	// Holds the state of all pins on port x. Used to check busy flag.

// In 8-bit mode pins 0-7 will be used so set this to 0
// In 4-bit mode put here the lowest pin number of the LCD_DATA_PORT
// For example if the 4 data pins are on pins 0 to 3 LCD_DATA_START_PIN is 0
// Or if the 4 data pins are on pins 4, 5, 6, 7 LCD_DATA_START_PIN is 4
#define LCD_DATA_START_PIN					0	// 

// Register selection signal - RS (x is the port letter, n is pin number)
#define LCD_RS_CONTROL_DDR 					DDRD
#define LCD_RS_CONTROL_PORT 				PORTD
#define LCD_RS_PIN							PD0

// Read/write signal - RW (x is the port letter, n is pin number)
#define LCD_RW_CONTROL_DDR 					DDRD
#define LCD_RW_CONTROL_PORT 				PORTD
#define LCD_RW_PIN							PD1

// Enable signal - E (x is the port letter, n is pin number)
#define LCD_E_CONTROL_DDR 					DDRD
#define LCD_E_CONTROL_PORT 					PORTD
#define LCD_E_PIN 							PD6

// LCD type
#define LCD_NR_OF_CHARACTERS 				20 	// e.g 16 if LCD is 16x2 type
#define LCD_NR_OF_ROWS 						4 	// e.g 2 if LCD is 16x2 type

// MCU bits
#define PORT_SIZE							8 	// 8 bit microcontroller

// Backlight brightness control using PWM
// TRUE or FALSE - TRUE only if you want to dim the backlight using PWM or turn it on/off and the OC0B pin in available for this use
#define LCD_BACKLIGHT_PWM					FALSE

#define LCD_PWM_DDR							DDRD // OC0B DDR for backlight brightness control
#define LCD_PWM_PORT						PORTD
#define LCD_PWM_PIN							PD5 // OC0B pin for backlight brightness control

// Select 4 or 8 bit mode - 4 bit mode is preffered since only 4 pins are needed for the data port instead of 8
#define LCD_DATA_4_BITS						4
#define LCD_DATA_8_BITS						8
#define LCD_DATA_BUS_SIZE					LCD_DATA_4_BITS // LCD_DATA_4_BITS or LCD_DATA_4_BITS

// Text wrap - If the text length is greater than the numbers of LCD characters
// the cursor will be set on the beginning of the next line
#define LCD_WRAP_TEXT						TRUE // TRUE or FALSE

// To be able to use LCDWriteFloat(), make this equal to 1. Enabling this, will load
// the math library and will add over a KB to the program size. If you are not planning to
// display floats, then make this equal to 0. If you are using it, then uncomment MATH_LIB = -lm
// in your Make file to reduce the loaded math library to half. 
#define LCD_DISPLAY_FLOATS 					TRUE // TRUE or FALSE

// Use of custom characters. BIG_DIGITS_1_CHARACTERS and BIG_DIGITS_3_CHARACTERS must be FALSE
#define LCD_CUSTOM_CHARS 					FALSE // TRUE or FALSE

// Use of big double height digits. LCD_CUSTOM_CHARS must be FALSE
// Only one type of big digits can be TRUE
#define BIG_DIGITS_1_CHARACTERS	    		FALSE // TRUE or FALSE - double height 1 character wide font
#define BIG_DIGITS_3_CHARACTERS	    		FALSE // TRUE or FALSE - double height 3 character wide font

// * Animations *
// This types of LCD aren't meant for animations
// The crystals have slow rise and fall times. Use TFT LCDs for animations
// But if you want you can try this function
#define LCD_ANIMATIONS						TRUE 	// TRUE or FALSE
#define LCD_SCROLL_SPEED					200 	// milliseconds

// * Utils *
// A function used to find positions on LCD. 
// This is for debugging and creating functions and is not really meant for all users.
#define LCD_UTILS							FALSE 	// TRUE or FALSE
#define LCD_X_POS_DELAY						200 	// In milliseconds


// The array with the 8 custom characters
#if LCD_CUSTOM_CHARS == TRUE
	// The LCD has space in it's CG-RAM for 8 custom characters. They can be accessed by sending commands such as
	// 0 to display custom char on 0 place
	// 5x8 pixel character
	// Each row is formed by 8 bits but the first 3 MSB are not used so all will start with 0b00012345 (each number designates the column number)
	/*
		C1	C2	C3	C4	C5
	___________________________
		0	0	0	0	0 	Row 1
		
		0	0	0	0	0	Row 2
		
		0	0	0	0	0	Row 3
		
		0	0	0	0	0	Row 4
		
		0	0	0	0	0	Row 5
		
		0	0	0	0	0	Row 6
		
		0	0	0	0	0	Row 7
		
		0	0	0	0	0 	Row 8 <-- reserved for the cursor but can be used also if cursor is not used
	____________________________
	*/
	
	static const uint8_t LCD_custom_chars[] = {
	//	Row 1,	Row 2...							Row 8
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, //Char0
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, //Char1
		0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, //Char2
		0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, //Char3
		0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, //Char4
		0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, //Char5
		0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, //Char6
		0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, //Char7
	};
	
// This defines can be renamed and used as a parameter for LCDPrintCustomChar function
#define BATTERY_LEVEL_1			0 // these indicates an index for LCD_custom_chars array
#define BATTERY_LEVEL_2			1
#define BATTERY_LEVEL_3			2
#define BATTERY_LEVEL_4			3
#define BATTERY_LEVEL_5			4
#define BATTERY_LEVEL_6			5
#define BATTERY_LEVEL_7			6
#define BATTERY_LEVEL_8			7
	
#endif


// Addresses of special symbols inside the HD44780 memory other than alphanumeric characters and
// punctuation signs. These defines can be used as a parameter for the LCDPrintExtraChar function
#define LCD_SPECIAL_SYMBOL_DEGREE			0b11011111
#define LCD_SPECIAL_SYMBOL_ARROW_RIGHT		0b01111110
#define LCD_SPECIAL_SYMBOL_ARROW_LEFT		0b01111111
#define LCD_SPECIAL_SYMBOL_DIVIDE			0b11111101
#define LCD_SPECIAL_SYMBOL_OHM				0b11110100
#define LCD_SPECIAL_SYMBOL_EPSILON			0b11110110
#define LCD_SPECIAL_SYMBOL_PI				0b11110111
#define LCD_SPECIAL_SYMBOL_MICRO			0b11100100
#define LCD_SPECIAL_SYMBOL_ALPHA			0b11100000
#define LCD_SPECIAL_SYMBOL_BETA				0b11100010

/*************************************************************
	--END OF USER SETUP SECTION
**************************************************************/


/*************************************************************
	SYSTEM SETTINGS
**************************************************************/
// LCD Commands
#define LCD_SHIFT_RIGHT 	0b00011100
#define LCD_SHIFT_LEFT	 	0b00011000
#define LCD_DISPLAY_ON	 	0b00001100
#define LCD_DISPLAY_OFF	 	0b00001000
#define LCD_CURSOR_BLINK 	0b00000011
#define LCD_CURSOR_ULINE 	0b00000010
#define LCD_CURSOR_NONE	 	0b00000000

#define LCD_MAXIMUM_DIGITS	10


/*************************************************************
	FUNCTION PROTOTYPES
**************************************************************/
void LCDSetup(uint8_t cursorStyle);
void LCDPrintCustomChar(uint8_t char_index);
void LCDPrintExtraChar(uint8_t char_address);
void LCDWriteString(const char *msg);
void LCDWriteInt(int32_t number, int8_t nrOfDigits);
void LCDWriteBigSeparator(void);
void LCDGotoXY(uint8_t x, uint8_t y);

#if LCD_DISPLAY_FLOATS == TRUE
	void LCDWriteFloat(float float_number, int8_t nrOfDigits, uint8_t nrOfDecimals);
#endif

#if BIG_DIGITS_1_CHARACTERS == TRUE
	#include "double_height_sharp_digits.h"
	void LCDWriteIntBig(int32_t number, int8_t nrOfDigits);
#endif

#if BIG_DIGITS_3_CHARACTERS == TRUE
	#include "double_height_3_characters_round_digits.h"
	void LCDWriteIntBig3Chars(int32_t number, int8_t nrOfDigits);
#endif

#if LCD_BACKLIGHT_PWM == TRUE
	void LCDBacklightPWM(uint8_t brightness);
#endif

void LCDByte(uint8_t, uint8_t);
void LCDBusyLoop(void);
void FlashEnable(void);

// Animations
#if LCD_ANIMATIONS == TRUE
	#include <string.h>
	void LCDScrollText(const char *text);
#endif

// Utils
#if LCD_UTILS == TRUE
	void LCDFindCharPositions(void);
#endif



/*************************************************************
	MACROS
**************************************************************/
#define LCDCmd(c) 			LCDByte(c, 0)  // send a command to LCD
#define LCDData(d) 			LCDByte(d, 1) // send data to LCD

#define LCDClear() 			LCDCmd(0b00000001)
#define LCDHome() 			LCDCmd(0b10000000)

#define E_ON() 				(LCD_E_CONTROL_PORT |= (1 << LCD_E_PIN))
#define RS_ON() 			(LCD_RS_CONTROL_PORT |= (1 << LCD_RS_PIN))
#define RW_ON() 			(LCD_RW_CONTROL_PORT |= (1 << LCD_RW_PIN))
#define E_OFF() 			(LCD_E_CONTROL_PORT &= (~(1 << LCD_E_PIN)))
#define RS_OFF() 			(LCD_RS_CONTROL_PORT &= (~(1 << LCD_RS_PIN)))
#define RW_OFF() 			(LCD_RW_CONTROL_PORT &= (~(1 << LCD_RW_PIN)))

#define LCDWriteStringXY(x, y, msg){\
	LCDGotoXY(x, y);\
	LCDWriteString(msg);\
}

#define LCDWriteIntXY(x, y, nr, nrOfDigits){\
	LCDGotoXY(x, y);\
	LCDWriteInt(nr, nrOfDigits);\
}

#define LCDWriteFloatXY(x, y, float_number, nrOfDigits, nrOfDecimals)\
	LCDGotoXY(x, y);\
	LCDWriteFloat(float_number, nrOfDigits, nrOfDecimals)\


/*************************************************************
	GLOBAL VARIABLES
**************************************************************/
uint8_t cursorPosition = 1, cursorLine = 1;


/*************************************************************
	FUNCTIONS
**************************************************************/

#if LCD_BACKLIGHT_PWM == TRUE
	uint8_t cursorType = 0b00001100; // Display on, cursor off by default
#endif

/*---------------------------------------------------------------------------------------------------
*	INITIALIZE THE LCD. MUST BE RUN FIRST ONLY ONCE.
*
*	@param [cursorStyle] 		can be: LCD_CURSOR_BLINK, LCD_CURSOR_ULINE, LCD_CURSOR_NONE
*							
*   @return 					NONE
*----------------------------------------------------------------------------------------------------*/
void LCDSetup(uint8_t cursorStyle){
	// After power on wait for LCD to initialize. On 3.3v LCD clock will be slower so add more delay
	_delay_ms(100);
	
	// Save cursor style - used by LCDBacklightPWM function
	#if LCD_BACKLIGHT_PWM == TRUE
		cursorType = cursorStyle;
	#endif
	
	// Set MCU IO Ports
	LCD_DATA_DDR |= (0x0F << LCD_DATA_START_PIN);
	LCD_DATA_PORT &= (~(0x0F << LCD_DATA_START_PIN));
	LCD_RS_CONTROL_DDR |= (1 << LCD_RS_PIN);
	LCD_RW_CONTROL_DDR |= (1 << LCD_RW_PIN);
	LCD_E_CONTROL_DDR  |= (1 << LCD_E_PIN) | (1 << LCD_RW_PIN) | (1 << LCD_RS_PIN);
	E_OFF();
	RW_OFF();
	RS_OFF();
	
	#if LCD_DATA_BUS_SIZE == LCD_DATA_8_BITS
		LCDCmd(0b00001100 | cursorStyle); // Turn on display, set cursor type
		LCDCmd(0x38); // 8 bit mode. Function Set: 8-bit, 2 Line, 5x7 Dots
		
	#elif LCD_DATA_BUS_SIZE == LCD_DATA_4_BITS
		// LCD reset instructions according to datasheet's flowchart
		LCD_DATA_PORT |= 0b00000011 << LCD_DATA_START_PIN;
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN); // Clear pins
		_delay_ms(10);
	
		LCD_DATA_PORT |= 0b00000011 << LCD_DATA_START_PIN;
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(1);
		
		LCD_DATA_PORT |= 0b00000011 << LCD_DATA_START_PIN;
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(1);
		// End reset
		
		LCD_DATA_PORT |= 0b00000010 << LCD_DATA_START_PIN; // Set 4 bit mode
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(1);
		
		// 4 bit mode. Function Set: 4-bit, 2 Line, 5x7 Dots. Lines are number of memory lines
		// not rows on LCD. There are LCDs with 1 line/row that have 2 memory lines and other
		// LCDs with 1 row with 1 memory line. Please read this article for a better understanding
		// http://web.alfredstate.edu/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
		LCD_DATA_PORT |= (0b00000010 | 0b00000000) << LCD_DATA_START_PIN;
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(1);
		
		LCD_DATA_PORT |= 0b00001000 << LCD_DATA_START_PIN; // Display off
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(1);
		
		LCD_DATA_PORT |= 0b00000001 << LCD_DATA_START_PIN; // Clear Display
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(5);
		
		LCD_DATA_PORT |= 0b00000110 << LCD_DATA_START_PIN; // Entry Mode Set
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(1);
		
		LCD_DATA_PORT |= 0b00001100 << LCD_DATA_START_PIN; // Turn on display
		_delay_ms(1);
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		FlashEnable();
		// End of initialization
	
		LCD_DATA_PORT |= ((0b00000010) << LCD_DATA_START_PIN);
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
		_delay_ms(10); // busy flag is not available yet
		LCDCmd(LCD_DISPLAY_ON | cursorStyle); // Turn on display, set cursor type
		//LCDCmd(0x28); // 4 bit mode. Function Set: 4-bit, 2 Line, 5x7 Dots
	#endif

	#if LCD_CUSTOM_CHARS == TRUE || BIG_DIGITS_3_CHARACTERS == TRUE || BIG_DIGITS_1_CHARACTERS == TRUE
		// Upload custom characters to LCD's volatile memory
		uint8_t i, location, pos;
		
		for(location = 0; location < 8; location++){
			pos = location * 8;
			LCDCmd(0b01000000 + pos); // Set location in CGRAM where to write custom char
			for(i=pos; i < pos+8; i++){
				LCDData(LCD_custom_chars[i]);
			}
		}
	#endif
	
	#if LCD_BACKLIGHT_PWM == TRUE
		LCDBacklightPWM(100);
	#endif
	
	LCDClear();
	LCDHome();
}




/*---------------------------------------------------------------------------------------------------
*	Print a custom character defined by the "LCD_custom_chars" array. Up to 8 custom characters are 
*	supported by the LCD module. LCD_CUSTOM_CHARS must be TRUE and BIG_DIGITS_3_CHARACTERS and
*	BIG_DIGITS_1_CHARACTERS must be both FALSE.
*
*	@param [char_index] 		from 0 to 7. Custom defines are recommended to assign a name to
*								a particular character.
*							
*   @return 					NONE
*----------------------------------------------------------------------------------------------------*/
void LCDPrintCustomChar(uint8_t char_index){
   LCDData(char_index);
}




/*---------------------------------------------------------------------------------------------------
*	Prints special characters inside the HD44780 memory other than the regular alphanumeric 
*	and punctuation signs
*
*	@param [char_address] 		the address of the character
*							
*   @return 					NONE
*----------------------------------------------------------------------------------------------------*/
void LCDPrintExtraChar(uint8_t char_address){
   LCDData(char_address);
}




/*---------------------------------------------------------------------------------------------------
*	PRINT A STRING OF CHARACTERS ON THE DISPLAY
*
*	@param [msg] 				string to be printed on the display
*							
*   @return 					NONE
*----------------------------------------------------------------------------------------------------*/
void LCDWriteString(const char *msg){
	uint8_t pos = 1;
	uint8_t line = 1;
	
	while(*msg > 0){
		#if LCD_WRAP_TEXT == TRUE
			if(pos > LCD_NR_OF_CHARACTERS){
				if(LCD_NR_OF_ROWS > 1){
					if(line == 1){
						LCDGotoXY(1, 2); // go to line 2
						line = 2;
					}else if(line == 2 && LCD_NR_OF_ROWS > 2){
						LCDGotoXY(1, 3); // go to line 3
						line = 3;
					}else if(line == 3 && LCD_NR_OF_ROWS > 3){
						LCDGotoXY(1, 4); // go to line 4
						line = 4;
					}
					
					if(line > 1 && *msg == 0x20) msg++; // remove space if it is at the beginning of the line
				}
				
				pos = 1;
			}
		#endif
		
		// Custom Char Support
		#if LCD_CUSTOM_CHARS == TRUE
			if(*msg == '%'){
				msg++;
				int8_t c = *msg - '0';
				
				if(c >= 0 && c < 8){
					LCDData(c);
				}else{
					LCDData('%');
					LCDData(*msg);
				}
			}else{
				LCDData(*msg);
			}
			
		#else
			LCDData(*msg);
		#endif
			
		msg++;
		pos++;
	}
}



/*-------------------------------------------------------------------------------------------------------------------------------
*	Big 2 lines height separator. Can be used in conjunction with big digits to make a digital clock
*	The function displays ":" but bigger, on two lines
*
*	@param [] 					NONE
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
void LCDWriteBigSeparator(){
	// Calculate new cursor position based on current position
	LCDGotoXY(cursorPosition, 1);
	
	LCDData(0b10100101); // big dot
	LCDGotoXY(cursorPosition-1, 2);
	LCDData(0b10100101); // big dot
}


/*--------------------------------------------------------------------------------------------------------------------------------
*	Custom double height 1 character wide font can be found in "double_height_sharp_digits.h"
*	Copy data from there in this file in the variable "LCD_custom_chars". LCD_CUSTOM_CHARS must be 
*	set to TRUE in the setup section.
*	
*	@param [number]				an integer number to be displayed	
*
*	@param [nrOfDigits] 		Total number of digits desired. If the number to be displayed has less digits than "nrOfDigits"
*								then it will be padded with zeros. E.g:
*								
*								uint16_t ADC_results = 120;
*								LCDWriteInt(ADC_results, 5);
*									will display "00120"
*								LCDWriteInt(ADC_results, 3);
*									will display "120"
*								You can also display negative numbers
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
#if BIG_DIGITS_1_CHARACTERS == TRUE
	void LCDWriteIntBig(int32_t number, int8_t nrOfDigits){
		uint8_t new_pos, line, length=0, i;
		uint8_t buffer[LCD_MAXIMUM_DIGITS] = {0};
		int32_t copyOfNumber = number;
		
		// Find number of digits
		//if(number == 0) length = 1;
		
		while(copyOfNumber != 0){
			length++;
			copyOfNumber /= 10;
		}
		
		if(number == 0){
			buffer[1] = 0;
			length = 1;
		}
		
		copyOfNumber = number;
		nrOfDigits -= length;
		
		if(nrOfDigits < 0) nrOfDigits = 0;
			
		if(number < 0){
			LCDGotoXY(cursorPosition, 1);
			LCDData('_');
			// Convert negative number to positive
			copyOfNumber = 0 - number;
		}
		
		length += nrOfDigits;
		
		for(i=0; i<length; i++){
			buffer[i] = copyOfNumber % 10;
			copyOfNumber /= 10;
		}
		
		
		// Display the numbers
		while(length){
			// Calculate new cursor position based on current position
			LCDGotoXY(cursorPosition, 1);
			new_pos = cursorPosition;
			line = 2;
		
			switch(buffer[length-1]){
				case 0:
					LCDData(1);
					LCDGotoXY(new_pos, line);
					LCDData(5);
				break;
				
				case 1:
					LCDData(0);
					LCDGotoXY(new_pos, line);
					LCDData(0);
				break;
				
				case 2:
					LCDData(7);
					LCDGotoXY(new_pos, line);
					LCDData(2);
				break;
				
				case 3:
					LCDData(6);
					LCDGotoXY(new_pos, line);
					LCDData(3);
				break;
				
				case 4:
					LCDData(5);
					LCDGotoXY(new_pos, line);
					LCDData(0);
				break;
				
				case 5:
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(3);
				break;
				
				case 6:
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(5);
				break;
				
				case 7:
					LCDData(7);
					LCDGotoXY(new_pos, line);
					LCDData(0);
				break;
				
				case 8:
					LCDData(4);
					LCDGotoXY(new_pos, line);
					LCDData(5);
				break;
				
				case 9:
					LCDData(4);
					LCDGotoXY(new_pos, line);
					LCDData(3);
				break;
			}
			
			length--;
		}
	}
#endif



/*-------------------------------------------------------------------------------------------------------------------------------
*	Custom double height 3 characters wide font can be found in "double_height_3_characters_round_digits.h"
*	LCD_CUSTOM_CHARS must be set to TRUE in the setup section.
*	
*	@param [number]				an integer number to be displayed	
*
*	@param [nrOfDigits] 		Total number of digits desired. If the number to be displayed has less digits than "nrOfDigits"
*								then it will be padded with zeros. E.g:
*								
*								uint16_t ADC_results = 120;
*								LCDWriteInt(ADC_results, 5);
*									will display "00120"
*								LCDWriteInt(ADC_results, 3);
*									will display "120"
*								You can also display negative numbers
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
#if BIG_DIGITS_3_CHARACTERS == TRUE
	void LCDWriteIntBig3Chars(int32_t number, int8_t nrOfDigits){
		uint8_t new_pos, line, length=0, i;
		uint8_t buffer[LCD_MAXIMUM_DIGITS] = {0};
		int32_t copyOfNumber = number;
		
		// Clear previous digits
		new_pos = cursorPosition;
		line = cursorLine;
		LCDGotoXY(new_pos, 1);
		
		for(i=0; i<7; i++){
			LCDData(' ');
		}
		
		LCDGotoXY(new_pos, cursorLine+1);
		
		for(i=0; i<7; i++){
			LCDData(' ');
		}
		
		LCDGotoXY(new_pos, line);
		
		// Find number of digits
		while(copyOfNumber != 0){ 
			length++;
			copyOfNumber /= 10;
		}
		
		if(number == 0){
			buffer[1] = 0;
			length = 1;
		}
		
		copyOfNumber = number;
		
		nrOfDigits -= length;
		if(nrOfDigits < 0) nrOfDigits = 0;
			
		if(number < 0){
			LCDGotoXY(cursorPosition, 1);
			LCDData('_');
			copyOfNumber = 0 - number;
		}
		
		length += nrOfDigits;
		
		for(i=0; i<length; i++){
			buffer[i] = copyOfNumber % 10;
			copyOfNumber /= 10;
		}
		
		
		// Display the numbers
		while(length){
			// Calculate new cursor position based on current position
			LCDGotoXY(cursorPosition, 1);
			new_pos = cursorPosition;
			line = 2;
		
			switch(buffer[length-1]){
				case 0:
					LCDData(0);
					LCDData(1);
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(3);
					LCDData(4);
					LCDData(5);
				break;
				
				case 1:
					LCDData(1);
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(4);
					LCDData(7);
					LCDData(4);
				break;
				
				case 2:
					LCDData(6);
					LCDData(6);
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(3);
					LCDData(4);
					LCDData(4);
				break;
				
				case 3:
					LCDData(6);
					LCDData(6);
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(4);
					LCDData(4);
					LCDData(5);
				break;
				
				case 4:
					LCDData(3);
					LCDData(4);
					LCDData(7);
					LCDGotoXY(new_pos+2, line);
					LCDData(7);
				break;
				
				case 5:
					LCDData(3);
					LCDData(6);
					LCDData(6);
					LCDGotoXY(new_pos, line);
					LCDData(4);
					LCDData(4);
					LCDData(5);
				break;
				
				case 6:
					LCDData(0);
					LCDData(6);
					LCDData(6);
					LCDGotoXY(new_pos, line);
					LCDData(3);
					LCDData(4);
					LCDData(5);
				break;
				
				case 7:
					LCDData(1);
					LCDData(1);
					LCDData(2);
					LCDGotoXY(new_pos+2, line);
					LCDData(7);
				break;
				
				case 8:
					LCDData(0);
					LCDData(6);
					LCDData(2);
					LCDGotoXY(new_pos, line);
					LCDData(3);
					LCDData(4);
					LCDData(5);
				break;
				
				case 9:
					LCDData(0);
					LCDData(6);
					LCDData(2);
					LCDGotoXY(new_pos+2, line);
					LCDData(7);
				break;
			}
			
			LCDData(' '); // Insert a space between digits to distinguish them better
			length--;
		}
	}
#endif



/*-------------------------------------------------------------------------------------------------------------------------------
*	PRINT AN INTEGER NUMBER ON THE DISPLAY
*	
*	@param [number]				integer number to be displayed	
*
*	@param [nrOfDigits] 		Total number of digits desired. If the number to be displayed has less digits than "nrOfDigits"
*								then it will be padded with zeros. E.g:
*								
*								uint16_t ADC_results = 120;
*								LCDWriteInt(ADC_results, 5);
*									will display "00120"
*								LCDWriteInt(ADC_results, 3);
*									will display "120"
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
void LCDWriteInt(int32_t number, int8_t nrOfDigits){
	char string[LCD_MAXIMUM_DIGITS] = {0};

	uint8_t isNegative = 0;
	uint8_t length = 0;
	uint8_t divide = 0;
	int32_t copyOfNumber = number;

	// Do not return here in case padding is necessary
	if(number == 0) length = 1;

	// Find number of digits
	while(copyOfNumber != 0){ 
		length++;
		copyOfNumber /= 10;
	}

	// Restore the number
	copyOfNumber = number;

	// Check if padding with 0 is necessary
	nrOfDigits -= length;
	if(nrOfDigits < 0) nrOfDigits = 0;

	// Check if number is negative and convert it to a positive
	if(number < 0){
		isNegative = 1;
		copyOfNumber = 0 - copyOfNumber;
		// Add 1 for the minus sign
		length++;
	}

	divide = length + nrOfDigits;

	// Convert int to string and put in the array
	while(divide){
	
		// Trim the length of the number if it's bigger than the limit
		if(divide < LCD_MAXIMUM_DIGITS){
			string[divide - 1] = (copyOfNumber % 10) + '0';
		}
		
		copyOfNumber /= 10;
		divide--;
	}

	if(isNegative) string[0] = '-';

	LCDWriteString(string);
}



/*-------------------------------------------------------------------------------------------------------------------------------
*	PRINT A FLOAT NUMBER ON THE DISPLAY
*	
*	@param [number]				float number to be displayed	
*
*	@param [nrOfDigits] 		Total number of digits desired. If the number to be displayed has less digits than "nrOfDigits"
*								then it will be padded with zeros. E.g:
*								
*								float temperature = 27.5;
*								LCDWriteFloat(temperature, 0);
*									will display "27.5"
*								LCDWriteFloat(temperature, 3);
*									will display "027.5" to make the part before the point 3 digits (027)
*
*	@param [nrOfDecimals] 		How many decimals to display. If there are many decimals sometimes only a few are needed
*								and the rest can be discarded
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
#if LCD_DISPLAY_FLOATS == TRUE
	void LCDWriteFloat(float float_number, int8_t nrOfDigits, uint8_t nrOfDecimals){
		uint8_t float_length = 0;
		uint8_t is_negative = 0;
		int32_t integer_part = 0;
		float rounding = 0.5;
		
		// Limit the size of decimal portion
		if(nrOfDecimals == 0) nrOfDecimals = 1;
		if(nrOfDecimals > 7) nrOfDecimals = 7;
		float_length = nrOfDecimals;
		
		// Adjust the rounding value
		while(float_length){
			rounding /= 10.0;
			float_length--;
		}
		
		// Restore float length
		float_length = nrOfDecimals;
		
		// Convert float part to positive
		if(float_number < 0){
			float_number = -float_number;
			is_negative = 1;
		}
		
		// Round up
		float_number += rounding;
		
		// Get integer part
		integer_part = float_number;
		
		// Get the decimal portion
		float_number -= integer_part;
		
		// Convert fractional part to integer
		while(float_length){
			float_number *= 10;
			float_length--;
		}
		
		if(is_negative) integer_part = -integer_part;
		if(is_negative && integer_part == 0) LCDWriteString("-");
		
		LCDWriteInt(integer_part, nrOfDigits);
		LCDWriteString(".");
		LCDWriteInt(float_number, nrOfDecimals);
	}
#endif



/*-------------------------------------------------------------------------------------------------------------------------------
*	Move the cursor to a specific XY location
*	
*	@param [x]					character position	
*
*	@param [y] 					line number
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
void LCDGotoXY(uint8_t x, uint8_t y){
	LCDBusyLoop();
	if(x == 0 || x == 255) x = 1; // User can use 0 or 1 as starting character position
	cursorPosition = x;
	cursorLine = y;
	
	switch(y){
		case 255: // If a variable is decremented and is negative it will reset to 255 because the parameter is unsigned
		case 0:
		case 1:
			x -= 1; // User can use values starting from 1, but LCD starts from 0 so we substract 1
		break;
		
		case 2:
			x += 63; // Line 2
		break;
		
		case 3:
			x += LCD_NR_OF_CHARACTERS - 1; // Line 3
		break;
		
		case 4:
			x += 79 + (LCD_NR_OF_CHARACTERS - 16); // Line 4
		break;
	}
	
	x |= 0b10000000;
	LCDCmd(x);
}



/*-------------------------------------------------------------------------------------------------------------------------------
*	Dim LCD backlight using Fast PWM Timer0, channel B (OC0B pin), OCR0A as TOP
*	Frequency is set to 400Hz to prevent flickering
*	
*	Circuit: a small signal transistor can be used with emitter connected to ground.
*	Connect OC0B pin to the base of transistor through a resistor. Connect LCD backlight anode to Vcc and
*	cathode to collector.
*
*	@param [brightness] 		can be between 0 - 100
*								"0" will turn off the backlight and the LCD without clearing DDRAM thus saving power
*								"100" will turn LED backlight fully on and stop PWM
*							
*   @return 					NONE
*--------------------------------------------------------------------------------------------------------------------------------*/
#if LCD_BACKLIGHT_PWM == TRUE
	void LCDBacklightPWM(uint8_t brightness){
		unsigned long prescaler = 256;
		
		if(brightness > 99){ // account for values greater than 100
			// No need for PWM - stop the timer
			TCCR0B = 0;
			TCCR0A = 0;
			LCD_PWM_PORT |= 1 << LCD_PWM_PIN;
			LCDCmd(LCD_DISPLAY_ON); // turn on display
			
		}else if(brightness < 1){ // account for negative values
			// Stop the timer and turn off LCD backlight
			TCCR0B = 0;
			TCCR0A = 0;
			LCD_PWM_PORT &= ~(1 << LCD_PWM_PIN);
			LCDCmd(0x08); // turn off display and cursor without clearing DDRAM
			
		}else{ // values between 1 and 99
			// Set Timer0 in Fast PWM mode and set OC0B pin on compare match, OCR0A as TOP
			LCD_PWM_DDR |= 1 << LCD_PWM_PIN;
			TCCR0A 	|= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
			TCCR0B 	|= (1 << WGM02) | (1 << CS02); // prescaler set to 256
			
			// Calculate OCR0A (TOP value) and set the frequency
			OCR0A = (F_CPU / (prescaler * 400)) - 1; // prescaler * frequency
			
			// Set duty cycle
			OCR0B = (OCR0A * brightness) / 100;
			LCDCmd(LCD_DISPLAY_ON); // turn on display
		}
	}
#endif





void LCDByte(uint8_t data, uint8_t isdata){
	LCDBusyLoop();
	
	if(isdata == 0){
		RS_OFF(); // Send command - RS to 0
		if(data == 0b10000000 || data == 0b00000001){
			cursorPosition = 1;
			cursorLine = 1;
		}
	}else{
		RS_ON(); // Send data - RS to 1
		cursorPosition++;
	}
	
	RW_OFF(); // RW to 0 - write mode
	
	#if LCD_DATA_BUS_SIZE == LCD_DATA_8_BITS
		LCD_DATA_PORT = data;
		FlashEnable();
		LCD_DATA_PORT = 0x00;
		
	#elif LCD_DATA_BUS_SIZE == LCD_DATA_4_BITS
		unsigned char temp; // If signed, after shift MSB will be replaced with 1 instead of 0 and we don't want that
		uint8_t shift = PORT_SIZE - (LCD_DATA_START_PIN + 4);
	
		// Send high nibble
		temp = (data & 0xF0); // Mask the lower nibble
		LCD_DATA_PORT |= temp >> shift; // Put data on data port
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN);
	
		// Send low nibble
		temp = ((data << 4) & 0xF0); // Shift 4-bit and mask
		LCD_DATA_PORT |= temp >> shift;
		FlashEnable();
		LCD_DATA_PORT &= ~(0x0F << LCD_DATA_START_PIN); // Clear data port
	#endif
}



void LCDBusyLoop(){
	#if LCD_DATA_BUS_SIZE == LCD_DATA_8_BITS
		LCD_DATA_DDR = 0x00;
	#elif LCD_DATA_BUS_SIZE == LCD_DATA_4_BITS
		LCD_DATA_DDR &= ~(0x0F << LCD_DATA_START_PIN); // Set DDR to input for reading LCD status
	#endif
	
	RW_ON();		// Read mode
	RS_OFF();		// Read status
	
	// Check LCD status 0b10000000 means busy, 0b00000000 means clear
	#if LCD_DATA_BUS_SIZE == LCD_DATA_8_BITS
		do{
			FlashEnable();
		}while(LCD_DATA_PIN >= 0x80);
	#elif LCD_DATA_BUS_SIZE == LCD_DATA_4_BITS
		uint8_t busy, high_nibble;
		
		do{
			// Read high nibble
			E_ON();
			_delay_us(1); // Implement 'Delay data time' (160 nS) and 'Enable pulse width' (230 nS)
			high_nibble = LCD_DATA_PIN >> LCD_DATA_START_PIN;
			high_nibble = high_nibble << 4;
			E_OFF();
			_delay_us(1); // Implement 'Address hold time' (10 nS), 'Data hold time' (10 nS), and 'Enable cycle time' (500 nS )
			
			// No need for low nibble
			E_ON();
			_delay_us(1);
			E_OFF();
			_delay_us(1);
			
			busy = high_nibble & 0b10000000;
		}while(busy);
	#endif
		
	RW_OFF();
	RS_ON();
		
	#if LCD_DATA_BUS_SIZE == LCD_DATA_8_BITS
		LCD_DATA_DDR = 0xFF; // Set DDR to output again
	#elif LCD_DATA_BUS_SIZE == LCD_DATA_4_BITS
		LCD_DATA_DDR |= (0x0F << LCD_DATA_START_PIN);
	#endif
}



void FlashEnable(){
	E_ON(); // Enable on
	_delay_us(50); // Wait
	E_OFF(); // Execute
}



/* ----------------------------------- ANIMATIONS */
#if LCD_ANIMATIONS == TRUE
	void LCDScrollText(const char *text){
		uint8_t shift_number=1, i=0, chars_to_display=1;
		size_t text_size = strlen(text);
		const char *text_start_pos = text;

		LCDClear();
		LCDGotoXY(LCD_NR_OF_CHARACTERS, 1);
		
		while(shift_number < text_size + LCD_NR_OF_CHARACTERS){
			
			while(i < chars_to_display){
				LCDData(*text);
				i++;
				text++;
			}
			
			_delay_ms(LCD_SCROLL_SPEED); // 200 - 300 is a good choise
			LCDClear();
			
			if(shift_number < LCD_NR_OF_CHARACTERS && shift_number < text_size){
				LCDGotoXY(LCD_NR_OF_CHARACTERS - shift_number, 1);
				text = text_start_pos;
				chars_to_display++;
			}else{
				if(text_size > LCD_NR_OF_CHARACTERS){
					if(shift_number + 1 > text_size)
						chars_to_display--;
					else
						chars_to_display = LCD_NR_OF_CHARACTERS;
					
					text = text_start_pos + (shift_number - (LCD_NR_OF_CHARACTERS - 1));
					LCDGotoXY(1, 1);
				}else{
					if(shift_number + 1 > LCD_NR_OF_CHARACTERS){
						chars_to_display--;
						text = text_start_pos + (shift_number - LCD_NR_OF_CHARACTERS) + 1;
						LCDGotoXY(1, 1);
					}else{
						text = text_start_pos;
						chars_to_display = text_size;
						LCDGotoXY(LCD_NR_OF_CHARACTERS - shift_number, 1);
					}
				}
			}
			
			i = 0;
			shift_number++;
		}
		
		LCDHome();
	}
#endif



/* ----------------------------------- UTILS */ 
#if LCD_UTILS == TRUE
	void LCDFindCharPositions(void){
		uint8_t x;
		
		for(x=0; x<255; x++){
			LCDCmd(0x80 | x);
			LCDWriteString("x");
			
			LCDHome();
			LCDWriteString("X:");
			LCDWriteInt(x, 3);
			
			_delay_ms(LCD_X_POS_DELAY);
			LCDClear();
		}
	}
#endif

#endif // OnLCDLib_H
