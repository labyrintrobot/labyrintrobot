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

void TWI_slave_ACK(void);
void TWI_slave_NACK(void);
int TWI_slave_receive_address(bool* write);
int TWI_slave_receive_data(uint8_t* data);
int TWI_slave_send_data(uint8_t data, bool nack);
int TWI_slave_wait_for_stop(void);

enum TWI_STATUS {
	TWI_SLAR_ACK_STATUS = 0xA8,
	TWI_DATA_WRITE_ACK_STATUS = 0xB8,
	TWI_LAST_DATA_WRITE_ACK_STATUS = 0xC8,
	TWI_SLAW_ACK_STATUS = 0x60,
	TWI_DATA_REC_ACK_STATUS = 0x80,
	TWI_DATA_REC_NACK_STATUS = 0x88,
	TWI_REP_START_STOP_STATUS = 0xA0
	};
	
/************************************************************************/
/* Initializes the TWI clockspeed and slave address. Also enables TWI.  */
/* my_address: the TWI address of this  module                          */
/* bitrate: the bitrate in kHz                                          */
/************************************************************************/
int TWI_slave_initialize(TWI_MODULE_ADDRESS my_address, TWI_BITRATE bitrate) {
		
	int err = TWI_common_initialize(bitrate);
	if (err) return err;
	
	TWAR = my_address & 0xFE;
	TWAMR = 0x00; // No filtering
	
	return 0;
}

void TWI_slave_ACK() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
}

void TWI_slave_NACK() {
	TWCR = (1<<TWINT) | (1<<TWEN);
}

int TWI_slave_receive_address(bool* write) {
	
	TWI_common_wait_for_TWINT();
	
	bool invalid_slaw = TWI_common_invalid_status(TWI_SLAW_ACK_STATUS);
	bool invalid_slar = TWI_common_invalid_status(TWI_SLAR_ACK_STATUS);
	if (invalid_slaw && invalid_slar) {
		return 0x0A;
	}
	*write = ! invalid_slaw;
	
	TWI_slave_ACK();
	
	return 0;
}

int TWI_slave_receive_data(uint8_t* data) {
	TWI_common_wait_for_TWINT();
	
	if (TWI_common_invalid_status(TWI_DATA_REC_ACK_STATUS)) {
		return 0x0C;
	}
	*data = TWDR;
	
	TWI_slave_ACK();
	
	return 0;
}

int TWI_slave_send_data(uint8_t data, bool nack) {
	TWDR = data; // Write data to register
	if (nack) {
		TWI_slave_NACK();
	} else {
		TWI_slave_ACK();
	}
	
	TWI_common_wait_for_TWINT();
	
	if (nack) {
		if (TWI_common_invalid_status(TWI_LAST_DATA_WRITE_ACK_STATUS)) {
			return 0x0D;
		}
	} else {
		if (TWI_common_invalid_status(TWI_DATA_WRITE_ACK_STATUS)) {
			return 0x0E;
		}
	}

	return 0;
}

int TWI_slave_wait_for_stop() {
	TWI_common_wait_for_TWINT();
	
	if (TWI_common_invalid_status(TWI_REP_START_STOP_STATUS)) {
		return 0x0F;
	}
	
	TWI_slave_ACK();
	
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

	bool write;
	int err = TWI_slave_receive_address(&write);
	stop_sending_irq_fn();
	if (err) return err;
	
	if (write) {
		return 0x10;
	}
	
	err = TWI_slave_send_data(header, false);
	if (err) return err;
	
	err = TWI_slave_send_data(data, true);
	if (err) return err;
	
	err = TWI_slave_wait_for_stop();
	if (err) return err;
	
	//TWI_common_enable_interrupt();
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
		return 0x11;
	}
	
	err = TWI_slave_receive_data(header);
	if (err) return err;
	
	err = TWI_slave_receive_data(data);
	if (err) return err;
	
	err = TWI_slave_wait_for_stop();
	if (err) return err;
	
	//TWI_common_enable_interrupt();
	
	return 0;
}