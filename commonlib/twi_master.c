/*
 * twi_master.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include <util/twi.h>
#include "twi_master.h"
#include "twi_common_private.h"

int TWI_master_receive_data(uint8_t* data, bool nack);
int TWI_master_send_start(void);
void TWI_master_send_stop(void);
int TWI_master_send_address(TWI_MODULE_ADDRESS to_address, bool write);
int TWI_master_send_data(uint8_t data);

/************************************************************************/
/* Initializes TWI. Sets the bitrate and enables TWI.                   */
/* bitrate: the bitrate in kHz                                          */
/************************************************************************/
int TWI_master_initialize(TWI_BITRATE bitrate) {
	
	int err = TWI_common_initialize(bitrate);
	if (err) return err;
	
	TWCR = (1<<TWEN); // Enable TWI
	
	return 0;
}

int TWI_master_receive_data(uint8_t* data, bool nack) {
	TWI_common_wait_for_TWINT();
	
	if (nack) {
		if (TWI_common_invalid_status(TW_MR_DATA_NACK)) {
			return 0x02;
		}
	} else {
		if (TWI_common_invalid_status(TW_MR_DATA_ACK)) {
			return 0x03;
		}
	}
	
	*data = TWDR;
	return 0;
}

/************************************************************************/
/* Returns 0 if successful                                              */
/************************************************************************/
int TWI_master_send_start() {
	
	while((1<<TWSTO) & TWCR); // Wait for stop to finish
	
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Send START condition
	TWI_common_wait_for_TWINT();
	
	if (TWI_common_invalid_status(TW_START)) {
		return 0x04;
	}
	return 0;
}

void TWI_master_send_stop() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

/************************************************************************/
/* address: The least significant bit is ignored                        */
/************************************************************************/
int TWI_master_send_address(TWI_MODULE_ADDRESS to_address, bool write) {
	
	if (write) {
		to_address &= 0x0FE; // Set LSB to 0 = write
	} else {
		to_address |= 0x01; // Set LSB to 1 = read
	}
	
	if (to_address == 0) {
		return 0x05;
	}
	
	TWDR = to_address; // Write address to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out address
	
	TWI_common_wait_for_TWINT();
	
	if (write) {
		if (TWI_common_invalid_status(TW_MT_SLA_ACK)) {
			//return TWSR & (0xF8);
			return 0x06;
		}
	} else {
		if (TWI_common_invalid_status(TW_MR_SLA_ACK)) {
			return 0x07;
		}
	}
	return 0;
}

int TWI_master_send_data(uint8_t data) {
	TWDR = data; // Write data to register
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	TWI_common_wait_for_TWINT();
	
	if (TWI_common_invalid_status(TW_MT_DATA_ACK)) {
		return 0x09;
	}
	return 0;
}

int TWI_master_send_message(TWI_MODULE_ADDRESS to_address, uint8_t header, uint8_t data) {
	
	int err = TWI_master_send_start();
	if (err) return err;
	
	err = TWI_master_send_address(to_address, true);
	if (err) return err;
	
	err = TWI_master_send_data(header);
	if (err) return err;
	
	err = TWI_master_send_data(data);
	if (err) return err;
	
	TWI_master_send_stop();
	
	return 0;
}

int TWI_master_receive_message(TWI_MODULE_ADDRESS from_address, uint8_t* header, uint8_t* data) {
	
	int err = TWI_master_send_start();
	if (err) return err;
	
	err = TWI_master_send_address(from_address, false);
	if (err) return err;
	
	err = TWI_master_receive_data(header, false);
	if (err) return err;
	
	err = TWI_master_receive_data(data, true);
	if (err) return err;
	
	TWI_master_send_stop();
	
	return 0;
}

void TWI_master_reset() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO) | (1<<TWEA);
}