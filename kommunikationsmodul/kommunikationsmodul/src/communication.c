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
#include "communication.h"

void enable_irqs(void);

//global variables
volatile uint8_t control_module_interrupt=false;
volatile uint8_t sensor_module_interrupt=false;

volatile uint8_t  fireflyheader , fireflydata; // for messages received from the firefly by the interrupt.

volatile uint8_t fireflyreceiveddata=0;

//interrupt init
//Remember to set firefly interrupt flag
void enable_irqs() {
	
	
	EICRA |= (1 << ISC00) | (1 << ISC01); // Interrupts on rising edge
	EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	EIFR |= (1 << INTF0) | (1 << INTF1);
	
	sei(); //enable interrupts by setting I-bit in SREG
}

//Firefly interrupt routine

ISR(USART0_RX_vect){
	//should interrupt on USART0 received a byte,dunno what to do otherwise
	USART_receive(&fireflyheader , &fireflydata);
	
	fireflyreceiveddata=true;
}



//interrupt routine for int 0 and 1, here because pin 16 and 17 (INT0,1) is interrupt pins

// INT0 interrupts from control module
ISR(INT0_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	control_module_interrupt=true;
	
	SREG = cSREG; // restore
}

// INT1 interrupts from sensor module
ISR(INT1_vect) {
	uint8_t cSREG;
	cSREG = SREG;
	
	sensor_module_interrupt=1;
	
	SREG = cSREG; // restore
}

bool forward = true;

void mainfunction(){
	
	uint8_t header , data;
	
	bool receiveddata = false;
	
	enable_irqs();
	
	while(1){
		
		if(receiveddata){
			
			//send to right instance depending on header
			if (header==0x00){
				//firefly is sending styr commands, relay to styr module
				// TODO
				//TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS , header , data);
				
			} else if (header == 0x01){
				//styr module is sending, relay to firefly
				USART_transmit(header , data);
			} else if (header == 0x02){
				//firefly is sending a calibration command, relay to sensor module
				TWI_master_send_message(TWI_SENSOR_MODULE_ADDRESS , header , data);
							
			} else if (header >= 0x03 && header <= 0x0B){
				//sensor module is sending, relay to styr module and firefly
				
				// TODO
				//TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS , header , data);
				
				USART_transmit(header, data);
			} else if (header == 0x0C){
				// error. Send to firefly
			
				USART_transmit(header, data);
			} else if (header == 0x0D){
				//firefly is sending ping, relay back
				
				USART_transmit(header, data);
			} else {
				// Invalid header. Send error message
				USART_transmit(0x0C, 0x00);
			}
			
			receiveddata = false;
		}
		
		if (sensor_module_interrupt){
			
			//get data from sensor module
			TWI_master_receive_message(TWI_SENSOR_MODULE_ADDRESS, &header, &data);
			
			receiveddata = true;
			sensor_module_interrupt=false;
		} else if (control_module_interrupt){
			
			//get data from styr module
			TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
			
			receiveddata = true;
			control_module_interrupt=false;
		} else if (fireflyreceiveddata){
			
			cli();//shutdown interrupts, so that no funky business happens
			
			header=fireflyheader;
			data=fireflyheader;
			fireflyreceiveddata=false;
			
			sei();//shutdown interrupts
			
			receiveddata = true;
		}
	}
}