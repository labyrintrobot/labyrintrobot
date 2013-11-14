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
#define PULSE_WIDTH_G 0x15

void pwm_start_L();
void pwm_start_R();
void pwm_start_G();

void forward();
void backward();
void rotate_right();
void rotate_left();
void forward_right();
void forward_left();
void stop();
void grip_on();
void grip_off();

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
	TCCR3B = 3;
}

void forward()
{
	PORTB = 0x03;
	OCR1BL = 0x60;
	OCR1AL = 0x60;
}

void backward()
{
	PORTB = 0x00;
	OCR1BL = 0x60;
	OCR1AL = 0x60;
}

void stop()
{
	PORTB = 0x00;
	OCR1BL = 0x00;
	OCR1AL = 0x00;
}

void grip_on()
{
	OCR3AL = 0x11; // 0x11 1.2ms 0x7 0.5 ms
}
void grip_off()
{
	OCR3AL = 0x1D; //0x1D 2 ms 0x23 2.5 ms
}

int main (void)
{
	board_init();
	
	pwm_start_L();
	pwm_start_R();
	pwm_start_G();
	uint8_t button, switch_;
	PORTB = 0xC0;
	
	while(1)
	{
		button = PINA & 0x02; // read PortA, pin 1
		switch_ = PINA & 0x01; // read PortA, pin 0
			
		if(button != 0)
			{
				grip_on();
			}
			else
			{
				grip_off();
			}
		if(switch_ != 0)
		{
			forward();
		}
		else
		{
			stop();
		}			
			
	}
	// Insert application code here, after the board has been initialized.
}
