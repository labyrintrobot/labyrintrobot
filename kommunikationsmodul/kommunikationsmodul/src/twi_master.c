/*
 * twi_master.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_master.h"

#define TWI_START_STATUS 0x08
#define TWI_SLAW_ACK_STATUS 0x18
#define TWI_DATA_WRITE_ACK_STATUS 0x28
#define TWI_SLAR_ACK_STATUS 0x40
#define TWI_DATA_REC_ACK_STATUS 0x50
#define TWI_DATA_REC_NACK_STATUS 0x58

void TWI_receive_data(unsigned char* data, int last_byte);
int TWI_send_start(void);
void TWI_send_stop(void);
int TWI_send_address(unsigned char address, int write);
int TWI_send_data(unsigned char data);

int TWI_initialize_bitrate(int bitrate) {
	if (bitrate == 5) {
		TWBR = 87; // Clockspeed
		TWSR |= 2; // Clockspeed scale
	} else if (bitrate == 50) {
		TWBR = 33;
		TWSR &= 0b11111101;
		TWSR |= 1;
	} else if(bitrate==200) {
		TWBR = 54;
		TWSR &= 0b11111100;
	} else if(bitrate==400) {
		TWBR = 24;
		TWSR &= 0b11111100;
	} else {
		return 1;
	}
	return 0;
}

int TWI_receive_data(unsigned char* data, int last_byte) {
	while (!(TWCR & (1<<TWINT)));
	*data = TWDR;
	
	if (last_byte && (TWSR & 0xF8) != TWI_DATA_REC_NACK_STATUS) {
		return 1;
	} else if ((TWSR & 0xF8) != TWI_DATA_REC_ACK_STATUS) {
		return 1;
	}
	return 0;
}

/************************************************************************/
/* Returns 0 if successful                                              */
/************************************************************************/
int TWI_send_start() {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))); // Wait START was successfully sent
	if ((TWSR & 0xF8) != TWI_START_STATUS) {
		return 0x01;
	}
	return 0;
}

void TWI_send_stop() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

/************************************************************************/
/* address is the ´7 most significant bits in address. If write is true,
it will be a write operation.       */
/************************************************************************/
int TWI_send_address(unsigned char address, int write) {
	
	if (address == 0) {
		return 0x02;
	}
	
	if (write) {
		address &= 0b11111110;
	} else {
		address |= 0b00000001;
	}
	
	TWDR = address; // Write address to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out address
	if (write && (TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) {
		return 0x03;
	} else if (TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) {
		return 0x04;
	}
	return 0;
}

int TWI_send_data(unsigned char data) {
	TWDR = data; // Write data to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out data
	if ((TWSR & 0xF8) != TWI_DATA_WRITE_ACK_STATUS) {
		return 0x05;
	}
	return 0;
}

int TWI_master_send_message(unsigned char address, unsigned char header, unsigned char data) {
	int err;
	
	err = TWI_send_start();
	if (err) return err;
	
	err = TWI_send_address(address, 1);
	if (err) return err;
	
	err = TWI_send_data(header);
	if (err) return err;
	
	err = TWI_send_data(data);
	if (err) return err;
	
	TWI_send_stop();
	
	return 0;
}

int TWI_master_receive_message(unsigned char address, unsigned char* header, unsigned char* data) {
	int err;
	
	err = TWI_send_start();
	if (err) return err;
	
	err = TWI_send_address(address, 1);
	if (err) return err;
	
	err = TWI_receive_data(header, 0);
	if (err) return err;
	
	err = TWI_receive_data(data, 1);
	if (err) return err;
	
	TWI_send_stop();
	
	return 0;
}