/*
* communication.c
*
* Created: 11/26/2013 5:43:02 PM
*  Author: kribo852
*/

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi_master.h"
#include "USART.h"
#include "communication.h"

void enable_irqs(void);
void enable_timer(void);
int TWI_master_init_slaves(void);

// Global variables
volatile bool control_module_interrupt = false;
volatile bool sensor_module_interrupt = false;
volatile uint8_t firefly_header = 0xFF;
volatile uint8_t firefly_data = 0xFF;
volatile bool firefly_received_data = false;
#ifdef USE_PING
volatile bool ping_received = false;
#endif

// Interrupt init
// Remember to set firefly interrupt flag
void enable_irqs() {
	
	EICRA |= (1 << ISC00) | (1 << ISC01); // Interrupt irq0 on rising edge
	EICRA |= (1 << ISC10) | (1 << ISC11); // Interrupt irq1 on rising edge
	EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	EIFR |= (1 << INTF0) | (1 << INTF1);
#ifdef USE_PING
	TIMSK = (1<<TOIE1); // Enable timer 1
#endif
	
	sei();
}

void enable_timer() {
#ifdef USE_PING
	TCNT1 = 0x00;
	TCCR1B = (1<<CS02); // 256 prescaler. 14000000 / 65536 / 256 = 0.8 times per second.
#endif
}

//Firefly interrupt routine
ISR(USART0_RX_vect) {
	uint8_t header;
	uint8_t data;
	USART_receive(&header, &data);
	firefly_header = header;
	firefly_data = data;
	
	firefly_received_data = true;
}

// INT0 interrupts from control module
ISR(INT0_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	control_module_interrupt = true;
	
	SREG = cSREG; // restore
}

// INT1 interrupts from sensor module
ISR(INT1_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	sensor_module_interrupt = true;
	
	SREG = cSREG; // restore
}

// Timer 1 overflow
#ifdef USE_PING
ISR(TIMER0_OVF_vect) {
	
	if (! ping_received) {
		// Send TWI
	}
	// Send new ping
	USART_transmit(0x0D, 0x00);
	ping_received = false;
}
#endif

void mainfunction() {
	
	uint8_t header, data;
	
	bool received_data = false;
	
	enable_irqs();
	enable_timer();
	
	while (true) {
		
		if (received_data) {
			
			int twi_send_err = 0;
			
			//send to right instance depending on header
			if (header == 0x00) {
				// Firefly is sending control commands, relay to control module
				cli();
				twi_send_err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, header, data);
				sei();
			} else if (header == 0x01) {
				// Control module is sending, relay to firefly
				USART_transmit(header, data);
			} else if (header == 0x02) {
				// Firefly is sending a calibration command, relay to sensor module
				cli();
				twi_send_err = TWI_master_send_message(TWI_SENSOR_MODULE_ADDRESS, header, data);
				sei();
			} else if (header >= 0x03 && header <= 0x0B) {
				// Sensor module is sending, relay to styr module and firefly
				cli();
				twi_send_err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, header, data);
				sei();
				USART_transmit(header, data);
			} else if (header == 0x0C) {
				// Error. Relay to firefly
				USART_transmit(header, data);
			} else if (header == 0x0D) {
#ifdef USE_PING
				ping_received = true;
#endif
			} else {
				// Invalid header. Send error message
				USART_transmit(0x0C, 0x00);
			}
			
			// If there was any error with TWI
			if (twi_send_err) {
				// TWI error. Send error message.
				USART_transmit(0x0C, 0x01);
			}
			
			received_data = false;
		}
		
		int twi_rec_err = 0;
		if (sensor_module_interrupt) {
			// Get data from sensor module
			cli();
			twi_rec_err = TWI_master_receive_message(TWI_SENSOR_MODULE_ADDRESS, &header, &data);
			sei();
			
			received_data = true;
			sensor_module_interrupt = false;
		} else if (control_module_interrupt) {
			// Get data from control module
			cli();
			twi_rec_err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
			sei();
			
			received_data = true;
			control_module_interrupt = false;
		} else if (firefly_received_data) {
			
			cli();
			header = firefly_header;
			data = firefly_data;
			firefly_received_data = false;
			sei();
			
			received_data = true;
		}
		if (twi_rec_err) {
			// TWI error. Send error message.
			USART_transmit(0x0C, 0x02);
		}
	}
}

// function to tell the slaves that it is ready to handle their interrupts
int TWI_master_init_slaves() {
	
	uint8_t init_header;
	
	_delay_ms(100);
	
	cli();
	int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, init_header, 0);
	if (err) {
		sei();
		return err;
	}
	err = TWI_master_send_message(TWI_SENSOR_MODULE_ADDRESS, init_header, 0);
	sei();
	
	return err;
}