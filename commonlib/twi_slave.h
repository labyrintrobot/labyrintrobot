/*
 * twi_slave.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */

#ifndef TWI_SLAVE_H_
#define TWI_SLAVE_H_

#include <stdbool.h>
#include "twi_common.h"

int TWI_slave_initialize(TWI_MODULE_ADDRESS my_address, int bitrate);

int TWI_slave_wait_for_address(bool* should_receive_out);

int TWI_slave_send_message(uint8_t header, uint8_t data);

int TWI_slave_receive_message(uint8_t* header_out, uint8_t* data_out);

#endif /* TWI_SLAVE_H_ */