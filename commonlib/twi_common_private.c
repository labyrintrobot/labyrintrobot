/*
 * twi_common_private.c
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_common_private.h"

bool interrupt_enabled = false;

int TWI_common_initialize(TWI_BITRATE bitrate) {
	
	switch (bitrate) {
		case (TWI_BITRATE_5_KHZ):
			TWBR = 87; // Clockspeed
			TWSR |= 2; // Scale
		break;
		
		case (TWI_BITRATE_50_KHZ):
			TWBR = 33;
			TWSR &= 0b11111101;
			TWSR |= 1;
		break;
		
		case (TWI_BITRATE_200_KHZ):
			TWBR = 54;
			TWSR &= 0b11111100;
		break;
		
		case (TWI_BITRATE_400_KHZ):
			TWBR = 24;
			TWSR &= 0b11111100;
		break;
		
		default:
			return 0x01;
		break;
	}
	
	TWDR = 0xFF; // Fill data register with garbage
	
	return 0;
}

void TWI_common_disable_interrupt() {
	interrupt_enabled = TWCR & 0x01;
	TWCR &= 0xFE;
} 

void TWI_common_enable_interrupt() {
	if (interrupt_enabled) {
		TWCR |= 0x01;
	} else {
		TWCR &= 0xFE;
	}
}

void TWI_common_wait_for_TWINT() {
	while (!(TWCR & (1<<TWINT)));
}

bool TWI_common_invalid_status(uint8_t status) {
	return ((TWSR & 0xF8) != status);
}