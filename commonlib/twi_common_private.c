/*
 * twi_common_private.c
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_common_private.h"

/************************************************************************/
/* Initializes TWI.                                                     */
/* my_address: the address of this module                               */
/* enable_interrupts: true if interrupts should be enabled when a
message is received                                                     */
/* bitrate: the bitrate in kHz                                          */
/* returns nonzero if error                                             */
/************************************************************************/
int TWI_common_initialize(TWI_MODULE_ADDRESS my_address, int bitrate) {
	
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
	TWDR = 0xFF; // Clean TWDR
	
	return 0;
}

void TWI_common_wait_for_TWINT() {
	while (!(TWCR & (1<<TWINT)));
}

bool TWI_common_invalid_status(uint8_t status) {
	return ((TWSR & 0xF8) != status);
}