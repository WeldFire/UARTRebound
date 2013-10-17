#ifndef STDUART_H
#define STDUART_H

//Initialize software UART
void uart_init(void);

//Set pointer for ISR to call when data was received.
void setUARTOnReceiveMethod(void (*rxPtr)(unsigned char c));

/**
 * Read only one character from the UART buffer.
 * @return	character received
 */
unsigned char uart_getc();

//Write one character to the UART buffer.
void uart_putc(unsigned char c);

//Write a string to the UART buffer.
void uart_puts(const char *str);
#endif
