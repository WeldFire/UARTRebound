/*	stduart.c
 *	A standard UART library for the MSP430 Launchpad M430G2553 Microcontroller.
 *
 *  Created on: Sep 5, 2013
 *      Author: WeldFire
 */
#include  "msp430g2553.h"
#include "stduart.h"


#define RXD		BIT1						//Receive Data (RXD) at P1.1
#define TXD		BIT2						//Transmit Data (TXD) at P1.2

unsigned char _rxByte;

//Callback handler for receive
void (*uartRxPtr)(unsigned char c);

void uart_init(void){
	setUARTOnReceiveMethod(0L);				//Reset the pointer

	P1SEL  |= RXD + TXD;
  	P1SEL2 |= RXD + TXD;
  	UCA0CTL1 |= UCSSEL_2;                   // SMCLK
  	UCA0BR0 = 104;                          // 1MHz 9600
  	UCA0BR1 = 0;                            // 1MHz 9600
  	UCA0MCTL = UCBRS0;                      // Modulation UCBRSx = 1
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
