/********************************************
*
*  Name:
*  Email:
*  Section:
*  Assignment: Lab 4 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

enum states { UP, DOWN, PAUSE };

int main(void) {

    // Setup DDR and PORT bits for the 3 input buttons as necessary

    // Initialize the LCD


    // Use a state machine approach to organize your code
    //   - Declare and initialize a variable to track what state you
    //     are in by assigning the values UP, DOWN or PAUSE to that variable.


    while (1) {               // Loop forever

        // Use "if" statements to read the buttons and determine which
        // state you are in


	// Based on the current state, determine the new count value


	// Perform the output operations if necessary


        // Delay before we go to the next iteration of the loop

    }

    return 0;   /* never reached */
}

