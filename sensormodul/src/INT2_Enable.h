/*
 * Setup Extern Int 
 *  
 * Filename: INT2_Enable
 * Created : 
 * Author  : Dai D Trinh
 */ 


#ifndef INT2_ENABLE_H_
#define INT2_ENABLE_H_

void Int2_setup()
{
	sei();									 // Turn on global interrupt
	EICRA  = (1 << ISC20)|( 1 << ISC21);	 //Rising edge
	EIMSK |= (1 << INT2);					 // Enable INT2
	EIFR  |= (1 << INTF2);					 // External interrupts flag register
	
	
}


#endif /* INT2_ENABLE_H_ */