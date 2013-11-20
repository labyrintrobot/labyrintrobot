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
	board_init();
	TWI_common_initialize(TWI_CONTROL_MODULE_ADDRESS, false, 5, false);
	pwm_start_L();
	pwm_start_R();
	pwm_start_G();
	uint8_t button, switch_;
	signed e;
	//PORTB = 0xC0; ? vad gör denna?
	
	//ADCsetup(); // A/D, test
	//sei(); // Allow interrupts A/D, test
	uint8_t header;
	uint8_t data; 
	TWI_slave_receive_message(&header, &data);
	 
	
	while(1)
	{
		button = PINA & 0x02; // read PortA, pin 1
		switch_ = PINA & 0x01; // read PortA, pin 0
		
		while(switch_ != 0) // man
		{	
			TWI_slave_receive_message(&header, &data);
			if(header == 0x00)
			{
				manual_action(data);
			}
		switch_ = (PINA & 0x01);
		}
		
		while(switch_ == 0) //auto
		{
			if(button != 0) //startar auto
			{
				find_start();
				find_goal();
				return_to_start();
					
				/*
				find start
				find goal
				grab target
				return to start
				stop
				*/
			}
			switch_ = PINA & 0x01;
		}
		
	}
	// Insert application code here, after the board has been initialized.
}

ISR(ADC_vect) //A/D test
{
	sensor_data = ADCH; // Store the 8 bit data
	// Do something with the dataS
}
