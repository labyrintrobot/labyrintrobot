#include <asf.h>
#include <avr/interrupt.h>
#include "twi_master.h"

void enable_irqs(void);

void execute_message(uint8_t header, uint8_t data);

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

void execute_message(uint8_t header, uint8_t data) {
	// TODO
	PORTB = data;
}

int main (void)
{
	board_init();
	
	int err = TWI_common_initialize(TWI_COMMUNICATION_MODULE_ADDRESS, true, 5);
	if (err) {
		PORTB = 0b11001100;
		while (1);
	}
	
	// Errors on PORT A
	// Messages on PORT B
	
	DDRB = 0xFF; // Only out
	PORTB = 0;
	DDRA = 0xFF; // Only out
	PORTA = 0;
	
	enable_irqs();
	
	//main loop 
	while(1) {
		
		if (interrupt_error) {
			PORTA = 0b10101010;
			while (1);
		}
		
		if (sensor_module_interrupt) {
			uint8_t header;
			uint8_t data;
			err = TWI_master_receive_message(TWI_SENSOR_MODULE_ADDRESS, &header, &data);
			if (err) {
				PORTA = err;
				while (1);
			}
			
			execute_message(header, data);
			
			sensor_module_interrupt = false;
		}
		if (control_module_interrupt) {
			uint8_t header;
			uint8_t data;
			err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
			if (err) {
				PORTA = err;
				while (1);
			}
			
			execute_message(header, data);
			
			control_module_interrupt = false;
		}
	}
}
