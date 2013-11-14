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
//#include <avr/>
#include <avr/interrupt.h>

#define PULSE_WIDTH_L 0x00
#define PULSE_WIDTH_R 0x00
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

void msleep(uint8_t millisec);

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
	OCR1BL = 0x70;
	OCR1AL = 0x70;
}

void backward()
{
	PORTB = 0x00;
	OCR1BL = 0x70;
	OCR1AL = 0x70; 
}

void forward_left() 
{
	PORTB = 0x03;
	OCR1BL = 0xA0;
	OCR1AL = 0x30;
}

void forward_right()
{
	PORTB = 0x03;
	OCR1BL = 0x30;
	OCR1AL = 0xA0;
}

void rotate_right()
{
	PORTB = 0x02;
	OCR1BL = 0x70;
	OCR1AL = 0x70;
}

void rotate_left()
{
	PORTB = 0x01;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
}
void stop()
{
	PORTB = 0x00;
	OCR1BL = 0x00;
	OCR1AL = 0x00;
}

void grip_on()
{
	OCR3AL = 0x12; // 0x12 ca 1.25ms, 0x11 1.2ms 0x7 0.5 ms
}
void grip_off()
{
	OCR3AL = 0x1B; // 0x1B ca 1.9ms, 0x1D 2 ms 0x23 2.5 ms
}

#define TICKCOUNT 252
SIGNAL(SIG_OVERFLOW0)
{
	// Gör ingenting
}

void msleep(uint8_t millisec)
{
	while(millisec--)
	{
		TCNT0 = TICKCOUNT;
		MCUCR = 0;
		MCUCR = (1 << SE);
		asm volatile ("sleep");
	}
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
				forward();
				uint8_t t;
				uint8_t x;
				for(t=0; t<5; t++)
				{			
					for(x=0; x < 255; x++)
					{
						msleep(200);
					}
				}
				stop();
				
					for(x=0; x < 255; x++)
					{
						msleep(200);
					}
				
				backward();
				for(t=0; t<5; t++)
				{
					for(x=0; x < 255; x++)
					{
						msleep(200);
					}
				}											
					
				
			}
			else
			{
				grip_off();
			}
		if(switch_ != 0)
		{
			//forward();
			rotate_left();
		}
		else
		{
			stop();
		}			
			
	}
	// Insert application code here, after the board has been initialized.
}
