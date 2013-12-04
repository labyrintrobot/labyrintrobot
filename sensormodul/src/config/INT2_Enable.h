/*
 * Setup Extern Int 
 *  
 * Filename: INT2_Enable
 * Created : 
 * Author  : Dai D Trinh
 */ 


#ifndef INT2_ENABLE_H_
#define INT2_ENABLE_H_
<<<<<<< HEAD
void Int2_setup();
=======


>>>>>>> Sensormodul
void Int2_setup()
{
	sei();									 // Turn on global interrupt
	EICRA  = (1 << ISC20)|( 1 << ISC21);	 //Rising edge
	EIMSK |= (1 << INT2);					 // Enable INT2
	EIFR  |= (1 << INTF2);					 // External interrupts flag register
	
	
}

<<<<<<< HEAD
=======
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
	DDRB |= (1<<DDB0) ;
	PORTB &= (1<<PORTB0); //Int signal low
}

>>>>>>> Sensormodul

#endif /* INT2_ENABLE_H_ */