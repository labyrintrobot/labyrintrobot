#ifndef USART_H_
#define USART_H_

#include <asf.h>
#include <avr/io.h>

int USART_init(uint32_t baud);

void USART_transmit(uint8_t header, uint8_t data);

int USART_receive(uint8_t *header, uint8_t *data);

#endif /* USART_H_ */