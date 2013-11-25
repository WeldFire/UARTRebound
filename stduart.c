/*	stduart.c
 *	A standard UART library for the MSP430 Launchpad M430G2553 Microcontroller.
 *
 *  Created on: Sep 5, 2013
 *      Author: WeldFire
 */
#include  "msp430g2553.h"
#include "stduart.h"


#define RXD		BIT1						// Receive Data (RXD) at P1.1
#define TXD		BIT2						// Transmit Data (TXD) at P1.2

unsigned char _rxByte;

//Callback handler for receive
void (*uartRxPtr)(unsigned char c);
char* uart_nts(long input, char* result, int pad);

void uart_init(void){
	setUARTOnReceiveMethod(0L);				// Reset the pointer

	P1SEL  |= RXD + TXD;					// Setup the pins
  	P1SEL2 |= RXD + TXD;

  	UCA0CTL1 |= UCSSEL_2;                   // SMCLK

  	// 1,000,000Hz, 9600Baud, UCBRx=104, UCBRSx=1, UCBRFx=0
  	UCA0BR0 = 104;                          // 1MHz 9600
  	UCA0BR1 = 0;                            // 1MHz 9600
  	UCA0MCTL = UCBRS0;                      // Modulation UCBRSx = 1

  	/*// 8,000,000Hz, 9600Baud, UCBRx=52, UCBRSx=0, UCBRFx=1
	UCA0BR0 = 52;                       // 8MHz, OSC16, 9600
	UCA0BR1 = 0;                        // 8MHz, OSC16, 9600
	UCA0MCTL = 0x10|UCOS16;             // UCBRFx=1,UCBRSx=0, UCOS16=1
  	*/

  	UCA0CTL1 &= ~UCSWRST;                   // Initialize USCI state machine
  	IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt

  	__bis_SR_register(GIE);					// Enable Interrupts
}

void setUARTOnReceiveMethod(void (*rxPtr)(unsigned char c)) {
	//Save the methods pointer to be called later
	uartRxPtr = rxPtr;
}

unsigned char uart_getc(){
	//Clear the received character buffer
    _rxByte = 0;

    //Wait to receive the next character
    while(_rxByte==0);

	return UCA0RXBUF;
}

void uart_putc(unsigned char c){
	// Wait for the TX buffer to be ready
	while (!(IFG2&UCA0TXIFG));
	// Transmit the character
  	UCA0TXBUF = c;
}

void uart_puts(const char *str){
     while(*str) uart_putc(*str++);
}

void uart_putd(long number){
	char resultNum[11];
	uart_puts(uart_nts(number, resultNum,0));
}

void uart_putdPadded(long number, int pad){
	char resultNum[16];
	uart_puts(uart_nts(number, resultNum, pad));
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void UART_RecieveInterrupt(void){
	//Make sure the function pointer is set
	if(uartRxPtr != 0L) {
			//Send the data to the function registered
			(uartRxPtr)(UCA0RXBUF);
	}
	//Set the last received character
	_rxByte = UCA0RXBUF;
}

//(whole)Number to string
char* uart_nts(long input, char* result, int pad){
	const char *numString = "0123456789";
	char* resultPtr = result;
	int padIndex = 0;
	long tmp_value;

	//Create the string but reversed
	while (input){
		tmp_value = input;
		input /= 10;
		*resultPtr++ = numString[(tmp_value - input * 10)];
		padIndex++;
	}

	//Apply padding to the end of the string
	while(padIndex < pad){
		*resultPtr++ = numString[0];
		padIndex++;
	}

	// Apply negative sign
	if (tmp_value < 0){
		*resultPtr++ = '-';
	}
	//Close the string
	*resultPtr-- = '\0';

	char *helperPtr = result;
	char tmp_char;

	//Reverse the string
	while(helperPtr < resultPtr){
		tmp_char = *resultPtr;
		*resultPtr--= *helperPtr;
		*helperPtr++ = tmp_char;
	}
	return result;
}
