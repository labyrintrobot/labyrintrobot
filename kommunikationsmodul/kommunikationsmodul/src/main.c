/*
 * main.c
 * Starting point of the application.
 * Version 1.0
 *  Author: Kristoffer Borg, Emil Berg
 */

#include <asf.h>
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
}