/*
 * twi_master.h
 *
 * Created: 11/14/2013 6:00:15 PM
 *  Author: Emil Berg, Kristoffer Borg
 */

#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

#include "twi_common.h"

int TWI_master_initialize(TWI_MODULE_ADDRESS my_address);

int TWI_master_send_message(TWI_MODULE_ADDRESS to_address, uint8_t header, uint8_t data);

int TWI_master_receive_message(TWI_MODULE_ADDRESS from_address, uint8_t* header, uint8_t* data);

#endif /* TWI_MASTER_H_ */