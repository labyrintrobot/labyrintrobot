/*
* communication.c
*
* Created: 11/26/2013 5:43:02 PM
*  Author: kribo852
*/

#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi_master.h"
#include "USART.h"

//global variables
volatile uint8_t control_module_interrupt=0;
volatile uint8_t sensor_module_interrupt=0;

volatile uint8_t  fireflyheader , fireflydata; // for messages received from the firefly by the interrupt.

volatile uint8_t fireflyreceiveddata=0;


//interrupt init
//Remember to set firefly interrupt flag
void enable_irqs() {
	EICRA |= (1 << ISC00) | (1 << ISC01); // Interrupts on rising edge
	EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	EIFR |= (1 << INTF0) | (1 << INTF1);
	
	
	UCSR0B|=(1<<RXCIE0);//enable USART interrupts
	
	sei(); //enable interrupts by setting I-bit in SREG
}

//Firefly interrupt routine

ISR(USART0_RX_vect){
	
	//should interrupt on USART0 received a byte,dunno what to do otherwise
	USART_receive(&fireflyheader , &fireflydata);
	
	fireflyreceiveddata=1;
}



//interrupt routine for int 0 and 1, here because pin 16 and 17 (INT0,1) is interrupt pins

// INT0 interrupts from control module
ISR(INT0_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	control_module_interrupt=1;
	
	SREG = cSREG; // restore
}

// INT1 interrupts from sensor module
ISR(INT1_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	sensor_module_interrupt=1;
	
	SREG = cSREG; // restore
}



void mainfunction(){
	
	uint8_t header , data;
	
	uint8_t receiveddata=0;
	
	enable_irqs();
	
	while(1){
		
		if(receiveddata){
			
			//send to right instance depending on header
			if(header==0){
				//styr module is sending, relay to firefly
				
				USART_transmit(header , data);
			}
			
			if(header>=3 && header<=9){
				//sensor module is sending, relay to styr module and firefly
				
				TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS , header , data);
				USART_transmit(header , data);
			}
			
			if(header==1){
				//firefly is sending styr commands, relay to styr module
				
				TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS , header , data);
				
			}
			
			if(header==2){
				//firefly is sending a calibration command, relay to sensor module
				
				TWI_master_send_message(TWI_SENSOR_MODULE_ADDRESS , header , data);
				
			}
			
			
			receiveddata=false;
		}
		
		if(sensor_module_interrupt){
			
			
			//get data from sensor module
			TWI_master_receive_message(TWI_SENSOR_MODULE_ADDRESS, &header, &data);
			
			receiveddata=true;
			sensor_module_interrupt=0;
		}
		else if(control_module_interrupt){
			
			//get data from styr module
			TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
			
			receiveddata=true;
			control_module_interrupt=0;
		}
		else if(fireflyreceiveddata){
			
			cli();//shutdown interrupts, so that no funky business happens
			
			header=fireflyheader;
			data=fireflyheader;
			fireflyreceiveddata=false;
			
			sei();//shutdown interrupts
			
			receiveddata=1;
		}
		
		
	}
	
}