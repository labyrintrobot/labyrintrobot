/*
 * main.c
 * Starting point of the application.
 *  Author: Kristoffer Borg, Emil Berg
 */

#define F_CPU 14.7456E6

#include <asf.h>
#include <util/delay.h>
#include "twi_master.h"
#include "USART.h"
#include "communication.h"

int main (void)
{
	board_init();
	
	DDRB = 0xFF; // Err
	DDRA = 0xFF; // Data
	PORTA = 0;
	PORTB = 0;
	
	USART_init(14400);
	TWI_master_initialize(TWI_COMMUNICATION_MODULE_ADDRESS);
	
	mainfunction();
	
	/*while(1){
		TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS , 0 , 0);
		_delay_ms(250);
		TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS , 0 , 6);
		_delay_ms(250);
		USART_transmit(0x01, 0x02);
	}*/
}
