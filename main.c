#include <msp430.h> 
#include "stduart.h"

/*	main.c
 *  A small program to test the stduart custom UART library
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
	uart_putc(c);		// Send the char that we got back to the sender
	P1OUT ^= BIT0;		// Toggle P1.0 (Red LED)
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop the watchdog timer

    // If calibration constants erased do not load, trap CPU!!
	if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF){
		while(1);
	}
	BCSCTL1 = CALBC1_1MHZ;             // Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

    //Set the red LED as an output
    P1DIR |= BIT0;

    //Initialize the UART connection
	uart_init();

	//Setup UART on receive method
	setUARTOnReceiveMethod(uartRecieve);

	return 0;
}
