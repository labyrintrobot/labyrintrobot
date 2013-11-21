/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
//#include <avr/signal.h>
#define F_CPU 1.5280E6
#include <util/delay.h>
#include <avr/interrupt.h>

#include <man_functions.h>
//#include <ADC_setup.h>
#include <auto_functions.h>
#include "twi_slave.h"


uint8_t sensor_data;


int main (void)
{
	PORTB = 0xFF;
	PORTA = 0xFF;
	PORTA = 0;
	PORTB = 0;
	
	board_init();
	TWI_common_initialize(TWI_CONTROL_MODULE_ADDRESS, false, 5, false);
	
	const bool master_receive = false;
	if (master_receive) {
		
	} else {
		uint8_t header;
		uint8_t data;
		int err = TWI_slave_receive_message(&header, &data);
		PORTB = err;
		err = TWI_slave_receive_message(&header, &data);
		PORTB = err;
		while(1);
	}
	
	// Insert application code here, after the board has been initialized.
}

ISR(ADC_vect) //A/D test
{
	sensor_data = ADCH; // Store the 8 bit data
	// Do something with the dataS
}
