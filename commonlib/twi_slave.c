/*
 * twi_slave.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include <util/twi.h>
#include "twi_slave.h"
#include "twi_common_private.h"

int TWI_slave_receive_data(uint8_t* data);
int TWI_slave_send_data(uint8_t data, bool nack);
int TWI_slave_wait_for_stop(bool write);

enum TWI_STATUS {
	TWI_SLAR_ACK_STATUS = 0xA8,
	TWI_DATA_WRITE_ACK_STATUS = 0xB8,
	TWI_LAST_DATA_WRITE_ACK_STATUS = 0xC8,
	TWI_SLAW_ACK_STATUS = 0x60,
	TWI_DATA_REC_ACK_STATUS = 0x80,
	TWI_DATA_REC_NACK_STATUS = 0x88,
	TWI_REP_START_STOP_STATUS = 0xA0
	};
	
volatile bool slave_initialized = false;
volatile bool slave_addressed = false;
	
int TWI_slave_initialize(TWI_MODULE_ADDRESS my_address, int bitrate) {
	
	if (slave_initialized) {
		return 0x02;
	}
	
	int err = TWI_common_initialize(my_address, bitrate);
	if (err) return err;
	
	// Enable TWI, TWI address listening and interrupts
	TWCR = (1<<TWEN) | (1<<TWEA) | (1<<TWIE);
	
	slave_initialized = true;
	
	return 0;
}

int TWI_slave_wait_for_address(bool* should_receive) {
	
	if (!slave_initialized) {
		return 0x01;
	}
	if (slave_addressed) {
		return 0x02;
	}
	
	TWI_common_wait_for_TWINT();
	
	bool invalid_slaw = TWI_common_invalid_status(TWI_SLAW_ACK_STATUS);
	bool invalid_slar = TWI_common_invalid_status(TWI_SLAR_ACK_STATUS);
	if (invalid_slaw && invalid_slar) {
		return 0x03;
	}
	*should_receive = ! invalid_slaw;
	
	if (*should_receive) {
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	}
	
	slave_addressed = true;
	
	return 0;
}

int TWI_slave_receive_data(uint8_t* data) {
	
	TWI_common_wait_for_TWINT();
	
	if (TWI_common_invalid_status(TWI_DATA_REC_ACK_STATUS)) {
		return 0x04;
	}
	*data = TWDR;
	
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	
	return 0;
}

int TWI_slave_send_data(uint8_t data, bool nack) {
	
	TWDR = data; // Write data to register
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	
	TWI_common_wait_for_TWINT();
	
	if (nack) {
		if (TWI_common_invalid_status(TW_ST_DATA_NACK)) {
			return 0x05;
		}
	} else {
		if (TWI_common_invalid_status(TW_ST_DATA_ACK)) {
			return 0x06;
		}
	}

	return 0;
}

int TWI_slave_wait_for_stop(bool write) {
	if (write) {
		TWI_common_wait_for_TWINT();
		
		if (TWI_common_invalid_status(TWI_REP_START_STOP_STATUS)) {
			return 0x07;
		}
	}
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA); // TODO
	
	slave_addressed = false;
	
	return 0;
}

/************************************************************************/
/* Sends a message to the communication module via TWI. First, it
issues an interrupt to the communication module.                        */
/* header: the message header                                           */
/* data: the message data                                               */
/* start_sending_irq_fun: a pointer to a function that starts sending 
the interrupt to the communication module                               */
/* stop_sending_irq_fn: a pointer to a function that stops sending
the interrupt to the communication module                               */
/* returns nonzero if error                                             */
/************************************************************************/
int TWI_slave_send_message(uint8_t header, uint8_t data) {
	
	if (!slave_addressed) {
		return 0x08;
	}
	
	int err = TWI_slave_send_data(header, false);
	if (err) return err;
	
	err = TWI_slave_send_data(data, true);
	if (err) return err;
	
	err = TWI_slave_wait_for_stop(false);
	if (err) return err;
	
	return 0;
}

/************************************************************************/
/* Blocks until a message is received from the communication module via 
TWI                                                                     */
/* header[out]: the message header that was received                    */
/* data[out]: the message data                                          */
/* returns nonzero if error                                             */
/************************************************************************/
int TWI_slave_receive_message(uint8_t* header, uint8_t* data) {
	
	if (!slave_addressed) {
		return 0x09;
	}
	
	int err = TWI_slave_receive_data(header);
	if (err) return err;
	
	err = TWI_slave_receive_data(data);
	if (err) return err;
	
	err = TWI_slave_wait_for_stop(true);
	if (err) return err;
	
	return 0;
}