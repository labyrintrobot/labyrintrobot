/*
 * twi_common_private.c
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <asf.h>
#include "twi_common.h"

bool interrupt_enabled = false;

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