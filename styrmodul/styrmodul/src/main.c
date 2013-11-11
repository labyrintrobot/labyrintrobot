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

int main (void)
{
	board_init();
	while (1)
	{
		PORTB = 0xff;
	}
	// Insert application code here, after the board has been initialized.
}

void board_init()
{
	DDRA = (1<<DDA3)|(1<<DDA2)|(1<<DDA1)|(1<<DDA0);
	DDRB = (1<<DDB1)|(1<<DDB0);
	DDRC = (1<<DDC1)|(1<<DDC0);
	DDRD = ((1<<DDD6)|(1<<DDD5)|(1<<DDD4));
	
	
}