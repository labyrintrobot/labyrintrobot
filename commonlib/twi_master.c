/*
 * twi_master.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_master.h"
#include "twi_common_private.h"

enum STATUS {
	TWI_START_STATUS = 0x08,
	TWI_SLAW_ACK_STATUS = 0x18,
	TWI_DATA_WRITE_ACK_STATUS = 0x28,
	TWI_DATA_WRITE_NACK_STATUS = 0x28,
	TWI_SLAR_ACK_STATUS = 0x40,
	TWI_DATA_REC_ACK_STATUS = 0x50,
	TWI_DATA_REC_NACK_STATUS = 0x58
	};

int TWI_master_receive_data(uint8_t* data, bool nack);
int TWI_master_send_start(void);
void TWI_master_send_stop(void);
int TWI_master_send_address(TWI_MODULE_ADDRESS to_address, bool write);
int TWI_master_send_data(uint8_t data, bool last_byte);

int TWI_master_receive_data(uint8_t* data, bool nack) {
	TWI_common_wait_for_TWINT();
	*data = TWDR;
	
	if (nack) {
		if (TWI_common_invalid_status(TWI_DATA_REC_NACK_STATUS)) {
			return 0x02;
		}
	} else {
		if (TWI_common_invalid_status(TWI_DATA_REC_ACK_STATUS)) {
			return 0x03;
		}
	}
	return 0;
}

/************************************************************************/
/* Returns 0 if successful                                              */
/************************************************************************/
int TWI_master_send_start() {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))); // Wait until START was successfully sent
	
	if (TWI_common_invalid_status(TWI_START_STATUS)) {
		return 0x04;
	}
	return 0;
}

void TWI_master_send_stop() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

/************************************************************************/
/* address is the ´7 most significant bits in address. If write is true,
it will be a write operation.       */
/************************************************************************/
int TWI_master_send_address(TWI_MODULE_ADDRESS to_address, bool write) {
	
	if (write) {
		to_address &= 0x0FE; // Set LSB to 0
	} else {
		to_address |= 0x01; // Set LSB to 1
	}
	
	if (to_address == 0) {
		return 0x05;
	}
	
	TWDR = to_address; // Write address to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out address
	
	if (write) {
		if (TWI_common_invalid_status(TWI_SLAW_ACK_STATUS)) {
			return 0x06;
		}
	} else {
		if (TWI_common_invalid_status(TWI_SLAR_ACK_STATUS)) {
			return 0x07;
		}
	}
	return 0;
}

int TWI_master_send_data(uint8_t data, bool nack) {
	TWDR = data; // Write data to register
	if (nack) {
		TWCR = (1<<TWINT) | (1<<TWEN);
		if (TWI_common_invalid_status(TWI_DATA_WRITE_NACK_STATUS)) {
			return 0x08;
		}
	} else {
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
		if (TWI_common_invalid_status(TWI_DATA_WRITE_ACK_STATUS)) {
			return 0x09;
		}
	}
	return 0;
}

int TWI_master_send_message(TWI_MODULE_ADDRESS to_address, uint8_t header, uint8_t data) {
	
	TWI_common_disable_interrupt();
	
	int err = TWI_master_send_start();
	if (err) return err;
	
	err = TWI_master_send_address(to_address, true);
	if (err) return err;
	
	err = TWI_master_send_data(header, false);
	if (err) return err;
	
	err = TWI_master_send_data(data, true);
	if (err) return err;
	
	TWI_master_send_stop();
	
	TWI_common_enable_interrupt();
	
	return 0;
}

int TWI_master_receive_message(TWI_MODULE_ADDRESS from_address, uint8_t* header, uint8_t* data) {
	
	TWI_common_disable_interrupt();
	
	int err = TWI_master_send_start();
	if (err) return err;
	
	err = TWI_master_send_address(from_address, false);
	if (err) return err;
	
	err = TWI_master_receive_data(header, false);
	if (err) return err;
	
	err = TWI_master_receive_data(data, true);
	if (err) return err;
	
	TWI_master_send_stop();
	
	TWI_common_enable_interrupt();
	
	return 0;
}