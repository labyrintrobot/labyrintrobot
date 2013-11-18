/*
 * twi_master.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_common.h"
#include "twi_slave.h"

void TWI_wait_for_start(void);
int TWI_receive_address(TWI_ADDRESS* from_address, bool* write);
int TWI_receive_data(uint8_t* data, bool nack);
int TWI_send_address(TWI_ADDRESS to_address, bool write);
int TWI_send_data(uint8_t data, bool nack);

enum TWI_STATUS {
	TWI_SLAR_ACK_STATUS = 0xA8,
	TWI_DATA_WRITE_ACK_STATUS = 0xB8,
	TWI_LAST_DATA_WRITE_ACK_STATUS = 0xC8,
	TWI_SLAW_ACK_STATUS = 0x60,
	TWI_DATA_REC_ACK_STATUS = 0x80,
	TWI_DATA_REC_NACK_STATUS = 0x88,
	TWI_REP_START_STOP_STATUS = 0xA0
	};

int TWI_receive_address(TWI_ADDRESS* from_address, bool* write) {
	TWI_wait_for_TWINT();
	if ((TWSR & 0xF8) != TWI_SLAW_ACK_STATUS || (TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) {
		return 1;
	}
	
	*from_address = TWDR & 0xFE;
	*write = TWDR & 0x01;
	
	return 0;
}

int TWI_receive_data(uint8_t* data, bool nack) {
	TWI_wait_for_TWINT();
	
	if(nack) {
		if ((TWSR & 0xF8) != TWI_DATA_REC_NACK_STATUS) {
			return 0x0A;
		}
		
	} else {
		if ((TWSR & 0xF8) != TWI_DATA_REC_ACK_STATUS) {
			return 0x0B;
		}
		
	}
	*data = TWDR;
	return 0;
}

int TWI_send_address(TWI_ADDRESS to_address, bool write) {
	
	if (to_address == 0) {
		return 2;
	}
	
	if (write) {
		to_address &= 0xFE;
	}
	else {
		to_address |= 0x01;
	}
	
	TWDR = to_address; // Write address to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out address
	if ((write && (TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) || (!write && (TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) ) {
		return 0x0C;
	}
	return 0;
}

int TWI_send_data(uint8_t data, bool nack) {
	TWDR = data; // Write data to register
	if (nack) {
		TWCR = (1<<TWINT) | (1<<TWEN) | (1 << TWEA); // Send out data
		if ((TWSR & 0xF8) != TWI_LAST_DATA_WRITE_ACK_STATUS) {
			return 0x0D;
		}
	} else {
		TWCR = (1<<TWINT) | (1<<TWEN); // Send out data
		if ((TWSR & 0xF8) != TWI_DATA_WRITE_ACK_STATUS) {
			return 0x0E;
		}
	}

	return 0;
}

/************************************************************************/
/* Sends a message via TWI                                              */
/* header: the message header                                           */
/* data: the message data                                               */
/* returns nonzero if error                                             */
/************************************************************************/
int TWI_slave_send_message(uint8_t header, uint8_t data) {
	
	TWI_disable_interrupt();
	
	// send interrupt flank
	
	TWI_ADDRESS address;
	bool write;
	int err = TWI_receive_address(&address, &write);
	
	// TODO validate address?
	
	if (write) {
		return 0x0F;
	}
	if ((TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) {
		return 0x10;
	}
	
	err = TWI_send_data(header, false);
	if (err) return err;
	
	err = TWI_send_data(data, true);
	if (err) return err;
	
	TWI_enable_interrupt();
	
	return 0;
}

/************************************************************************/
/* Blocks until a message is received via TWI                           */
/* header[out]: the message header that was received                    */
/* data[out]: the message data                                          */
/* returns nonzero if error                                             */
/************************************************************************/
int TWI_slave_receive_message(uint8_t *header, uint8_t *data) {
	
	TWI_disable_interrupt();
	
	TWI_ADDRESS address;
	bool write;
	int err = TWI_receive_address(&address, &write);
	
	// TODO validate address?
		
	if (! write) {
		return 0x11;
	}
	if ((TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) {
		return 0x12;
	}
	
	err = TWI_receive_data(header, false);
	if (err) return 1;
	
	err = TWI_receive_data(data, true);
	if (err) return 1;
	
	TWI_enable_interrupt();
	
	return 0;
}