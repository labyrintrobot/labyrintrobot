#define F_CPU 14.7456E6

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi_master.h"
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

void a(void);
void a() {}

int main (void)
{
	board_init();
	
	// Errors on PORT A
	// Messages on PORT B
	
	DDRB = 0xFF; // Err
	PORTB = 0;
	DDRA = 0xFF; // Data
	PORTA = 0;
	
	//enable_irqs();
	
	USART_init(14400);
	TWI_common_initialize(TWI_COMMUNICATION_MODULE_ADDRESS, false, 5, true);
	
	const int test_mode = 0;
	
	if (test_mode == 0) {
		USART_transmit(0b1111101, 0b1111110);
	}
	if (test_mode == 1) {
		uint8_t header;
		uint8_t data;
		int err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
		if (err) {
			PORTA = TWSR;
			PORTB = err;
			while(1);
		}
		err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
		if (err) {
			PORTA = TWSR;
			PORTB = err;
			while(1);
		}
		PORTB = header;
		while(1);
	}
	if (test_mode == 2) {
		_delay_ms(200);
		
		int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, 0b11001100, 0b10101010);
		if (err) {
			PORTA = TWSR;
			PORTB = err;
			while(1);
		}
		err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, 0b11001100, 0b10101010);
		if (err) {
			PORTA = TWSR;
			PORTB = err;
			while(1);
		}
		PORTB = 0b01010101;
		while(1);
	}
}
