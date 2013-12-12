/*
 * communication.c
 * Functionality for receiving and sending both TWI and Bluetooth messages.
 * Created: 11/26/2013 5:43:02 PM
 * Version 1.1
 *  Author: Kristoffer Borg, Emil Berg
 */

#include <asf.h>
#include <avr/interrupt.h>
#include "twi_master.h"
#include "USART.h"
#include "communication.h"

void send_error(uint8_t error_code);
void enable_irqs(void);
void enable_timer(void);

// Global variables
volatile bool control_module_interrupt = false;
volatile bool sensor_module_interrupt = false;
volatile uint8_t firefly_header = 0xFF;
volatile uint8_t firefly_data = 0xFF;
volatile bool firefly_received_data = false;

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
	control_module_interrupt = true;
}

// INT1 interrupts from sensor module
ISR(INT1_vect) {
	sensor_module_interrupt = true;
}

inline void send_error(uint8_t error_code) {
	USART_transmit(0x0C, error_code);
}

// Interrupt init
// Remember to set firefly interrupt flag
void enable_irqs() {
	
	EICRA |= (1 << ISC00) | (1 << ISC01); // Interrupt irq0 on rising edge
	EICRA |= (1 << ISC10) | (1 << ISC11); // Interrupt irq1 on rising edge
	EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	EIFR |= (1 << INTF0) | (1 << INTF1); //firefly
	
	sei();
}

void mainfunction() {
	
	uint8_t header, data;
	
	bool received_data = false;
	
	bool has_sent_error = false;
	
	enable_irqs();
	
	while (true) {
		
		while (received_data) {
			int twi_send_err = 0;
			
			// Send to right instance depending on header
			if (header == 0x00 || (header >= 0x0E && header <= 0x12) ) {
				// FireFly is sending control commands. Relay to control module.
				cli();
				twi_send_err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, header, data);
				sei();
			} else if (header == 0x01 || header == 0x0C) {
				// Control module is sending, or error
				USART_transmit(header, data);
			} else if (header == 0x02 || header == 0x13) {
				// Calibration command or item picked up signal
				cli();
				twi_send_err = TWI_master_send_message(TWI_SENSOR_MODULE_ADDRESS, header, data);
				sei();
			} else if (header >= 0x03 && header <= 0x0B) {
				// Sensor module is sending. Relay to control module and FireFly.
				cli();
				twi_send_err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, header, data);
				sei();
				if (! twi_send_err) {
					USART_transmit(header, data);
				}
			} else if (header == 0x0D) {
				// TODO: Ping
			} else {
				// Invalid header. Send error message
				send_error(0x00);
			}
			
			// If there was any error with TWI
			if (twi_send_err) {
				// TWI error. Send error message, but only once.
				if (! has_sent_error) {
					send_error(0x01);
					has_sent_error = true;
				}
			} else {
				received_data = false;
			}
		}
		
		has_sent_error = false;
		
		int twi_rec_err = 0;
		do {
			if (sensor_module_interrupt) {
				sensor_module_interrupt = false;
				// Get data from sensor module
				cli();
				twi_rec_err = TWI_master_receive_message(TWI_SENSOR_MODULE_ADDRESS, &header, &data);
				sei();
			
				if (twi_rec_err) {
					sensor_module_interrupt = true;
				} else {
					received_data = true;
					twi_rec_err=0;// la till denne
				}
			} else if (control_module_interrupt) {
				control_module_interrupt = false;
				// Get data from control module
				cli();
				twi_rec_err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
				sei();
				
				if (twi_rec_err) {
					control_module_interrupt = true;
				} else {
					received_data = true;
					twi_rec_err=0;// la till denne
				}
			} else if (firefly_received_data) {
			
				cli();
				header = firefly_header;
				data = firefly_data;
				firefly_received_data = false;
				sei();
			
				received_data = true;
			}
		
			// TWI error. Send error message, but only once.
			if (twi_rec_err) {
				if (! has_sent_error) {
					send_error(0x02);
					has_sent_error = true;
				}
			}
		
		} while (twi_rec_err);
		
		//has_sent_error = false; omdeklarerad och reset:ad ovanför loopen
	}
}