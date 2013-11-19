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

#include <functions.h>
//#include <ADC_setup.h>
//#include <auto_functions.h>
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
	//PORTB = 0xC0; ? vad g�r denna?
	
	//ADCsetup(); // A/D, test
	//sei(); // Allow interupts A/D, test
	uint8_t header;
	uint8_t data; 
	PORTB = TWI_slave_receive_message(&header,&data);
	PORTA = data;
	 
	while(1)
	{
		
	}
	
	
	while(1)
	{
		button = PINA & 0x02; // read PortA, pin 1
		switch_ = PINA & 0x01; // read PortA, pin 0
		if(button != 0)
		{
			
			while(switch_ != 0) // man
			{
				forward();
				switch_ = PINA & 0x01;
			}
		
			while(switch_ == 0) //auto
			{
				_delay_ms(100);
				ADCSRA |= 1<<ADSC; // Start ADC conversion
				//e = 0;
				e = ADCH-56;
				forward_regulated(e);	
				//backward();
				switch_ = PINA & 0x01;
			}
		}
/*
		if(button != 0)
			{
				if(switch_ != 0) // testk�rning
				{
					grip_on();
					_delay_ms(1000);
					forward();
					_delay_ms(3000);
					rotate_right90();
					forward();
					_delay_ms(1500);
					stop();
					grip_off();
					_delay_ms(500);
					backward();
					_delay_ms(200);
					rotate_right90();
					rotate_right90();
					forward();
					_delay_ms(1300);					
					rotate_left90();
					forward();
					_delay_ms(3000);
			
				}
				else
				{
					rotate_right90();
				}
		

				
			}
			else
			{
				stop();
			}*/
	
	}
	// Insert application code here, after the board has been initialized.
}

ISR(ADC_vect) //A/D test
{
	sensor_data = ADCH; // Store the 8 bit data
	// Do something with the dataS
}
