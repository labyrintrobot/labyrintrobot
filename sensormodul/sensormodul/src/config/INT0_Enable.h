//================================================================
//================================================================
// INT0_Enable
// Dai D Trinh
// Version 1.1
// 2013-11-27
//================================================================
//================================================================


#ifndef INT0_ENABLE_H_
#define INT0_ENABLE_H_


void Int0_setup()
{
	sei();									 // Turn on global interrupt
	EICRA  = (1 << ISC00)|( 1 << ISC01);	 //Rising edge
	EIMSK |= (1 << INT0);					 // Enable INT0 using PORTD2 Pin16 to start integrating Gyro
	EIFR  |= (1 << INTF0);					 // External interrupts flag register
	
	
}

void Timer0_setup_start()
	{
	/*========== Timer0 Setup ========================*/
	TCCR0B = 0;  //stop timer
	TCNT0 = 0;   //set to 0
	TCCR0A = 0;
	TCCR0B = 1<<CS02 | 1<<CS00 ; // prescaler 1024 and start timer
	//TIMSK0 |= 1<<TOIE0;  //overflow int timer0   14745600/255/1024 = 56.47058824(Hz) => 1/f= 0.01770833333 times / sec
	/*================================================*/
	
	/*==Set BP0 and PB1 as output ==*/
	DDRB |= (1<<DDB0);
	PORTB &= (1<<PORTB0); //Int signal low
}


#endif /* INT0_ENABLE_H_ */