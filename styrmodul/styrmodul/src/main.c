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
#define F_CPU 14.745E6
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t control_command, left_short_s, right_short_s, left_long_s, right_long_s, forward_left_s, forward_right_s, forward_center_s, tape; //Sensordata 
signed e; //reglerfelet
#include <communication.h>
#include <man_functions.h>
#include <auto_functions.h>

#include "twi_slave.h"
uint8_t header = 0;
uint8_t data = 0;

ISR(INT2_vect) //Avbrott från sensormodulen: sluta rotera
{
	keep_turning = 0;
	
}


int main (void)
{
	board_init();

	//TWI_common_initialize(TWI_CONTROL_MODULE_ADDRESS, false, 5, false);
	pwm_start_L();
	pwm_start_R();
	pwm_start_G();
	uint8_t button, switch_;
	uint8_t header;
	uint8_t data; 
	 
	sei();
	
	//Testa sändning
	
	
	
	while(1) // test av _delay_ms()
	{
		_delay_ms(50);
		PORTB = 0x00;
		_delay_ms(50);
		PORTB = 0xFF;
	}
	_delay_ms(2000);
	while(1)
	{
		PORTB = 0x00;
		_delay_ms(1000);
		rotate_left90();
		PORTB = 0x00;
		_delay_ms(1000);
		rotate_right90();
		
		//button = PINA & 0x02; // read PortA, pin 1
		//switch_ = PINA & 0x01; // read PortA, pin 0
		/*
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
					
				
				find start
				find goal
				grab target
				return to start
				stop
				
			}
			switch_ = PINA & 0x01;
		}
		*/
	}
	// Insert application code here, after the board has been initialized.
}

