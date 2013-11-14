/*
 * twi.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: emibe709
 */ 

#include <asf.h>
#include "twi.h"

//function that receives data via I2C if interrupt is raised
void TWI_receive_data(unsigned char* data){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	*data = TWDR;
}

/************************************************************************/
/* Returns 0 if successful                                              */
/************************************************************************/
int TWI_send_start(int repeated) {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))); // Wait until TWINT has been set = START was successfully sent
	if (repeated) {
		if ((TWSR & 0xF8) != TWI_REP_START_STATUS) {
			return 1;
		}
		} else {
		if ((TWSR & 0xF8) != TWI_START_STATUS) {
			return 1;
		}
	}
	return 0;
}

void TWI_send_stop() {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

/************************************************************************/
/* address is the ´7 most significant bits in address. If write is true,
it will be a write operation.       */
/************************************************************************/
int TWI_send_address(unsigned char address , int write) {
	
	if (address == 0) {
		return 2;
	}
	
	if(write){
		address&=0b11111110;
	}
	else{
		address|=0b00000001;
	}
	
	TWDR = address; // Write address to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out address
	if ((write && (TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) || (!write && (TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) ) {
		return 1;
	}
	return 0;
}

int TWI_send_data(unsigned char data) {
	TWDR = data; // Write data to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out data
	if ((TWSR & 0xF8) != TWI_DATA_WRITE_ACK_STATUS) {
		return 1;
	}
	return 0;
}

//function that sends data if via I2C if needed
int TWI_master_send_message(unsigned char address, unsigned char header, unsigned char data) {
	int err;
	
	err = TWI_send_start(0);
	if (err) return err;
	
	err = TWI_send_address(address , 1);
	if (err) return err;
	
	err = TWI_send_data(header);
	if (err) return err;
	
	err = TWI_send_data(data);
	if (err) return err;
	
	TWI_send_stop();
	
	return 0;
}

int TWI_master_receive_message(unsigned char address , unsigned char *header , unsigned char *data){
	int err;
	
	err = TWI_send_start(0);
	if (err) return err;
	
	err = TWI_send_address(address , 1);
	if (err) return err;
	
	TWI_receive_data(header);
	
	TWI_receive_data(data);
	
	TWI_send_stop();
	
	return 0;
}

int TWI_initialize_bitrate(int bitrate){
	
	if(bitrate==5){
		TWBR=87;//init clockspeed
		TWSR|=2;//init clockspeed scale
	}
	
	else if(bitrate==50){
		TWBR=33;//init clockspeed
		TWSR&=0b11111101;//init clockspeed scale
		TWSR|=1;//init clockspeed scale
	}
	
	else if(bitrate==200){
		TWBR=54;//init clockspeed
		TWSR&=0b11111100;//init clockspeed scale
	}
	
	else if(bitrate==400){
		TWBR=24;//init clockspeed
		TWSR&=0b11111100;//init clockspeed scale
	}
	else{
		return 1;
		
	}
	return 0;
}