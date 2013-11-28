//================================================================
//================================================================
// Gyro.c
// Dai D Trinh
//================================================================
//================================================================
#include<asf.h>
#include <util/delay.h>
#include <interrupt.h>
#include "Gyro.h"

#define SS_Low PORTB &= ~(1<<PORTB4)
#define SS_High PORTB |= (1<<PORTB4)


unsigned char SPI_EXCH (unsigned char output);
unsigned int SPI_Init(void);
unsigned int SPI_GetRate(void);
unsigned int SPI_GetTemp(void) ;
int SPI_GetNullvalue(void);


//SPI interface initialize and waking up the Gyro ADC . Do once at start up.
unsigned int SPI_Init(void)
{
	/* Set direction for SPI pins SCK, MOSI, SS is output */
	DDRB |= (1<<DDB4) | (1<<DDB5) | (1<<DDB7);
	/* Set SPI double frequency */
	SPSR = (1<<SPI2X);
	/* Configure SPI mode: */
	/* Enable SPI, Master mode, MSB first, SCK high when idle, trailing edge sampling, Fc/8, */
	
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<DORD)|(1<<CPOL)|(1<<CPHA)|(1<<SPR0) ;
	
	//Wake up !!!
	unsigned int ADC_response;
	SS_Low;
	SPI_EXCH(0b10011100);
	ADC_response = SPI_EXCH(0x00)<<8;
	ADC_response |= SPI_EXCH(0x00);
	SS_High;
	_delay_us(200); //SO that we dont read the ADC direct after init.
	
	return 0;
}
//================================================================
// Exchange data with slave, send one byte to receive one byte
unsigned char SPI_EXCH (unsigned char output)
{
	SPDR = output; /* Start transmission */
	/* Wait till a transmission and reception are completed */
	while(!(SPSR & (1<<SPIF)));
	return SPDR; /* Return Data Register */
}
//================================================================
unsigned int SPI_GetRate(void)
{
	
	unsigned int Rate, Dummy_response;
	SS_Low;  //SS Low >> Slave selected (G-sensorn)
	
	SPI_EXCH(0b10010100); //Send ADC command for angular rate
	Dummy_response = SPI_EXCH(0x00)<<8;  //Ignore the 16 bit response, don't bother check for ERROR or refusal answer ;)
	Dummy_response |= SPI_EXCH(0x00);

	SPI_EXCH(0b10000000); //Send ADCC Read command
	//_delay_us(150);  //Do we need this delay ????
	
	//Read 2 byte and put in Rate;
	Rate = SPI_EXCH(0x00)<<8;
	Rate |= SPI_EXCH(0x00);
	
	SS_High; //Disable slave;
	
	Rate = 0x7FF & (Rate>>1);// Mask unused bits [00001111.1111111x] >> [00000111.11111111] 11 bits data from Gyro
	
	return Rate;
	
}



//================================================================
unsigned int SPI_GetTemp(void)
{
	
	unsigned int Temp, Dummy_response;
	SS_Low;  //SS Low >> Slave selected (G-sensorn)
	
	SPI_EXCH(0b10011100); //Send ADC command for temperature
	Dummy_response = SPI_EXCH(0x00)<<8;  //Ignore the 16 bit response, don't bother check for ERROR or refusal answer ;)
	Dummy_response |= SPI_EXCH(0x00);
	//_delay_us(150);
	
	SPI_EXCH(0b10000000); //Send Read command
	//_delay_us(150); //Do we need this delay ???
	
	//Read 2 byte and put in Rate;
	Temp = SPI_EXCH(0x00)<<8;
	Temp |= SPI_EXCH(0x00);
	
	SS_High; //Disable slave;
	
	Temp = 0x7FF & (Temp>>1);// Mask unused bits [00001111.1111111x] >> [00000111.11111111] 11 bits data from Gyro
	
	return Temp;
	
}

//================================================================
// Get null value at start. Make sure the thing is standing still !!!
int SPI_GetNullvalue(void)
{
	long Rate_Temp=0;

	int average=1024, null = 0;
	
	while (average )
	{
		_delay_ms(1);
		Rate_Temp += SPI_GetRate();
		average--;
	}
	null = Rate_Temp/1024;
	
	return null;
}

