/*
 * twi_master.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_master.h"

#define TWI_SLAR_ACK_STATUS 0xA8
#define TWI_DATA_WRITE_ACK_STATUS 0xB8
#define TWI_LAST_DATA_WRITE_ACK_STATUS 0xC8

#define TWI_SLAW_ACK_STATUS 0x60
#define TWI_DATA_REC_ACK_STATUS 0x80

#define TWI_DATA_REC_NACK_STATUS 0x88

#define TWI_REP_START_STOP_STATUS 0xA0

//function that receives data via I2C if interrupt is raised
int TWI_receive_address(unsigned char* slarrw){
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TWI_SLAW_ACK_STATUS || (TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) {
		return 1;
	}
	*slarrw = TWDR;
	return 0;
}

int TWI_receive_data(unsigned char* data , int last_byte){
	while (!(TWCR & (1<<TWINT)));
	
	if(last_byte){
		if ((TWSR & 0xF8) != TWI_DATA_NACK_STATUS) {
			return 1;
		}
		
	}else{
		if ((TWSR & 0xF8) != TWI_DATA_ACK_STATUS) {
			return 1;
		}
		
	}
	*slarrw = TWDR;
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

int TWI_send_data(unsigned char data, int last_data) {
	TWDR = data; // Write data to register
	if (last_data) {
		TWCR = (1<<TWINT) | (1<<TWEN) | (1 << TWEA); // Send out data
		if ((TWSR & 0xF8) != TWI_LAST_DATA_WRITE_ACK_STATUS) {
			return 1;
		}
	} else {
		TWCR = (1<<TWINT) | (1<<TWEN); // Send out data
		if ((TWSR & 0xF8) != TWI_DATA_WRITE_ACK_STATUS) {
			return 1;
		}
	}

	return 0;
}

//function that sends data if via I2C if needed
int TWI_slave_send_message(unsigned char header, unsigned char data) {
	int err;
	
	// send interrupt flank
	
	TWI_wait_for_start();
	unsigned char address;
	TWI_receive_address(&address);
	int read = address & 0x01;
	
	address=(address&0xFE)>>1;
	// TODO check for address
	
	if (!read) {
		return 1;
	}
	if ((TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) {
		return 1;
	}
	
	err = TWI_send_data(header, 0);
	if (err) return err;
	
	err = TWI_send_data(data, 1);
	if (err) return err;
	
	
	return 0;
}

int TWI_slave_receive_message(unsigned char *header , unsigned char *data){
	int err;
	// Set enable TWINT interrupts
	
	TWI_wait_for_start();
	unsigned char address;
	TWI_receive_address(&address);
	int read = address & 0x01;
	
	address = (address & 0xFE) >> 1;
	// TODO check for address
		
	if (read) {
		return 1;
	}
	if ((TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) {
		return 1;
	}
	
	err = TWI_receive_data(header, 0);
	if (err) return 1;
	
	err = TWI_receive_data(data, 1);
	if (err) return 1;
	
	TWI_send_stop();
	
	return 0;
}

int TWI_initialize_bitrate(int bitrate) {
	if (bitrate == 5){
		TWBR=87;//init clockspeed
		TWSR|=2;//init clockspeed scale
	} else if (bitrate == 50){
		TWBR=33;//init clockspeed
		TWSR&=0b11111101;//init clockspeed scale
		TWSR|=1;//init clockspeed scale
	} else if (bitrate == 200){
		TWBR=54;//init clockspeed
		TWSR&=0b11111100;//init clockspeed scale
	} else if (bitrate == 400){
		TWBR=24;//init clockspeed
		TWSR&=0b11111100;//init clockspeed scale
	} else {
		return 1;
	}
	return 0;
}

// Bit to the left is ignored
void TWI_initialize(unsigned char* address) {
	TWAR = (address << 1) & 0xFE;
	TWAMR = 0x00;
	TWCR = (1<<TWEN) | (1<<TWEA);
}