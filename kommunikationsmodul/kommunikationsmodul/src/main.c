#define F_CPU 14.7456E6

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi_master.h"
#include "twi_master_test.h"
#include "USART.h"

int main (void)
{
	board_init();
	
	// Errors on PORT A
	// Messages on PORT B
	
	DDRB = 0xFF; // Err
	PORTB = 0;
	DDRA = 0xFF; // Data
	PORTA = 0;
	
	int err = USART_init(115200);
	if (err) {
		PORTB = err;
	} else {
		while (1) {
			_delay_ms(200);
			USART_transmit('a', 'b');
		}
	}

	while(1);
	
	//enable_irqs();
	//USART_init(14400);
	/*int err = TWI_master_initialize(TWI_COMMUNICATION_MODULE_ADDRESS);
	if (err) {
		PORTB = err;
		while (1);
	}
	
	//_delay_ms(200);
	err = TWI_master_test_both();
	if (err) {
		PORTB = 0;
	} else {
		PORTB = 0b00110011;
	}*/
}
