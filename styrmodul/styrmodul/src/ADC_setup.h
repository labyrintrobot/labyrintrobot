/*
 * ADC_setup.h
 *
 * Created: 2013-11-14 17:21:57
 *  Author: Hampus Nilsson
 */ 


#ifndef ADC_SETUP_H_
#define ADC_SETUP_H_

void ADCsetup()
{
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set the prescale of the clock to 128 so the ADC clock becomes systemclock/128
	ADMUX  |= (1 << REFS0); // Set reference to AVCC
	ADMUX  |= (1 << ADLAR); // Allows the 8 bit data to be stored to the ADCH register for 8 bit precision
	ADMUX = 0x62; // Read ADC2
	
	ADCSRA |= 1<<ADIE; // Set ADC interupt enable
	ADCSRA |= 1<<ADEN; // Start the ADC
}



#endif /* ADC_SETUP_H_ */