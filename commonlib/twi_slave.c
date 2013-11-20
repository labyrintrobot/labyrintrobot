/*
 * twi_slave.c
 *
 * Created: 11/14/2013 5:47:41 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_slave.h"
#include "twi_common_private.h"

int TWI_slave_receive_address(bool* write);
int TWI_slave_receive_data(uint8_t* data, bool nack);
int TWI_slave_send_data(uint8_t data, bool nack);

enum TWI_STATUS {
	TWI_SLAR_ACK_STATUS = 0xA8,
	TWI_DATA_WRITE_ACK_STATUS = 0xB8,
	TWI_LAST_DATA_WRITE_ACK_STATUS = 0xC8,
	TWI_SLAW_ACK_STATUS = 0x60,
	TWI_DATA_REC_ACK_STATUS = 0x80,
	TWI_DATA_REC_NACK_STATUS = 0x88,
	TWI_REP_START_STOP_STATUS = 0xA0
	};

int TWI_slave_receive_address(bool* write) {
	TWI_common_wait_for_TWINT();
	bool invalid_slaw = TWI_common_invalid_status(TWI_SLAW_ACK_STATUS);
	bool invalid_slar = TWI_common_invalid_status(TWI_SLAR_ACK_STATUS);
	if (invalid_slaw && invalid_slar) {
		return 0x0A;
	}
	*write = ! invalid_slaw;
	
	return 0;
}

int TWI_slave_receive_data(uint8_t* data, bool nack) {
	TWI_common_wait_for_TWINT();
	
	if(nack) {
		if (TWI_common_invalid_status(TWI_DATA_REC_NACK_STATUS)) {
			return 0x0B;
		}
		
	} else {
		if (TWI_common_invalid_status(TWI_DATA_REC_ACK_STATUS)) {
			return 0x0C;
		}
	}
	*data = TWDR;
	return 0;
}

int TWI_slave_send_data(uint8_t data, bool nack) {
	TWDR = data; // Write data to register
	if (nack) {
		TWCR = (1<<TWINT) | (1<<TWEN) | (1 << TWEA); // Send out data
		if (TWI_common_invalid_status(TWI_LAST_DATA_WRITE_ACK_STATUS)) {
			return 0x0D;
		}
	} else {
		TWCR = (1<<TWINT) | (1<<TWEN); // Send out data
		if (TWI_common_invalid_status(TWI_DATA_WRITE_ACK_STATUS)) {
			return 0x0E;
		}
	}

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
int TWI_slave_send_message(uint8_t header, uint8_t data, void (*start_sending_irq_fn)(void), void (*stop_sending_irq_fn)(void)) {
	
	//TWI_common_disable_interrupt();
	start_sending_irq_fn();

	PORTA = 0x10;
	TWI_MODULE_ADDRESS address;

	bool write;
	int err = TWI_slave_receive_address(&write);
	stop_sending_irq_fn();
	PORTA = 0x30;
	if (err) return err;
	
	if (write) {
		return 0x0F;
	}
	
	err = TWI_slave_send_data(header, false);
	if (err) return err;
	
	err = TWI_slave_send_data(data, true);
	if (err) return err;
	
	//TWI_common_enable_interrupt();
	PORTA = 0x70;
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
	
	//TWI_common_disable_interrupt();
	
	bool write;
	int err = TWI_slave_receive_address(&write);
	if (err) return err;
		
	if (! write) {
		return 0x10;
	}
	
	err = TWI_slave_receive_data(header, false);
	if (err) return err;
	
	err = TWI_slave_receive_data(data, true);
	if (err) return err;
	
	//TWI_common_enable_interrupt();
	
	return 0;
}