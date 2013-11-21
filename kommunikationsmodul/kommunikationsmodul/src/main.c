#define F_CPU 14.7456E6

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi_master.h"

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
	
	DDRB = 0xFF;
	PORTB = 0;
	DDRA = 0xFF;
	PORTA = 0;
	
	const bool receive = false;
	
	TWI_master_initialize(TWI_BITRATE_5_KHZ);
	
	if (receive) {
		enable_irqs();
		while (1) {
			while (!sensor_module_interrupt);
			uint8_t header;
			uint8_t data;
			int err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
			if (err) {
				PORTB = err;
			} else {
				PORTA = header;
			}
			while(1);
		}
	} else {
		_delay_ms(200);
		
		int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, 0b11001100, 0b10101010);
		if (err) {
			PORTB = err;
			while(1);
		}
		err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, 0b11111110, 0b10101010);
		if (err) {
			PORTB = err;
			while(1);
		}
		if (!err) {
			PORTA = 0b11100011;
		}
		while(1);
	}
}
