/*
 * USART.c
 *
 * Created: 11/21/2013 1:36:47 PM
 *  Author: kribo852
 */ 

#include "USART.h"

void USART_initialize(unsigned int baud);

void USART_initialize(unsigned int baud){
	/*set baud rate*/
	UBRR0H=(unsigned char)(baud>>8);
	UBRR0L=(unsigned char)(baud);
	
	/*enable receiver transmitter*/
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
	
	/*set frame format to 8 data bits (3<<UCSZ00) and 1 stopbit (0<<USBS0) and no parity*/
	UCSR0C=(3<<UCSZ00) | 1<<UMSEL00;
}

int USART_init(uint32_t baud){
	
	if(baud==115200){
		USART_initialize(7);
	} else if(baud==57600){
		USART_initialize(15);
	} else if(baud==14400){
		USART_initialize(63);
	} else {
		return 1;
	}
	
	return 0;
}

void USART_transmit(unsigned char header , unsigned char data){
	/*wait for empty transmit-buffer*/
	while((!((UCSR0A)&(1<<UDRE0)))  );
	/*put header into transmit-buffer*/
	UDR0=header;
	/*wait for empty transmit-buffer*/
	while(!((UCSR0A)&(1<<UDRE0))  );
	/*put header into transmit-buffer*/
	UDR0=data;
}

int USART_receive(unsigned char *header , unsigned char *data){
	
	/*wait for header*/
	while(!((UCSR0A)&(1<RXC0)));
	
	if(UCSR0A & (1<<UPE0)){
		/*return parity error*/
		return 1;
	}
	
	*header=UDR0;
	
	/*wait for data*/
	while(!((UCSR0A)&(1<RXC0)));
	
	if(UCSR0A & (1<<UPE0)){
		/*return parity error*/
		return 1;
	}
	
	*data=UDR0;
	
	return 0;
}
