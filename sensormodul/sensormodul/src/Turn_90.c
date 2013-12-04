/*
Function Turn_90
Dai D Trinh 
30-11-2013


*/


#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Turn_90.h"
#include "Gyro.h"

volatile bool styrmodul_interrupt = false;
volatile bool turn_90_done = false;


ISR(INT2_vect)
{
	uint8_t cSREG;
	cSREG = SREG;
	
<<<<<<< HEAD
	int Angula_Rate, test_nr;
=======
	int Angula_Rate;
>>>>>>> Sensormodul
	float Angle=0, Degree=0; 
	styrmodul_interrupt = true;
	turn_90_done = false;
	

	TCNT0 = 0;   //set timer to 0 
<<<<<<< HEAD
	while(1)
	{
		int cnt=0;
=======
	while(!turn_90_done)
	{
	
>>>>>>> Sensormodul
		if(TCNT0 == 255 && !turn_90_done) // This happens every 0.01770833333s
		{
			Angula_Rate = SPI_GetRate();
			Angula_Rate = Angula_Rate - 1009;  // Use function SPI_GetNullvalue to get null value or set to 1009
			Angle = (float)((Angula_Rate/3.2)*0.01771);
			Degree += Angle;		
		} // End if
		
		if(abs(Degree) >= 90)
		{
			turn_90_done = 1;
			PORTB |= (1<<PORTB0); // Signal to Styrmodul that turn_90 is done
			break;
		}
		
	} // End while
<<<<<<< HEAD
//	_delay_ms(100);
//	PORTB &=(1<<PORTB0);
	
	
	
=======
>>>>>>> Sensormodul

	SREG = cSREG;
} // End ISR INT2



void do_turn_90(void)
{
	/*========== Timer0 Setup ========================*/
	TCCR0B = 0;  //stop timer
	TCNT0 = 0;   //set to 0
	TCCR0A = 0;
	TCCR0B = 1<<CS02 | 1<<CS00 ; // prescaler 1024 and start timer
	//TIMSK0 |= 1<<TOIE0;  //overflow int timer0   14745600/255/1024 = 56.47058824(Hz) => 1/f= 0.01770833333 times / sec
	/*================================================*/
	
	/*==Set BP0 and PB1 as output ==*/
	DDRB |= (1<<DDB0)|(1<<DDB1) ;		 
	PORTB &= (1<<PORTB0); //Int signal low
	
	while(1)
	{
<<<<<<< HEAD
		_delay_ms(300);
		PORTB &= ~(1<<PORTB0);
	
	//	PORTB &= ~(1<<PORTB0);
		PORTB = PORTB ^ 0x02;  //Toggle PB0 1.476 MHZ ;))
		//int test_rate = SPI_GetRate();

		
	//	PORTB &= ~(1<<PORTB0);	//BP0 low
=======
		_delay_ms(200);
	
		_delay_ms(100);
		//PORTB &= ~(1<<PORTB1);
	
	//	PORTB &= ~(1<<PORTB0);
	//	PORTB = PORTB ^ 0x02;  //Toggle PB1 1.476 MHZ ;))
		//int test_rate = SPI_GetRate();
		
		SPI_GetNullvalue();
		
		_delay_ms(200);
		PORTB &= ~(1<<PORTB0);	//BP0 low
>>>>>>> Sensormodul
	
	
	
	
	
	} //End while
}	  //End main		