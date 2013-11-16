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
#include <avr/signal.h>
#define F_CPU 1.5280E6
#include <util/delay.h>
#include <avr/interrupt.h>
#include <functions.h>


void pwm_start_L();
void pwm_start_R();
void pwm_start_G();

void forward();
void backward();
void rotate_right();
void rotate_left();
void rotate_right90();
void rotate_left90();
void forward_right();
void forward_left();
void stop();
void grip_on();
void grip_off();
void forward_regulated(signed e);

int main (void)
{
	board_init();
	
	pwm_start_L();
	pwm_start_R();
	pwm_start_G();
	uint8_t button, switch_;
	signed int e = 0;
	PORTB = 0xC0;
	
	while(1)
	{
		button = PINA & 0x02; // read PortA, pin 1
		switch_ = PINA & 0x01; // read PortA, pin 0
/*		while(switch_ != 0) // man
		{
			forward();
			switch_ = PINA & 0x01;
		}
		
		while(switch_ == 0) //auto
		{
			backward();
			switch_ = PINA & 0x01;
		}*/

		if(button != 0)
			{
				if(switch_ != 0) // testkörning
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
			}
	
	}
	// Insert application code here, after the board has been initialized.
}
