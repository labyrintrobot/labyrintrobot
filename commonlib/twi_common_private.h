/*
 * twi_common_private.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef TWI_COMMON_PRIVATE_H_
#define TWI_COMMON_PRIVATE_H_

#include <stdbool.h>

void TWI_common_disable_interrupt(void);

void TWI_common_enable_interrupt(void);

void TWI_common_wait_for_TWINT(void);

bool TWI_common_invalid_status(uint8_t status);

#endif /* TWI_COMMON_PRIVATE_H_ */