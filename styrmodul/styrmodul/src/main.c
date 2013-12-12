/*
 * main.c
 *
 * Latest update: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Now�n, Kim Persson
 *
 * Styrmodulens huvudprogram som k�r det manuella eller autonoma l�get.
 *
 */ 

#include <asf.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
#define F_CPU 14.745E6
#include <util/delay.h>
#include <avr/interrupt.h>


// Sensordata 
uint8_t control_command = 0x06, left_short_s, right_short_s, left_long_s, right_long_s, 
		forward_left_s, forward_right_s, forward_center_s, tape_value = 0x04;

//-------- PD-reglering --------//
uint8_t speed = 0x80;
uint8_t Kp_lsb = 15, Kd_lsb = 20;	// Kp & Kd
signed int e = 0;						// Reglerfelet, 
signed int e_last = 0;				// Det sparade reglerfelet


// Header och data som s�nds och tas emot
volatile uint8_t header_s = 0;
volatile uint8_t data_s = 0;
volatile uint8_t header_r = 0;
volatile uint8_t data_r = 0x06;		// 0x06 = Stop

// Startknapp och switch
uint8_t button, switch_;

uint8_t grip_value = 0; // 0 = �ppen griparm

#include <communication.h>	// kommunikationen
#include <man_functions.h>	// manuella funktioner
#include <auto_functions.h>	// automatiska funktioner som kr�ver sensormodulen

#include "twi_slave.h"		// I2C

// Avbrott fr�n sensormodulen: sluta rotera, gjort 90 grader
ISR(INT2_vect) 
{
	keep_turning = 0;
}


int main (void)
{
	board_init();	// Initierar portar, avbrott och PWM 
	TWI_slave_initialize(TWI_CONTROL_MODULE_ADDRESS);
	
	pwm_start_L();	// Starta PWM f�r v�nster hjulpar
	pwm_start_R();	// Starta PWM f�r h�ger hjulpar
	pwm_start_G();	// Starta PWM f�r griparmen
	
	sei(); // Aktivera avbrott

	while(1)
	{
		switch_ = (PINA & 0x01); // L�s PORTA, PINA0
		
		while(switch_ != 0)	// Manuella l�get
		{	
			manual_action(control_command);
			switch_ = (PINA & 0x01); // L�s PORTA, PINA0
		}
		
		while(switch_ == 0) // Autonomt l�ge
		{
			button = PINA & 0x02; // L�s PortA, pinA0		

			
			if(button != 0) // Startar autonomt l�ge
			{	
				find_start();
				find_goal();
				get_target();
				return_to_start();			
			}
			
			switch_ = PINA & 0x01;	// L�s PORTA, PINA0
		}
	}
}

