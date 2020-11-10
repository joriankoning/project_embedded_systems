/*
 * uart.h
 *
 * data verzenden en ontvangen via usb. gebruik van tevoren uart_init
 *
 * Created: 5-11-2020 21:28:07
 *  Author: Jorian
 */ 


#include <stdlib.h>
#include <avr/sfr_defs.h>

#define UBBRVAL 51

void uart_init(void) {
	//baudrate instellen, door 51 te gebruiken wordt de baudrate 19200
	UBRR0H = (UBBRVAL >> 8);
	UBRR0L = UBBRVAL;
	
	//USART Control and Status Registers instellen
	UCSR0A &= ~_BV(U2X0);	// optimalistie is uit
	UCSR0B |= _BV(TXEN0) | _BV(RXEN0);	// er kan data verstuurd worden en ontvangen worden
}

void transmit(uint8_t data) {
	loop_until_bit_is_set(UCSR0A, UDRE0);	// loop zodat de buffer klaar is om te versturen
	UDR0 = data;	// data verzenden
}

uint8_t receive(void) {
	loop_until_bit_is_set(UCSR0A, TXC0);	// loop totdat er iets binnen komt
	return UDR0;							// binnengekomen waarde is de return
}