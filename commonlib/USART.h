#ifndef USART_H_
#define USART_H_

#include <asf.h>
#include <avr/io.h>

void USART_init(unsigned int baud);

void USART_transmit(unsigned char header , unsigned char data);

int USART_receive(unsigned char *header , unsigned char *data);

#endif /* USART_H_ */