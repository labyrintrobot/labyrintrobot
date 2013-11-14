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

#define PULSE_WIDTH_L 0x50
#define PULSE_WIDTH_R 0x50
#define PULSE_WIDTH_G 0x60

void pwm_start_L();
void pwm_start_R();
void pwm_start_G();



void pwm_start_L()
{
	OCR1AL = PULSE_WIDTH_L;
	OCR1AH = 0;
	TCCR1B = 1;
}

void pwm_start_R()
{
	OCR1BL = PULSE_WIDTH_R;
	OCR1BH = 0;
	TCCR1B = 1;
}

void pwm_start_G()
{
	OCR3AL = PULSE_WIDTH_G;
	OCR3AH = 0;
	TCCR3B = 1;
}


int main (void)
{
	board_init();
	
	pwm_start_L();
	pwm_start_R();
	pwm_start_G();
	uint8_t temp;
	PORTB = 0xC0;
	
	while(1)
	{
		temp = PINA;	
		if(temp != 0)
			{
			PORTB = 0x03;
			OCR1BL = 0x80;
			OCR1AL = 0x38;
			}
			else
			{
			PORTB = 0x00;
			OCR1BL = 0x00;
			OCR1AL = 0x00;
			}
			
	}
	// Insert application code here, after the board has been initialized.
}
