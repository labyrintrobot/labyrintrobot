/*
 * twi_common.c
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_common.h"

bool interrupt_enabled = false;

void TWI_disable_interrupt() {
	interrupt_enabled = TWCR & 0x01;
	TWCR &= 0xFE;
} 

void TWI_enable_interrupt() {
	if (interrupt_enabled) {
		TWCR |= 0x01;
	} else {
		TWCR &= 0xFE;
	}
}

void TWI_wait_for_TWINT() {
	while (!(TWCR & (1<<TWINT)));
}

bool TWI_invalid_status(uint8_t status) {
	return (TWSR & 0xF8) != TWI_DATA_REC_NACK_STATUS);
}

/************************************************************************/
/* Initializes TWI.                                                     */
/* my_address: the address of this module                               */
/* enable_interrupts: true if interrupts should be enabled when a 
message is received                                                     */
/* bitrate: the bitrate in kHz                                          */
/* returns nonzero if error                                             */
/************************************************************************/
int TWI_initialize(TWI_ADDRESS my_address, bool enable_interrupts, int bitrate) {
	
	if (bitrate == 5) {
		TWBR = 87; // Clockspeed
		TWSR |= 2; // Scale
	} else if (bitrate == 50) {
		TWBR = 33;
		TWSR &= 0b11111101;
		TWSR |= 1;
	} else if (bitrate == 200) {
		TWBR = 54;
		TWSR &= 0b11111100;
	} else if (bitrate == 400) {
		TWBR = 24;
		TWSR &= 0b11111100;
	} else {
		return 0x01;
	}
	
	TWAR = my_address & 0xFE;
	TWAMR = 0x00; // No filtering
	// TWEN = Enable TWI
	// TWEA = Enable acknowledgment of own address
	// TWIE = Enable interrupts
	if (enable_interrupts) {
		TWCR = (1<<TWEN) | (1<<TWEA) | (1<<TWIE);
	} else {
		TWCR = (1<<TWEN) | (1<<TWEA);
	}
	
	return 0;
}