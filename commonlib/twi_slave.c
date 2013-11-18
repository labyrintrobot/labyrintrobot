/*
 * twi_master.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_common.h"
#include "twi_slave.h"

int TWI_receive_address(enum TWI_ADDRESS* from_address, bool* write);
int TWI_receive_data(uint8_t* data, bool nack);
void TWI_send_stop(void);
int TWI_send_address(enum TWI_ADDRESS to_address, bool write);
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

int TWI_receive_address(enum TWI_ADDRESS* from_address, bool* write) {
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
			return 1;
		}
		
	} else {
		if ((TWSR & 0xF8) != TWI_DATA_REC_ACK_STATUS) {
			return 1;
		}
		
	}
	*data = TWDR;
	return 0;
}

void TWI_send_stop() {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

/************************************************************************/
/* address is the ´7 most significant bits in address. If write is true,
it will be a write operation.       */
/************************************************************************/
int TWI_send_address(enum TWI_ADDRESS to_address, bool write) {
	
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
		return 1;
	}
	return 0;
}

int TWI_send_data(uint8_t data, bool nack) {
	TWDR = data; // Write data to register
	if (nack) {
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
int TWI_slave_send_message(uint8_t header, uint8_t data) {
	
	TWI_disable_interrupt();
	
	// send interrupt flank
	
	int err = TWI_wait_for_start();
	
	TWI_ADDRESS address;
	bool write;
	err = TWI_receive_address(&address, &write);
	
	// TODO validate address?
	
	if (write) {
		return 1;
	}
	if ((TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) {
		return 1;
	}
	
	err = TWI_send_data(header, false);
	if (err) return err;
	
	err = TWI_send_data(data, true);
	if (err) return err;
	
	TWI_enable_interrupt();
	
	return 0;
}

int TWI_slave_receive_message(uint8_t *header, uint8_t *data) {
	
	TWI_disable_interrupt();
	
	int err = TWI_wait_for_start();
	TWI_ADDRESS address;
	bool write;
	TWI_receive_address(&address, &write);
	
	// TODO validate address?
		
	if (! write) {
		return 1;
	}
	if ((TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) {
		return 1;
	}
	
	err = TWI_receive_data(header, false);
	if (err) return 1;
	
	err = TWI_receive_data(data, true);
	if (err) return 1;
	
	TWI_send_stop();
	
	TWI_enable_interrupt();
	
	return 0;
}