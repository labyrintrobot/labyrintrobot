#define F_CPU 14.7456E6

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi_master.h"
#include "twi_master_test.h"
#include "USART.h"

void enable_irqs(void);

volatile int interrupt_error = 0;
volatile bool sensor_module_interrupt = false;
volatile bool control_module_interrupt = false;

// INT0 interrupts from control module
ISR(INT0_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	if (control_module_interrupt) {
		interrupt_error = 0x01;
	}
	
	control_module_interrupt = true;
	
	SREG = cSREG; // restore
}

// INT1 interrupts from sensor module
ISR(INT1_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	if (sensor_module_interrupt) {
		interrupt_error = 0x02;
	}
	
	sensor_module_interrupt = true;
	
	SREG = cSREG; // restore
}

void enable_irqs() {
	EICRA |= (1 << ISC00) | (1 << ISC01); // Interrupts on rising edge
	EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	EIFR |= (1 << INTF0) | (1 << INTF1);
	sei(); //enable interrupts
}

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
