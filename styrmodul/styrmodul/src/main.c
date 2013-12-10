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
//#define F_CPU 1.843E6
#include <util/delay.h>
#include <avr/interrupt.h>

//Sensordata 
uint8_t control_command = 0x06, left_short_s, right_short_s, left_long_s, right_long_s, 
		forward_left_s, forward_right_s, forward_center_s, tape_value = 0x04, speed = 0x80;

//-------- PD-reglering --------//
uint8_t Kp_msb, Kp_lsb = 15, Kd_msb, Kd_lsb = 20; // Kp & Kd
signed e = 0; // Reglerfelet, 
signed int e_last = 0; // Det sparade reglerfelet


// Header och data som sänds och tas emot
volatile uint8_t header_s = 0;
volatile uint8_t data_s = 0;
volatile uint8_t header_r = 0;
volatile uint8_t data_r = 0x06; // Stop

//startknapp och switch
uint8_t button, switch_;

uint8_t grip_value = 0; // 0 = öppen griparm

#include <communication.h> // kommunikationen
#include <man_functions.h> // manuella funktioner
#include <auto_functions.h> // automatiska funktioner som kräver sensormodulen

#include "twi_slave.h" // I2C


ISR(INT2_vect) //Avbrott från sensormodulen: sluta rotera, gjort 90 grader
{
	keep_turning = 0;
}


int main (void)
{
	board_init();
	TWI_slave_initialize(TWI_CONTROL_MODULE_ADDRESS);
	
	pwm_start_L();
	pwm_start_R();
	pwm_start_G();
	
	sei(); // Avbrott
//uint8_t test2 = 0;
//float test = 25;
//test = test/10;
//test2 = test; 
	while(1)
	{
		switch_ = (PINA & 0x01); // Läs PortA, pinA0
		
		while(switch_ != 0) // Manuella läget
		{	
			manual_action(control_command);
			switch_ = (PINA & 0x01); // Läs PortA, pinA0
		}
		
		while(switch_ == 0) // Autonomt läge
		{
			button = PINA & 0x02; // Läs PortA, pinA0		

			
			if(button != 0) // Startar autonomt läge
			{
				//while(!goal_detected())	
				//{
				//	forward(0x70);	
				//}
				uint8_t grip_switch = 4;
				while(grip_switch == 4)
				{
					grip_switch = PINA & 0x04;
					goal_regulated();
				}
				stop();			
			}
			
			switch_ = PINA & 0x01;
		}
	}
}

