#include <msp430.h> 
#include "stduart.h"

/*	main.c
 *  A small program to test the custom standard UART library
 *	for the MSP430 Launchpad M430G2553 Microcontroller.
 *
 *	The program echos the received UART data back and blinks
 *	the red LED on each character received.
 *
 *  Created on: Sep 5, 2013
 *      Author: WeldFire
 */

int main(void);
void uartRecieve(unsigned char c);

//ON RECIEVE FUNCTION
void uartRecieve(unsigned char c){
	uart_putc(c);
	P1OUT ^= BIT0;		// toggle P1.0 (red led)
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop the watchdog timer
	
    //Set the red LED as an output
    P1DIR |= BIT0;

    //Initialize the UART connection
	uart_init();

	//Setup UART on receive method
	setUARTOnReceiveMethod(uartRecieve);

	return 0;
}
